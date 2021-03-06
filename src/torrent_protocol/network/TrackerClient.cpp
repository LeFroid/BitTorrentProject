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

#include <cstdint>
#include "TrackerClient.h"

#include "Decoder.h"
#include "Response.h"
#include "Request.h"
#include "TorrentFile.h"
#include "TorrentState.h"
#include "URL.h"

#include "LogHelper.h"

using namespace bencoding;

namespace network
{
    TrackerClient::TrackerClient(boost::asio::io_service &ioService, Socket::Mode mode) :
        Socket(ioService, mode),
        m_peerID(nullptr),
        m_connectionMgr(),
        m_torrentState()
    {
    }

    void TrackerClient::setPeerID(const char *peerID)
    {
        m_peerID = peerID;
    }

    void TrackerClient::setConnectionMgr(std::shared_ptr< ConnectionMgr<Peer> > connMgr)
    {
        m_connectionMgr = connMgr;
    }

    void TrackerClient::setTorrentState(std::shared_ptr<TorrentState> state)
    {
        m_torrentState = state;
    }

    boost::asio::ip::tcp::endpoint TrackerClient::findTrackerEndpointTCP()
    {
        http::URL announce = m_torrentState->getTorrentFile()->getAnnounceURL();
        std::string host;
        std::string port;
        auto delimPos = announce.getHost().find_last_of(':');
        if (delimPos != std::string::npos)
        {
            host = announce.getHost().substr(0, delimPos);
            port = announce.getHost().substr(delimPos + 1);
        }
        else
        {
            host = announce.getHost();
            port = "80";
        }

        boost::asio::ip::tcp::resolver resolver(m_socket.get_io_service());
        boost::asio::ip::tcp::resolver::query query(host, port);
        return (*resolver.resolve(query));
    }

    void TrackerClient::parsePeerString(const std::string &peerStr)
    {
        // The string consists of multiples of 6 bytes. First 4 bytes are the IP address and last 2 bytes are the port number. All in network (big endian) notation.
        std::string::size_type strIdx = 0;
        const char *data = peerStr.c_str();

        uint32_t tmpIP;
        uint16_t tmpPort;
        while (strIdx + 6 <= peerStr.size())
        {
            tmpIP = 0;
            tmpPort = 0;

            memcpy(&tmpIP, data, 4);
            boost::endian::big_to_native_inplace(tmpIP);
            data += 4;

            memcpy(&tmpPort, data, 2);
            boost::endian::big_to_native_inplace(tmpPort);
            data += 2;

            // Send info to Connection Manager
            if (m_connectionMgr.get())
                m_connectionMgr->attemptConnection(tmpIP, tmpPort, m_torrentState);

            strIdx += 6;
        }
    }

    void TrackerClient::parsePeerList(BenList *peerList)
    {
        /*
         * The given parameter is a list of dictionaries, each with the following keys:
         *   peer id: peer's self-selected ID, as described above for the tracker request (string)
         *   ip: peer's IP address either IPv6 (hexed) or IPv4 (dotted quad) or DNS name (string)
         *   port: peer's port number (integer)
         */
    }

    void TrackerClient::onConnect()
    {
        //todo: UDP support for trackers
        if (getMode() != Socket::Mode::TCP)
            return;

        // Send GET request if TorrentState and TorrentFile are both valid
        if (!m_torrentState.get())
            return;

        auto &torrentFile = m_torrentState->getTorrentFile();
        if (!torrentFile.get())
            return;

        auto bytesDownloaded = m_torrentState->getNumPiecesHave() * torrentFile->getPieceLength();
        http::URL announceURL = torrentFile->getAnnounceURL();
        announceURL.setParameter("info_hash", torrentFile->getInfoHash(), 20);
        announceURL.setParameter("peer_id", m_peerID, 20);
        announceURL.setParameter("port", 6881);
        announceURL.setParameter("uploaded", m_torrentState->getNumBytesUploaded());
        announceURL.setParameter("downloaded", bytesDownloaded);
        announceURL.setParameter("left", torrentFile->getFileSize() - bytesDownloaded);
        announceURL.setParameter("compact", 1);
        announceURL.setParameter("event", "started");
        announceURL.setParameter("key", "magic");

        std::string requestText = http::Request::getText(announceURL);
        LOG_DEBUG("torrent_protocol.network", "Sending request as follows:\n", requestText);
        MutableBuffer mb;
        mb << requestText;
        send(std::move(mb));
        read();
    }

    void TrackerClient::onRead()
    {
        //todo: UDP support for trackers
        if (getMode() != Socket::Mode::TCP)
            return;

        // Get HTTP response
        std::string responseStr;
        responseStr.reserve(m_bufferRead.getSizeUnread());
        for (auto i = 0; i < responseStr.capacity(); ++i)
        {
            responseStr.push_back(*(m_bufferRead.getReadPointer()));
            m_bufferRead.advanceReadPosition(1);
        }

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

            // Check for failure or warning message
            auto keyItr = dict->find("failure reason");
            if (keyItr != dict->end())
            {
                LOG_WARNING("torrent_protocol.network", "Failure to get torrent information from tracker. Reason given is: ",
                            bencast<BenString*>(keyItr->second)->getValue());
                close();
                return;
            }
            keyItr = dict->find("warning message");
            if (keyItr != dict->end())
                LOG_WARNING("torrent_protocol.network", "Warning reported by tracker: ", bencast<BenString*>(keyItr->second)->getValue());

            // Check for interval and min interval to send requests to tracker

            /*
             * interval: Interval in seconds that the client should wait between sending regular requests to the tracker
             * min interval: (optional) Minimum announce interval. If present clients must not reannounce more frequently than this.
             * tracker id: A string that the client should send back on its next announcements. If absent and a previous announce sent a tracker id, do not discard the old value; keep using it.
             * complete: number of peers with the entire file, i.e. seeders (integer)
             * incomplete: number of non-seeder peers, aka "leechers" (integer)
             */
            keyItr = dict->find("peers");
            if (keyItr != dict->end())
            {
                // Get peer info into readable format - we requested compact mode so it should be a string. Otherwise should be a list
                if (BenString *peerString = bencast<BenString*>(keyItr->second))
                    parsePeerString(peerString->getValue());
                else if (BenList *peerList = bencast<BenList*>(keyItr->second))
                    parsePeerList(peerList);
                else
                    LOG_WARNING("torrent_protocol.network", "Unable to determine data type of peers response.");
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

