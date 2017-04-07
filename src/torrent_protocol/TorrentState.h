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

    /// Returns the index of the piece currently being downloaded (or about to be downloaded)
    uint64_t getCurrentPieceNum() { return 0; }
    // use piece selection algorithm to order pieces, push onto a stack, pop and return value here once the
    // last selected piece has been downloaded in its entirety (invisible to the caller)

    /// Returns true if the client has the given piece of the torrent data, false if else
    bool havePiece(uint64_t pieceIdx) const;

private:
    /// Shared pointer to the torrent file
    std::shared_ptr<TorrentFile> m_file;

    /// Bitset representing pieces of the torrent that have or haven't yet been
    /// downloaded. 1 = Downloaded, 0 = Not Downloaded
    boost::dynamic_bitset<> m_pieceInfo;

    //std::ofstream m_diskFile; -- figure out multi file mode downloads
};
