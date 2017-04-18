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

#include <cmath>
#include <cstdlib>

#include <algorithm>

#include "PieceMgr.h"

#include "BenString.h"
#include "SHA1Hash.h"
#include "LogHelper.h"

const static uint32_t DefaultFragmentLength = 16384;

PieceMgr::PieceMgr(uint32_t pieceLength, uint64_t numPieces, uint64_t fileSize, std::shared_ptr<bencoding::BenString> digestString) :
    m_pieceLength(pieceLength),
    m_fragmentsPerPiece(0),
    m_fileSize(fileSize),
    m_digestString(digestString),
    m_currentPiece(0),
    m_pieceInfo(numPieces),
    m_piecesAvailable(numPieces),
    m_pieceBeingDownloaded(),
    m_numPeersDownloading(0),
    m_numPeersFinishedFragment(0)
{
    m_fragmentsPerPiece = std::ceil(double(pieceLength) / DefaultFragmentLength);
}

bool PieceMgr::havePiece(uint32_t pieceIdx) const
{
    // Make sure index is valid
    if (pieceIdx >= m_pieceInfo.size())
        return false;
    return m_pieceInfo[pieceIdx];
}

const uint32_t &PieceMgr::getCurrentPieceNum()
{
    std::lock_guard<std::mutex> lock(m_pieceLock);

    if ((!m_pieceInfo.all() && m_pieceInfo[m_currentPiece]) || !m_piecesAvailable[m_currentPiece])
        determineNextPiece();

    return m_currentPiece;
}

void PieceMgr::markPieceAvailable(const uint32_t &pieceIdx)
{
    // Make sure index is valid
    if (pieceIdx >= m_piecesAvailable.size())
        return;

    m_piecesAvailable[pieceIdx] = true;
}

void PieceMgr::readPeerBitset(const boost::dynamic_bitset<> &set)
{
    m_piecesAvailable |= set;
}

TorrentFragment *PieceMgr::getFragmentToDownload()
{
    std::lock_guard<std::mutex> lock(m_pieceLock);

    TorrentFragment *fragPtr = nullptr;

    auto activeFragCount = m_numPeersDownloading + m_numPeersFinishedFragment;
    if (activeFragCount < m_pieceBeingDownloaded.size())
    {
        fragPtr = &(m_pieceBeingDownloaded[activeFragCount]);
        ++m_numPeersDownloading;
    }

    return fragPtr;
}

void PieceMgr::onFragmentDownloaded()
{
    std::lock_guard<std::mutex> lock(m_pieceLock);

    // Sanity check
    if (m_numPeersDownloading == 0)
        return;

    // Decrement the count of peers still downloading fragments, increment the completed frag. download counter
    --m_numPeersDownloading;
    ++m_numPeersFinishedFragment;

    // Now check if all fragments have been downloaded
    if (m_numPeersFinishedFragment == m_pieceBeingDownloaded.size())
        onAllFragmentsDownloaded();
}

uint32_t PieceMgr::getNumFragments(uint32_t pieceIdx)
{
    // calculate number of fragments in the last piece
    if (pieceIdx + 1 >= m_pieceInfo.size())
    {
        int64_t finalPieceLen = m_pieceInfo.size() * m_pieceLength - m_fileSize;
        if (finalPieceLen < 0)
            finalPieceLen *= -1;
        if (DefaultFragmentLength > finalPieceLen)
            return 1;
        return (uint32_t)ceil(double(finalPieceLen) / DefaultFragmentLength);
    }

    return m_fragmentsPerPiece;
}

void PieceMgr::determineNextPiece()
{
    //TODO: First piece: randomly chosen, after that use rarest first algorithm
    uint32_t maybePiece;
    auto numPieces = m_pieceInfo.size();

    boost::dynamic_bitset<> piecesToChoose = (m_piecesAvailable & (~m_pieceInfo));
    if (!piecesToChoose.any())
        m_currentPiece = numPieces + 1;

    while (true)
    {
        maybePiece = uint64_t(rand()) % numPieces;
        if (piecesToChoose[maybePiece])
        {
            m_currentPiece = maybePiece;

            // Reset fragment vector and set peer download counts back to 0
            m_pieceBeingDownloaded.clear();
            m_numPeersDownloading = 0;
            m_numPeersFinishedFragment = 0;

            // Populate fragment vector for the new piece
            uint32_t numFragsForPiece = getNumFragments(m_currentPiece);
            uint32_t currentFragmentLength = std::min(DefaultFragmentLength, m_pieceLength);
            uint32_t currentFragmentOffset = 0;
            for (uint32_t i = 0; i < numFragsForPiece; ++i)
            {
                TorrentFragment fragment(m_currentPiece, currentFragmentOffset, currentFragmentLength);
                m_pieceBeingDownloaded.push_back(fragment);

                currentFragmentOffset += currentFragmentLength;
                currentFragmentLength = std::min(DefaultFragmentLength, m_pieceLength - currentFragmentOffset);
            }

            return;
        }
    }
    //m_currentPiece = numPieces + 1;
}

void PieceMgr::onAllFragmentsDownloaded()
{
    size_t pieceLength = m_pieceLength;

    // Check if current piece is the last piece or not
    if (m_currentPiece + 1 == m_pieceInfo.size())
    {
        int64_t finalPieceLen = m_pieceInfo.size() * m_pieceLength - m_fileSize;
        if (finalPieceLen < 0)
            finalPieceLen *= -1;
        pieceLength = finalPieceLen;
    }

    // Combine data from each fragment into one array, get the digest, compare to expected value
    uint8_t *pieceData = new uint8_t[pieceLength];

    for (auto fragment : m_pieceBeingDownloaded)
        memcpy(&pieceData[fragment.Offset], fragment.Data, fragment.Length);

    // Calculate digest value
    SHA1Hash hash;
    hash.update(pieceData, pieceLength);
    hash.finalize();

    // Attempt to fetch the torrent file's digest for the current piece
    const std::string &digestStr = m_digestString->getValue();
    if (digestStr.size() < SHA_DIGEST_LENGTH * (m_currentPiece + 1))
    {
        LOG_ERROR("torrent_protocol.PieceMgr", "Digest string from torrent file is too small!");
        delete[] pieceData;
        return;
    }

    uint8_t *digestPtr = (uint8_t*)digestStr.c_str();
    digestPtr += (SHA_DIGEST_LENGTH * m_currentPiece);

    // If hashes match, store the piece onto the disk, set m_pieceInfo[m_currentPiece] to 1
    if (memcmp(hash.getDigest(), digestPtr, SHA_DIGEST_LENGTH) == 0)
    {
        //TODO: Write to disk
        m_pieceInfo[m_currentPiece] = 1;
    }
    else
    {
        // Attempt to re-download the fragment
        m_numPeersFinishedFragment = 0;
    }

    // Free memory that was allocated ot pieceData
    delete[] pieceData;
}
