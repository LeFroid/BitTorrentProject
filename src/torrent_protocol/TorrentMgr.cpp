/*
Copyright (c) 2017, Timothy Vaccarelli
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <cstring>

#include <algorithm>
#include <functional>
#include <random>

#include "LogHelper.h"

#include "TorrentMgr.h"
#include "TorrentFile.h"

const char *PeerNameVersion = "-BTP001-";

TorrentMgr::TorrentMgr(const std::string &configFile) :
    m_ioService(),
    m_signalSet(m_ioService, SIGINT, SIGTERM),
    m_ioThread(),
    m_torrentMap(),
    m_trackerTimers(),
    m_connectionMgr(std::make_shared< network::ConnectionMgr<network::Peer> >(m_ioService)),
    m_trackerMgr(m_ioService),
    m_peerListener(m_ioService),
    m_config()
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> peerIDDist(0, 255);

    // Set peer ID
    memcpy(&m_peerID[0], &PeerNameVersion[0], 8);
    for (int i = 8; i < 20; ++i)
        m_peerID[i] = static_cast<char>(peerIDDist(gen));

    // Load configuration file
    m_config.loadFile(configFile);
}

TorrentMgr::~TorrentMgr()
{
    if (m_ioThread.get_id() != std::thread::id())
        m_ioThread.join();
}

void TorrentMgr::run()
{
    // Check if thread already running - if not, will have invalid thread id
    if (m_ioThread.get_id() != std::thread::id())
        return;

    m_ioThread = std::thread([this]()
        {
            // Tell signal set to halt io service when caught
            this->m_signalSet.async_wait(std::bind(&boost::asio::io_service::stop, &m_ioService));

            // Run tracker connection manager and peer connection manager
            m_trackerMgr.run();
            m_connectionMgr->run();

            // Start tcp listener
            auto listenPort = m_config.getValue<int>("network.listen_port");
            auto maxConnections = m_config.getValue<int>("network.max_pending_connections");
            if (listenPort && maxConnections)
                m_peerListener.start(*listenPort, *maxConnections, m_connectionMgr);
            this->m_ioService.run();
        }
    );
}

std::shared_ptr<TorrentState> TorrentMgr::addTorrent(const std::string &torrentPath)
{
    auto retVal = std::make_shared<TorrentState>(torrentPath);

    // Check that the torrent file is valid (if file size == 0, invalid)
    auto &torrentRef = retVal->getTorrentFile();
    if (torrentRef->getFileSize() > 0)
    {
        // Add to map, begin download process, return shared_ptr
        {
            std::lock_guard<std::mutex> lock(m_torrentLock);
            m_torrentMap[torrentRef->getInfoHash()] = retVal;
        }


        // Add new deadline timer for connecting to tracker
        std::unique_ptr<boost::asio::deadline_timer> trackerTimer(new boost::asio::deadline_timer(m_ioService));
        m_trackerTimers.push_back(std::move(trackerTimer));

        // Connect to tracker immediately, then every few minutes
        connectToTracker(m_trackerTimers.at(m_trackerTimers.size() - 1).get(), torrentRef->getInfoHash());

        /*if (retVal->verifyFile())
            LOG_INFO("torrent_protocol.mgr", "Each piece of file verified!");
        else
            LOG_INFO("torrent_protocol.mgr", "Could not verify file!");*/
        return retVal;
    }

    return std::shared_ptr<TorrentState>(nullptr);
}

bool TorrentMgr::hasTorrentFile(uint8_t *infoHash)
{
    std::lock_guard<std::mutex> lock(m_torrentLock);
    return (m_torrentMap.find(infoHash) != m_torrentMap.end());
}

std::shared_ptr<TorrentState> TorrentMgr::getTorrentState(uint8_t *infoHash)
{
    std::lock_guard<std::mutex> lock(m_torrentLock);
    auto it = m_torrentMap.find(infoHash);
    if (it != m_torrentMap.end())
        return it->second;

    return std::shared_ptr<TorrentState>(nullptr);
}

const char *TorrentMgr::getPeerID() const
{
    return m_peerID;
}

std::string TorrentMgr::getDownloadDirectory()
{
    if (auto dir = m_config.getValue<std::string>("disk.download_dir"))
        return *dir;
    return std::string();
}

void TorrentMgr::setDownloadDirectory(const std::string &dir)
{
    m_config.setValue<std::string>("disk.download_dir", dir);
}

void TorrentMgr::connectToTracker(boost::asio::deadline_timer *timer, uint8_t *infoHash)
{
    if (timer == nullptr)
        return;

    auto torrent = m_torrentMap.find(infoHash);
    if (torrent == m_torrentMap.end())
        return;

    std::shared_ptr<TorrentState> torrentPtr = torrent->second;

    // Instantiate tracker client
    auto trackerClient = std::make_shared<network::TrackerClient>(m_ioService, network::Socket::Mode::TCP);
    trackerClient->setPeerID(m_peerID);
    trackerClient->setTorrentState(torrentPtr);
    trackerClient->setConnectionMgr(m_connectionMgr);

    // Connect client to tracker service
    auto trackerEndpoint = trackerClient->findTrackerEndpointTCP();
    trackerClient->connect(trackerEndpoint);

    // Add tracker client to its connection manager
    m_trackerMgr.addConnection(trackerClient);

    timer->expires_from_now(boost::posix_time::minutes(5));
    timer->async_wait(std::bind(&TorrentMgr::connectToTracker, this, timer, infoHash));
}
