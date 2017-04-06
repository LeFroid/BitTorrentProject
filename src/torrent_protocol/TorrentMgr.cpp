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

#include "LogHelper.h"

#include "ConnectionMgr.h"
#include "Listener.h"

#include "TorrentMgr.h"
#include "TorrentState.h"
#include "TorrentFile.h"

#include "TrackerClient.h"

const char *PeerNameVersion = "-BTP001-";

TorrentMgr::TorrentMgr() :
    m_ioService(),
    m_signalSet(m_ioService, SIGINT, SIGTERM),
    m_ioThread(),
    m_torrentMap(),
    m_trackerClients(),
    m_trackerLock(),
    m_trackerTimer(m_ioService),
    m_connectionMgr(),
    m_peerListener(m_ioService)
{
    // Set peer ID
    memcpy(&m_peerID[0], &PeerNameVersion[0], 8);
    for (int i = 8; i < 20; ++i)
        m_peerID[i] = rand() % 256;
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

            // Begin timer to check for stale tracker connections
            this->m_trackerTimer.expires_from_now(boost::posix_time::seconds(5));
            this->m_trackerTimer.async_wait(std::bind(&TorrentMgr::checkTrackerConnections, this, std::placeholders::_1));
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
        m_torrentMap[torrentRef->getInfoHash()] = retVal;

        // Instantiate tracker client
        auto trackerClient = std::make_shared<network::TrackerClient>(m_ioService, network::Socket::Mode::TCP);
        trackerClient->setPeerID(m_peerID);
        trackerClient->setTorrentState(retVal);

        // Connect client to tracker service
        auto trackerEndpoint = trackerClient->findTrackerEndpointTCP();
        trackerClient->connect(trackerEndpoint);

        // Push tracker client into container, return torrent state
        {
            std::lock_guard<std::mutex> lock(m_trackerLock);
            m_trackerClients.push_back(trackerClient);
        }
        return retVal;
    }

    return std::shared_ptr<TorrentState>(nullptr);
}

bool TorrentMgr::hasTorrentFile(uint8_t *infoHash) const
{
    return (m_torrentMap.find(infoHash) != m_torrentMap.end());
}

void TorrentMgr::checkTrackerConnections(const boost::system::error_code &ec)
{
    if (ec)
    {
        LOG_WARNING("torrent_protocol.asio", "Error in TorrentMgr::checkTrackerConnections timer, message: ", ec.message());
        return;
    }

    std::lock_guard<std::mutex> lock(m_trackerLock);
    m_trackerClients.erase(std::remove_if(m_trackerClients.begin(), m_trackerClients.end(), [this](std::shared_ptr<network::TrackerClient> client)
    {
        if (client.get())
            if (client->isClosing())
                return true;
        return false;
    }), m_trackerClients.end());

    // Reset timer
    m_trackerTimer.expires_from_now(boost::posix_time::seconds(5));
    m_trackerTimer.async_wait(std::bind(&TorrentMgr::checkTrackerConnections, this, std::placeholders::_1));
}
