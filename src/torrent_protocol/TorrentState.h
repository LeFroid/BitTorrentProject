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

class TorrentFile;

class TorrentState
{
public:
    /// TorrentState constructor - requires torrent file path
    explicit TorrentState(const std::string &torrentFilePath);

    /// Returns a reference to the torrent file pointer
    std::shared_ptr<TorrentFile> &getTorrentFile();

    /// Returns the index of the piece currently being downloaded (or about to be downloaded)
    const uint32_t &getCurrentPieceNum();
    // use piece selection algorithm to order pieces, push onto a stack, pop and return value here once the
    // last selected piece has been downloaded in its entirety (invisible to the caller)

    /// Returns true if the client has the given piece of the torrent data, false if else
    bool havePiece(uint32_t pieceIdx) const;

    /// Sets the flag for the piece at the given index as being available for downloading from a peer
    void markPieceAvailable(uint32_t pieceIdx);

    /// Reads the bitset from a peer of pieces they do or dont have, setting the set of available
    /// pieces to the union of the currently available pieces and the peer's pieces
    void readPeerBitset(const boost::dynamic_bitset<> &set);

    //note to self:
    // when a piece is downloaded, set m_downloadComplete to m_pieceInfo.all() (true if all bits are set)

private:
    /// Based on the pieces already downloaded, and the pieces that are avaialable, returns the index of the
    /// next piece that should be downloaded. If the download is already finished, or there are no peers
    /// that have any pieces needed by the client, returns the total number of pieces + 1
    uint32_t determineNextPiece();

private:
    /// Shared pointer to the torrent file
    std::shared_ptr<TorrentFile> m_file;

    /// Bitset representing pieces of the torrent that have or haven't yet been
    /// downloaded. 1 = Downloaded, 0 = Not Downloaded
    boost::dynamic_bitset<> m_pieceInfo;

    /// Bitset which is the union of peer's bitsets representing the pieces they have
    boost::dynamic_bitset<> m_piecesAvailable;

    /// Current piece either being downloaded or about to be downloaded
    uint32_t m_currentPiece;

    /// True if download is complete, false if else
    bool m_downloadComplete;

    //std::ofstream m_diskFile; -- figure out multi file mode downloads
};
