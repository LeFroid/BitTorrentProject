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

#include <boost/dynamic_bitset.hpp>
#include <fstream>
#include <memory>
#include <vector>

#include "TorrentFragment.h"

class TorrentFile;
class TorrentState;
namespace bencoding { class BenString; }

/**
 * @class PieceMgr
 * @brief Responsible for storing information on the state of each piece in
 *        a torrent file, the pieces that have been downloaded, the pieces
 *        that are being downloaded, the pieces that are available, which
 *        piece to download next and how to assign the fragments of each
 *        piece to the appropriate peers.
 */
class PieceMgr
{
    friend class TorrentState;

public:
    /**
     * @brief Constructs the piece manager with the given parameters
     * @param torrentFile Pointer to the torrent file
     */
    explicit PieceMgr(std::shared_ptr<TorrentFile> torrentFile);

    /// Destructor
    ~PieceMgr();

    /// Returns true if the piece is available on the client side, false if else
    bool havePiece(uint32_t pieceIdx) const;

    /// Returns the index of the current piece to be downloaded
    const uint32_t &getCurrentPieceNum();

    /// Returns the total number of pieces that have been downloaded & verified
    uint64_t getNumPiecesHave();

    /// Returns a bitset of the pieces that the client has
    const boost::dynamic_bitset<> &getBitsetHave() const;

    /// Returns the number of bytes uploaded to other peers
    const uint64_t &getNumBytesUploaded() const;

    /// Verifies the contents of the torrent file, returning true on successful verification
    bool verifyFile();

    //todo: ability to serialize the pieces already verified and downloaded to a config file,
    //      so application can resume downloading after program exits

protected:
    /// Sets the flag for the piece at the given index as being available for downloading from a peer
    void markPieceAvailable(const uint32_t &pieceIdx);

    /// Reads the bitset from a peer of pieces they do or dont have, setting the set of available
    /// pieces to the union of the currently available pieces and the peer's pieces
    void readPeerBitset(const boost::dynamic_bitset<> &set);

    /// Returns a pointer to a torrent fragment structure that needs to be downloaded.
    /// If the fragments associated with the current piece have already been assigned,
    /// returns a null pointer.
    std::shared_ptr<TorrentFragment> getFragmentToDownload();

    /// If the client has the given fragment, it will return a shared pointer to a structure containing its data.
    /// Otherwise, returns a null pointer
    std::shared_ptr<TorrentFragment> getFragmentToUpload(uint32_t pieceIdx, uint32_t offset, uint32_t length);

    /// Called by a peer once a fragment has been downloaded in its entirety
    void onFragmentDownloaded(uint32_t pieceIdx);

private:
    /// Returns the number of fragments that make up the piece with the given index
    uint32_t getNumFragments(uint32_t pieceIdx);

    /// Called after a piece has been downloaded in its entirety, calculating the next piece to be downloaded
    void determineNextPiece();

    /// Called after all fragments of a piece have finished downloading
    void onAllFragmentsDownloaded();

    /// Writes the verified piece onto the disk, given a pointer to the data and its length in bytes
    void writePieceToDisk(uint8_t *data, size_t pieceLength);

    /// Initializes the file handle which is used in single-file torrents
    void initializeSingleFileHandle();

private:
    /// Number of bytes per typical piece
    uint32_t m_pieceLength;

    /// Number of bytes in the final piece of the torrent
    int64_t m_finalPieceLen;

    /// Number of fragments per typical piece
    uint32_t m_fragmentsPerPiece;

    /// Number of fragments in the final piece of the torrent
    uint32_t m_fragmentsInFinalPiece;

    /// Size of the torrent file
    uint64_t m_fileSize;

    /// Number of bytes uploaded to remote peers (info sent to tracker)
    uint64_t m_bytesUploaded;

    /// Shared pointer to the torrent file
    std::shared_ptr<TorrentFile> m_torrentFile;

    /// Bencoded string of SHA-1 hash values of each piece in the torrent file
    std::shared_ptr<bencoding::BenString> m_digestString;

    /// Index of the current piece being downloaded
    uint32_t m_currentPiece;

    /// Bitset representing pieces of the torrent that have or haven't yet been
    /// downloaded. 1 = Downloaded, 0 = Not Downloaded
    boost::dynamic_bitset<> m_pieceInfo;

    /// Bitset which is the union of peer's bitsets representing the pieces they have
    boost::dynamic_bitset<> m_piecesAvailable;

    /// Stores the blocks/fragments of the piece currently being downloaded
    std::vector< std::shared_ptr<TorrentFragment> >m_pieceBeingDownloaded;

    /// Number of peers downloading the fragments in m_pieceBeingDownloaded
    uint32_t m_numPeersDownloading;

    /// Number of peers that have finished downloading a fragment in m_pieceBeingDownloaded
    uint32_t m_numPeersFinishedFragment;

    /// Used to synchronize requests about piece downloading or information
    std::mutex m_pieceLock;

    /// Handle used when performing I/O in single file download mode
    std::fstream m_singleFileHandle;
};
