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
    m_numPeers(0),
    m_numPeersCanUnchoke(5),
    m_downloadComplete(false),
    m_pieceMgr(m_file),
    m_torrentFileName()
{
    // Parse torrent file path string for "_.torrent" and set torrent file name to that substring
    auto pathPos = torrentFilePath.find_last_of('/');
    if (pathPos == std::string::npos)
        pathPos = torrentFilePath.find_last_of('\\');
    m_torrentFileName = (pathPos == std::string::npos) ? torrentFilePath : torrentFilePath.substr(pathPos + 1);
}

const std::string &TorrentState::getTorrentFileName()
{
    return m_torrentFileName;
}

std::shared_ptr<TorrentFile> &TorrentState::getTorrentFile()
{
    return m_file;
}

uint32_t TorrentState::getNumPeers()
{
    return m_numPeers.load();
}

void TorrentState::incrementPeerCount()
{
    ++m_numPeers;
}

void TorrentState::decrementPeerCount()
{
    if (m_numPeers > 0)
        --m_numPeers;
}

bool TorrentState::canUnchokePeer()
{
    if (m_numPeersCanUnchoke.load() > 0)
    {
        --m_numPeersCanUnchoke;
        return true;
    }
    return false;
}

void TorrentState::onPeerChoked()
{
    ++m_numPeersCanUnchoke;
}
