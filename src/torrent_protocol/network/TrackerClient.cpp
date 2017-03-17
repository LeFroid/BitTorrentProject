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

#include "TrackerClient.h"

#include "Request.h"
#include "TorrentFile.h"

#include "LogHelper.h"

namespace network
{
    TrackerClient::TrackerClient(boost::asio::io_service &ioService, Socket::Mode mode) :
        ClientBase(ioService, mode),
        m_torrentFile()
    {
    }

    void TrackerClient::setTorrentFile(std::shared_ptr<TorrentFile> file)
    {
        m_torrentFile = file;
    }

    void TrackerClient::onConnect()
    {
        // Send GET request if m_torrentFile is valid
        if (!m_torrentFile.get())
            return;

        // dummy peer id for testing
        char peerID[20] = "-AZ2060-";
        for (int i = 0; i < 12; ++i)
            peerID[8 + i] = rand() % 256;
        char infoHash[SHA_DIGEST_LENGTH];
        memcpy(infoHash, m_torrentFile->getInfoHash(), 20);

        http::URL announceURL = m_torrentFile->getAnnounceURL();
        announceURL.setParameter("info_hash", m_torrentFile->getInfoHash(), 20);
        announceURL.setParameter("peer_id", peerID, 20);
        announceURL.setParameter("port", 6881);
        announceURL.setParameter("uploaded", 0);
        announceURL.setParameter("downloaded", 0);
        announceURL.setParameter("left", m_torrentFile->getFileSize());
        announceURL.setParameter("compact", 1);
        announceURL.setParameter("event", "started");

        std::string requestText = http::Request::getText(announceURL);
        LOG_INFO("test", "Sending request as follows:\n", requestText);
        MutableBuffer mb;
        mb << requestText;
        send(std::move(mb));
        read();
    }

    void TrackerClient::onRead()
    {
        // print response
        std::string response;
        response.reserve(m_bufferRead.getSizeUnread());
        m_bufferRead >> response;
        LOG_INFO("torrent.test", "read pos = ", m_bufferRead.getReadPosition());
        LOG_INFO("torrent.test", "tracker response size = ", m_bufferRead.getSizeUnread());
        LOG_INFO("torrent.test", "buffer read size = ", m_bufferRead.getSize());
        LOG_INFO("torrent.test", "data is (raw string): ", response);
        // close connection
        close();
    }
}

