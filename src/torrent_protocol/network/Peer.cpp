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
#include "Peer.h"
#include "TorrentMgr.h"
#include "TorrentState.h"
#include "TorrentFile.h"

namespace network
{
    Peer::Peer(boost::asio::io_service &ioService, Mode mode) :
        Socket(ioService, mode),
        m_chokedBy(true),
        m_amChoking(true),
        m_peerInterested(false),
        m_amInterested(false),
        m_doneHandshake(false),
        m_torrentState()
    {
    }

    Peer::Peer(boost::asio::ip::tcp::socket &&socket) :
        Socket(std::move(socket)),
        m_chokedBy(true),
        m_amChoking(true),
        m_peerInterested(false),
        m_amInterested(false),
        m_doneHandshake(false),
        m_torrentState()
    {
    }

    void Peer::setTorrentState(std::shared_ptr<TorrentState> state)
    {
        m_torrentState = state;
    }

    void Peer::onConnect()
    {
        if (!m_torrentState.get())
            return;

        // Send handshake, following the format "<pstrlen><pstr><reserved><info_hash><peer_id>"
        auto endpoint = getTCPEndpoint();
        LOG_DEBUG("torrent_protocol.network", "Sending handshake to peer with endpoint ", endpoint.address().to_string(), ':', endpoint.port());

        uint8_t pstrlen = 19;
        char pstr[20] = "BitTorrent protocol";
        uint64_t reserved = 0;

        MutableBuffer mb(1 + pstrlen + 8 + 20 + 20);
        mb << pstrlen;
        mb.write(pstr, pstrlen);
        mb << reserved;
        mb.write((const char*)m_torrentState->getTorrentFile()->getInfoHash(), 20);
        mb.write(eTorrentMgr.getPeerID(), 20);

        // Send handshake, read from peer
        send(std::move(mb));
        read();
    }

    void Peer::onRead()
    {
        // Update time of last message received
        time(&m_timeLastMessage);

        // Handle handshake before any other message
        if (!m_doneHandshake.load())
        {
            readHandshake();
            return;
        }

        // Ensure there was enough data transmitted
        if (m_bufferRead.getSizeUnread() < 4)
            return;

        // Data format: "<length prefix (32-bit)><message ID (8-bit)><payload>."
        uint32_t length;
        m_bufferRead >> length;

        // Before reading message ID, check if keep-alive was sent (len == 0)
        if (length == 0)
            return;

        uint8_t messageID;
        m_bufferRead >> messageID;

        // Handle message
        switch (messageID)
        {
            // Choke [no payload]
            case 0:
                LOG_DEBUG("torrent_protocol.network", "Choke message received by peer");
                m_chokedBy = true;
                break;
            // Unchoke [no payload]
            case 1:
                LOG_DEBUG("torrent_protocol.network", "Unchoke message received by peer");
                m_chokedBy = false;
                break;
            // Interested [no payload]
            case 2:
                LOG_DEBUG("torrent_protocol.network", "Interested message received by peer");
                m_peerInterested = true;
                break;
            // Not interested [no payload]
            case 3:
                LOG_DEBUG("torrent_protocol.network", "Not interested message received by peer");
                m_peerInterested = false;
                break;
            // Have: <len=0005><id=4><piece index>
            case 4:
                LOG_DEBUG("torrent_protocol.network", "Have message received by peer");
                break;
            // Bitfield: <len=0001+X><id=5><bitfield>
            case 5:
                LOG_DEBUG("torrent_protocol.network", "Bitfield received by peer");
                break;
            // Request: <len=0013><id=6><index><begin><length>
            case 6:
                LOG_DEBUG("torrent_protocol.network", "Request received by peer");
                break;
            // Piece: <len=0009+X><id=7><index><begin><block>
            case 7:
                LOG_DEBUG("torrent_protocol.network", "Piece (block) message received by peer");
                break;
            // Cancel: <len=0013><id=8><index><begin><length>
            case 8:
                LOG_DEBUG("torrent_protocol.network", "Cancel message received by peer");
                break;
            // Port: <len=0003><id=9><listen-port>
            case 9:
                LOG_DEBUG("torrent_protocol.network", "Port message received by peer. Ignoring.");
                m_bufferRead.advanceReadPosition(length - 1);
                break;
            default:
                LOG_DEBUG("torrent_protocol.network", "Unknown message of id ", (uint32_t)messageID, " received by peer. Ignoring.");
                break;
        }

        read();
    }

    void Peer::readHandshake()
    {
        // Read handshake, check info hash.
        // Drop connection if info hash not in TorrentMgr
        if (m_bufferRead.getSizeUnread() < 1)
            return;

        uint8_t pstrlen;
        m_bufferRead >> pstrlen;
        if (m_bufferRead.getSizeUnread() < 48 + pstrlen)
            return;
        m_bufferRead.advanceReadPosition(pstrlen + 8);

        uint8_t infoHash[20];
        memcpy(infoHash, m_bufferRead.getReadPointer(), 20);

        if (!eTorrentMgr.hasTorrentFile(infoHash))
        {
            close();
            return;
        }

        // Ignore peer_id for the time being
        m_bufferRead.advanceReadPosition(20 + 20);
        m_doneHandshake.store(true);
        read();
    }
}
