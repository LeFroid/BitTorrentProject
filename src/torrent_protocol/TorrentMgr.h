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

#include "HashMapUtils.h"

namespace network { class TrackerClient; }

class TorrentState;

class TorrentMgr
{
public:
    /// TorrentMgr constructor
    TorrentMgr();

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
    bool hasTorrentFile(uint8_t *infoHash) const;

private:
    /// Checks for stale connections between the client and a tracker service
    void checkTrackerConnections(const boost::system::error_code &ec);

private:
    /// I/O service used for networking
    boost::asio::io_service m_ioService;

    /// Signal set that, when caught, will halt the io service
    boost::asio::signal_set m_signalSet;

    /// Thread of execution that runs io_service
    std::thread m_ioThread;

    /// Hash map of info_hashes to shared_ptr's of TorrentStates
    std::unordered_map< uint8_t*, std::shared_ptr<TorrentState>, DigestHasher, DigestCompare > m_torrentMap;

    /// Container of tracker clients being used to get peer information
    std::vector< std::shared_ptr<network::TrackerClient> > m_trackerClients;

    /// Lock for access to tracker client container
    std::mutex m_trackerLock;

    /// Timer used to check for stale tracker client connections
    boost::asio::deadline_timer m_trackerTimer;

    /// Global peer id for client
    char m_peerID[20];
};
