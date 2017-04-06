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
#include <boost/dynamic_bitset.hpp>
#include <cstdint>
#include <memory>
#include <unordered_map>

class TorrentFile;

class TorrentState
{
public:
    /// TorrentState constructor - requires torrent file path
    explicit TorrentState(const std::string &torrentFilePath);

    /// Returns a reference to the torrent file pointer
    std::shared_ptr<TorrentFile> &getTorrentFile();

    /// Adds the IP address and port number of a peer to the map of new peers,
    /// if applicable.
    void addPeerInfo(uint32_t ipAddr, uint16_t portNum);

private:
    /// Shared pointer to the torrent file
    std::shared_ptr<TorrentFile> m_file;

    /// Bitset representing pieces of the torrent that have or haven't yet been
    /// downloaded. 1 = Downloaded, 0 = Not Downloaded
    boost::dynamic_bitset<> m_pieceInfo;

    /// Map of new peers (no connection attempted yet). Key = IP, Value = Port
    std::unordered_map<uint32_t, uint16_t> m_newPeers;

    /// Map of known peers. Key = IP, Value = Port
    std::unordered_map<uint32_t, uint16_t> m_knownPeers;

    //std::ofstream m_diskFile; -- figure out multi file mode downloads
};
