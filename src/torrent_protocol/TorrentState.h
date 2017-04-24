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
#include <atomic>
#include <memory>

#include "PieceMgr.h"

class TorrentFile;
namespace network { class Peer; }

/**
 * @class TorrentState
 * @brief Manages the state of the torrent - number of peers connected,
 *        pieces being downloaded, etc.
 */
class TorrentState
{
    friend class network::Peer;
public:
    /// TorrentState constructor - requires torrent file path
    explicit TorrentState(const std::string &torrentFilePath);

    /// Returns a reference to the torrent file pointer
    std::shared_ptr<TorrentFile> &getTorrentFile();

    /// Returns the name of the ".torrent" file from which the torrent data was found
    const std::string &getTorrentFileName();

    /// Returns the number of connected peers that are associated with the torrent
    uint32_t getNumPeers();

    /// Returns the number of pieces that have been downloaded and verified
    uint64_t getNumPiecesHave() { return m_pieceMgr.getNumPiecesHave(); }

    /// Returns true if the client has the given piece of the torrent data, false if else
    bool havePiece(uint32_t pieceIdx) const { return m_pieceMgr.havePiece(pieceIdx); }

    /// Returns the index of the piece currently being downloaded (or about to be downloaded)
    const uint32_t &getCurrentPieceNum() { return m_pieceMgr.getCurrentPieceNum(); }

    /// Returns the total number of bytes uploaded to peers
    const uint64_t &getNumBytesUploaded() const { return m_pieceMgr.getNumBytesUploaded(); }

    /// Returns true if the integrity of the file is confirmed, false if else.
    bool verifyFile() { return m_pieceMgr.verifyFile(); }

protected:
    /// Called when a new peer has been associated with this torrent object
    void incrementPeerCount();

    /// Called when a peer has been disassociated from this torrent object
    void decrementPeerCount();

    /// Returns true if it will be allowed to unchoke another peer, false if else
    bool canUnchokePeer();

    /// Called when a peer has been choked (increments count of peers to allow to be unchoked)
    void onPeerChoked();

    /// Sets the flag for the piece at the given index as being available for downloading from a peer
    void markPieceAvailable(uint32_t pieceIdx) { m_pieceMgr.markPieceAvailable(pieceIdx); }

    /// Reads the bitset from a peer of pieces they do or dont have, setting the set of available
    /// pieces to the union of the currently available pieces and the peer's pieces
    void readPeerBitset(const boost::dynamic_bitset<> &set) { m_pieceMgr.readPeerBitset(set); }

    /// Returns a bitset of the pieces that the client has
    const boost::dynamic_bitset<> &getBitsetHave() const { return m_pieceMgr.getBitsetHave(); }

    /// Returns a pointer to a torrent fragment structure that needs to be downloaded.
    /// If the fragments associated with the current piece have already been assigned,
    /// returns a null pointer.
    std::shared_ptr<TorrentFragment> getFragmentToDownload() { return m_pieceMgr.getFragmentToDownload(); }

    /// If the client has the given fragment, it will return a shared pointer to a structure containing its data.
    /// Otherwise, returns a null pointer
    std::shared_ptr<TorrentFragment> getFragmentToUpload(uint32_t pieceIdx, uint32_t offset, uint32_t length) { return m_pieceMgr.getFragmentToUpload(pieceIdx, offset, length); }

    /// Called by a peer once a fragment has been downloaded in its entirety
    void onFragmentDownloaded(uint32_t pieceIdx) { m_pieceMgr.onFragmentDownloaded(pieceIdx); }

private:
    /// Shared pointer to the torrent file
    std::shared_ptr<TorrentFile> m_file;

    /// Current number of peers which the client is connected to for the torrent file
    std::atomic<uint32_t> m_numPeers;

    /// Counter of peers that will be allowed to be unchoked
    std::atomic<uint32_t> m_numPeersCanUnchoke;

    /// True if download is complete, false if else
    bool m_downloadComplete;

    /// Piece selection and download manager
    PieceMgr m_pieceMgr;

    /// Name of the ".torrent" file itself (used in graphical interface)
    std::string m_torrentFileName;
};
