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

#include "TorrentFile.h"
#include "TorrentState.h"

TorrentState::TorrentState(const std::string &torrentFilePath) :
    m_file(std::make_shared<TorrentFile>(torrentFilePath)),
    m_pieceInfo(m_file->getNumPieces()),
    m_piecesAvailable(m_file->getNumPieces()),
    m_currentPiece(0),
    m_downloadComplete(false)
{
}

std::shared_ptr<TorrentFile> &TorrentState::getTorrentFile()
{
    return m_file;
}

const uint32_t &TorrentState::getCurrentPieceNum()
{
    // First check if current piece has finished downloading
    if ((!m_downloadComplete && m_pieceInfo[m_currentPiece])
            || !m_piecesAvailable[m_currentPiece])
        m_currentPiece = determineNextPiece();

    return m_currentPiece;
}

bool TorrentState::havePiece(uint32_t pieceIdx) const
{
    // Make sure index is valid
    if (pieceIdx >= m_pieceInfo.size())
        return false;

    return m_pieceInfo[pieceIdx];
}

void TorrentState::markPieceAvailable(uint32_t pieceIdx)
{
    // Make sure index is valid
    if (pieceIdx >= m_piecesAvailable.size())
        return;

    m_piecesAvailable[pieceIdx] = true;
}

void TorrentState::readPeerBitset(const boost::dynamic_bitset<> &set)
{
    m_piecesAvailable |= set;
}

uint32_t TorrentState::determineNextPiece()
{
    //TODO: First piece == randomly chosen, after that use rarest first algorithm
    uint32_t retVal = 0;
    auto numPieces = m_pieceInfo.size();

    boost::dynamic_bitset<> piecesToChoose = (m_piecesAvailable & (~m_pieceInfo));
    if (!piecesToChoose.any())
        return numPieces + 1;

    while (true)
    {
        retVal = uint64_t(rand()) % numPieces;
        if (piecesToChoose[retVal])
            return retVal;
    }
    return numPieces + 1;
}
