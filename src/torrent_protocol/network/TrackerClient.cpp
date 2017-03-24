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

#include "Decoder.h"
#include "Response.h"
#include "Request.h"
#include "TorrentFile.h"

#include "LogHelper.h"

using namespace bencoding;

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
        char peerID[21] = "-TR1330-";
        for (int i = 0; i < 12; ++i)
            peerID[8 + i] = rand() % 256;

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
        LOG_DEBUG("torrent_protocol.test", "Sending request as follows:\n", requestText);
        MutableBuffer mb;
        mb << requestText;
        send(std::move(mb));
        read();
    }

    void TrackerClient::onRead()
    {
        // Get HTTP response
        std::string responseStr;
        responseStr.reserve(m_bufferRead.getSizeUnread());
        m_bufferRead >> responseStr;

        http::Response response(responseStr);

        // If response code is good, extract dictionary from payload
        if (response.StatusCode == 200)
        {
            Decoder decoder;

            auto benResponse = decoder.decode(response.Payload);
            BenDictionary *dict = bencast<BenDictionary*>(benResponse);
            if (!dict)
            {
                LOG_ERROR("torrent_protocol.network", "Unable to decode response from Tracker!");
                close();
                return;
            }

            auto keyItr = dict->find("failure reason");
            if (keyItr != dict->end())
            {
                LOG_WARNING("torrent_protocol.network", "Failure to get torrent information from tracker. Reason given is: ",
                            bencast<BenString*>(keyItr->second)->getValue());
                close();
                return;
            }
        }

        LOG_DEBUG("torrent_protocol.test", "Response.Version = ", response.Version);
        LOG_DEBUG("torrent_protocol.test", "Response.StatusCode = ", response.StatusCode);
        LOG_DEBUG("torrent_protocol.test", "Response.Reason = ", response.Reason);
        LOG_DEBUG("torrent_protocol.test", "Response.Payload = ", response.Payload);

        // close connection
        close();
    }
}

