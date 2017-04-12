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

#include <boost/dynamic_bitset.hpp>
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
        m_recvdHandshake(false),
        m_sentHandshake(false),
        m_piecesHave(),
        m_torrentState()
    {
    }

    Peer::Peer(boost::asio::ip::tcp::socket &&socket) :
        Socket(std::move(socket)),
        m_chokedBy(true),
        m_amChoking(true),
        m_peerInterested(false),
        m_amInterested(false),
        m_recvdHandshake(false),
        m_sentHandshake(false),
        m_piecesHave(),
        m_torrentState()
    {
    }

    void Peer::setTorrentState(std::shared_ptr<TorrentState> state)
    {
        m_torrentState = state;
        m_piecesHave.resize(m_torrentState->getTorrentFile()->getNumPieces());
    }

    void Peer::onConnect()
    {
        if (!m_torrentState.get())
            return;

        // Send handshake and read from peer
        sendHandshake();
        read();
    }

    void Peer::onRead()
    {
        // Update time of last message received
        time(&m_timeLastMessage);

        // Handle handshake before any other messages
        if (!m_recvdHandshake)
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

        // Handle message (TODO: messages 6 through 9)
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
                uint32_t pieceIdx;
                m_bufferRead >> pieceIdx;
                m_torrentState->markPieceAvailable(pieceIdx);
                break;
            // Bitfield: <len=0001+X><id=5><bitfield>
            case 5:
                LOG_DEBUG("torrent_protocol.network", "Bitfield received by peer, length ", length - 1);
                if (m_bufferRead.getSizeUnread() < length - 1) // Make sure all of the data was sent first
                    readBitfield(length - 1);
                else
                    m_bufferRead.reverseReadPosition(5);       // Reverse position back to length prefix if waiting on data
                break;
            // Request: <len=0013><id=6><index><begin><length>
            case 6:
                LOG_DEBUG("torrent_protocol.network", "Request received by peer");
                if (m_bufferRead.getSizeUnread() < length - 1)
                    readRequest();
                else
                    m_bufferRead.reverseReadPosition(5);
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

        // Drop connection if we do not have the torrent file being requested
        if (!eTorrentMgr.hasTorrentFile(infoHash))
        {
            close();
            return;
        }
        else if (!m_torrentState.get())
        {
            // If not already set, get the TorrentState pointer
            m_torrentState = eTorrentMgr.getTorrentState(infoHash);
        }

        // Advance past info hash, read peer ID (sent to TorrentState to associate peer id's with the pieces they have)
        m_bufferRead.advanceReadPosition(20);
        memcpy(m_peerID, m_bufferRead.getReadPointer(), 20);
        m_bufferRead.advanceReadPosition(20);

        // Set received handshake to true, send our handshake if it has not yet been sent
        m_recvdHandshake = true;
        if (!m_sentHandshake)
            sendHandshake();

        // Continue to read data from peer
        read();
    }

    void Peer::readBitfield(uint32_t length)
    {
        // Bounds check already performed on raw buffer
        char *rawBuffer = m_bufferRead.getReadPointer();

        // Populate the peer's bitset with data that was just received
        auto fieldSize = m_piecesHave.size();
        for (uint32_t i = 0; i < 8 * length; i += 8)
        {
            m_piecesHave[i]     = (*rawBuffer & char(0x80));
            m_piecesHave[i + 1] = (*rawBuffer & char(0x40));
            m_piecesHave[i + 2] = (*rawBuffer & char(0x20));
            m_piecesHave[i + 3] = (*rawBuffer & char(0x10));
            m_piecesHave[i + 4] = (*rawBuffer & char(0x08));
            m_piecesHave[i + 5] = (*rawBuffer & char(0x04));
            m_piecesHave[i + 6] = (*rawBuffer & char(0x02));
            m_piecesHave[i + 7] = (*rawBuffer & char(0x01));
            ++rawBuffer;
        }

        // Advance position in read buffer
        m_bufferRead.advanceReadPosition(length);

        // Inform TorrentState of the pieces this peer has
        m_torrentState->readPeerBitset(m_piecesHave);
    }

    void Peer::readRequest()
    {
        uint32_t pieceIdx, offset, length;
        m_bufferRead >> pieceIdx;
        m_bufferRead >> offset;
        m_bufferRead >> length;

        // Make sure we have this piece
        if (m_torrentState->havePiece(pieceIdx))
            sendPiece(pieceIdx, offset, length)
    }

    void Peer::sendHandshake()
    {
        // Handshake is of the format "<pstrlen><pstr><reserved><info_hash><peer_id>"
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

        // Send handshake
        m_sentHandshake = true;
        send(std::move(mb));
    }

    void Peer::sendPiece(uint32_t pieceIdx, uint32_t offset, uint32_t length)
    {
        // At this point it is already confirmed we have the piece
        // Fetch fragment from m_torrentState and send piece message to peer
    }
}

