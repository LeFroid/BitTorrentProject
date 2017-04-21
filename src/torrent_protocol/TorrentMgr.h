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

#pragma once

#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <memory>
#include <thread>
#include <unordered_map>

#include "Configuration.h"
#include "ConnectionMgr.h"
#include "Listener.h"
#include "Peer.h"
#include "TorrentState.h"
#include "TrackerClient.h"

#include "HashMapUtils.h"

class TorrentState;

/**
 * @class TorrentMgr
 * @brief Responsible for asynchronous transfer of torrent files between client
 *        and peers.
 */
class TorrentMgr
{
public:
    /// TorrentMgr constructor
    TorrentMgr(const std::string &configFile);

    /// TorrentMgr destructor
    ~TorrentMgr();

    /// Runs the io_service in its own thread, allowing asynchronous network operations to run
    void run();

public:
    /// Adds a torrent to the map of torrent files being downloaded. If the file
    /// associated with the given path is valid, the torrent file will begin downloading
    /// immediately.
    std::shared_ptr<TorrentState> addTorrent(const std::string &torrentPath);

    /// Returns true if the client has a torrent file associated with the given infoHash, false if else.
    /// Used when a peer initiates a handshake with us
    bool hasTorrentFile(uint8_t *infoHash);

    /// Returns the torrent state associated with the infoHash
    std::shared_ptr<TorrentState> getTorrentState(uint8_t *infoHash);

    /// Returns a pointer to the client's peer id
    const char *getPeerID() const;

    /// Returns the directory of which torrent files are to be downloaded
    std::string getDownloadDirectory();

    /// Sets the directory for torrent files to be downloaded into
    void setDownloadDirectory(const std::string &dir);

private:
    /// Searches for the torrent with the given info hash, attempting to connect to its tracker service and
    /// find peers. Runs every 5 minutes by default
    void connectToTracker(boost::asio::deadline_timer *timer, uint8_t *infoHash);

private:
    /// I/O service used for networking
    boost::asio::io_service m_ioService;

    /// Signal set that, when caught, will halt the io service
    boost::asio::signal_set m_signalSet;

    /// Thread of execution that runs io_service
    std::thread m_ioThread;

    /// Hash map of info_hashes to shared_ptr's of TorrentStates
    std::unordered_map< uint8_t*, std::shared_ptr<TorrentState>, DigestHasher, DigestCompare > m_torrentMap;

    /// Timers to re-connect to tracker services
    std::vector< std::unique_ptr<boost::asio::deadline_timer> >m_trackerTimers;

    /// Lock used when accessing torrent map
    std::mutex m_torrentLock;

    /// Global peer id for client
    char m_peerID[20];

    /// Peer connection manager
    std::shared_ptr< network::ConnectionMgr<network::Peer> > m_connectionMgr;

    /// Tracker connection manager
    network::ConnectionMgr<network::TrackerClient> m_trackerMgr;

    /// Incoming peer listener
    network::Listener m_peerListener;

    /// Configuration data
    Configuration m_config;
};

extern TorrentMgr eTorrentMgr;
