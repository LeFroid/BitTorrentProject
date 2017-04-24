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
#include <algorithm>
#include "Peer.h"
#include "TorrentMgr.h"
#include "TorrentState.h"
#include "TorrentFile.h"
#include "TorrentFragment.h"

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
        m_torrentState(),
        m_fragmentDownload(nullptr),
        m_fragBytesDownloaded(0)
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
        m_torrentState(),
        m_fragmentDownload(nullptr),
        m_fragBytesDownloaded(0)
    {
    }

    Peer::~Peer()
    {
        if (m_torrentState.get())
        {
            if (!m_amChoking)
                m_torrentState->onPeerChoked();
            m_torrentState->decrementPeerCount();
        }
    }

    void Peer::setTorrentState(std::shared_ptr<TorrentState> state)
    {
        m_torrentState = state;
        m_piecesHave.resize(m_torrentState->getTorrentFile()->getNumPieces());
    }

    void Peer::tryToRequestPiece()
    {
        if (m_chokedBy)
            return;

        // Check if peer has the current piece to be downloaded
        auto currentPiece = m_torrentState->getCurrentPieceNum();
        if (currentPiece < m_piecesHave.size() && m_piecesHave[currentPiece])
        {
            m_fragmentDownload = m_torrentState->getFragmentToDownload();
            if (m_fragmentDownload != nullptr)
            {
                m_fragBytesDownloaded = 0;
                sendRequest(m_fragmentDownload->PieceIdx, m_fragmentDownload->Offset, m_fragmentDownload->Length);
            }
        }
    }

    void Peer::sendPieceHave()
    {
        const auto &pieces = m_torrentState->getBitsetHave();
        if (pieces.any())
        {
            sendHave(pieces.find_first());
        }
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

        // Check if length is equal to amount of data received
        //LOG_DEBUG("torrent_protocol.network", "Length specified = ", length, ", amount received = ", m_bufferRead.getSizeUnread());
        if (m_bufferRead.getSizeUnread() < length)
        {
            // Reverse position by the size of length variable (4 byte), read more data, exit
            m_bufferRead.reverseReadPosition(4);
            read();
            return;
        }

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
                readUnchoked();
                break;
            // Interested [no payload]
            case 2:
                LOG_DEBUG("torrent_protocol.network", "Interested message received by peer");
                m_peerInterested = true;
                if (m_torrentState->canUnchokePeer())
                    sendUnchoke();
                break;
            // Not interested [no payload]
            case 3:
                LOG_DEBUG("torrent_protocol.network", "Not interested message received by peer");
                m_peerInterested = false;
                if (!m_amChoking)
                {
                    sendChoke();
                    m_torrentState->onPeerChoked();
                }
                break;
            // Have: <len=0005><id=4><piece index>
            case 4:
                LOG_DEBUG("torrent_protocol.network", "Have message received by peer");
                uint32_t pieceIdx;
                m_bufferRead >> pieceIdx;
                m_piecesHave[pieceIdx] = true;
                m_torrentState->markPieceAvailable(pieceIdx);
                tryToRequestPiece();
                break;
            // Bitfield: <len=0001+X><id=5><bitfield>
            case 5:
                LOG_DEBUG("torrent_protocol.network", "Bitfield received by peer, length ", length - 1);
                readBitfield(length - 1);
                break;
            // Request: <len=0013><id=6><index><begin><length>
            case 6:
                LOG_DEBUG("torrent_protocol.network", "Request received by peer");
                readRequest();
                break;
            // Piece: <len=0009+X><id=7><index><begin><block>
            case 7:
                LOG_DEBUG("torrent_protocol.network", "Piece (block) message received by peer");
                readPiece(length - 9);
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

        // Set received handshake to true, send our handshake if it has not yet been sent, otherwise send bitfield
        m_recvdHandshake = true;
        if (!m_sentHandshake)
            sendHandshake();
        else
            sendBitfield();

        // Continue to read data from peer
        read();
    }

    void Peer::readUnchoked()
    {
        m_chokedBy = false;
        tryToRequestPiece();
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

        // Check if peer has the current piece to be downloaded
        auto currentPiece = m_torrentState->getCurrentPieceNum();
        if (currentPiece < m_piecesHave.size() && m_piecesHave[currentPiece])
            sendInterested();
    }

    void Peer::readPiece(uint32_t blockSize)
    {
        LOG_DEBUG("torrent_protocol.network", "Reading piece of block size ", blockSize, " from peer");
        uint32_t pieceIdx, offset;
        m_bufferRead >> pieceIdx;
        m_bufferRead >> offset;

        if (!m_fragmentDownload.get())
        {
            m_bufferRead.advanceReadPosition(blockSize);
            return;
        }

        if (pieceIdx != m_fragmentDownload->PieceIdx)
            LOG_WARNING("torrent_protocol.network", "Received fragment of piece ", pieceIdx, ", but requested data for piece ", m_fragmentDownload->PieceIdx);
        if (offset != m_fragmentDownload->Offset)
            LOG_WARNING("torrent_protocol.network", "Received offset of ", offset, " for piece ", pieceIdx, " but requested offset of ", m_fragmentDownload->Offset);
        if (blockSize > m_fragmentDownload->Length)
            LOG_WARNING("torrent_protocol.network", "Received fragment of size ", blockSize, ", however requested length of ", m_fragmentDownload->Length, " instead");

        // Copy data, advance read position and take note of bytes downloaded
        m_fragmentDownload->WriteData(m_bufferRead.getReadPointer(), m_fragBytesDownloaded, blockSize);
        //memcpy(&(m_fragmentDownload->Data[m_fragBytesDownloaded]), m_bufferRead.getReadPointer(), blockSize);
        m_bufferRead.advanceReadPosition(blockSize);
        m_fragBytesDownloaded += blockSize;

        // Inform torrent state if fully downloaded
        if (m_fragBytesDownloaded >= m_fragmentDownload->Length)
        {
            m_torrentState->onFragmentDownloaded(m_fragmentDownload->PieceIdx);
            tryToRequestPiece();
        }
    }

    void Peer::readRequest()
    {
        uint32_t pieceIdx, offset, length;
        m_bufferRead >> pieceIdx;
        m_bufferRead >> offset;
        m_bufferRead >> length;

        // Make sure we have this piece and are not currently choking the peer
        if (m_torrentState->havePiece(pieceIdx) && !m_amChoking)
            sendPiece(pieceIdx, offset, length);
    }

    void Peer::sendHandshake()
    {
        m_torrentState->incrementPeerCount();

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

        // Check if should send bitfield
        if (m_recvdHandshake)
            sendBitfield();
    }

    void Peer::sendBitfield()
    {
        const boost::dynamic_bitset<> &bitset = m_torrentState->getBitsetHave();
        if (!bitset.any())
            return;

        const size_t bytesToSend = (size_t)ceil((float)bitset.size() / 8.0f);
        size_t bitsetPos = 0;

        MutableBuffer mb(4 + 1 + bytesToSend);
        mb << uint32_t(1 + bytesToSend);        // Length
        mb << uint8_t(5);                       // Message ID
        for (size_t i = 0; i < bytesToSend; ++i)
        {
            if (bitsetPos + 7 >= bitset.size())
            {
                mb << uint8_t(0);
                break;
            }
            uint8_t currentByte = ((bitset.test(bitsetPos) ? uint8_t(0x80) : uint8_t(0x00))
                    | (bitset.test(bitsetPos + 1) ? uint8_t(0x40) : uint8_t(0x00))
                    | (bitset.test(bitsetPos + 2) ? uint8_t(0x20) : uint8_t(0x00))
                    | (bitset.test(bitsetPos + 3) ? uint8_t(0x10) : uint8_t(0x00))
                    | (bitset.test(bitsetPos + 4) ? uint8_t(0x08) : uint8_t(0x00))
                    | (bitset.test(bitsetPos + 5) ? uint8_t(0x04) : uint8_t(0x00))
                    | (bitset.test(bitsetPos + 6) ? uint8_t(0x02) : uint8_t(0x00))
                    | (bitset.test(bitsetPos + 7) ? uint8_t(0x01) : uint8_t(0x00)));
            mb << currentByte;

            bitsetPos += 8;
        }

        send(std::move(mb));
    }

    void Peer::sendInterested()
    {
        MutableBuffer mb(4 + 1);
        mb << uint32_t(1);       // Length
        mb << uint8_t(2);        // Message ID
        send(std::move(mb));
    }

    void Peer::sendChoke()
    {
        MutableBuffer mb(4 + 1);
        mb << uint32_t(1);      // Length
        mb << uint8_t(0);       // Message ID
        send(std::move(mb));
    }

    void Peer::sendUnchoke()
    {
        MutableBuffer mb(4 + 1);
        mb << uint32_t(1);       // Length
        mb << uint8_t(1);        // Message ID
        send(std::move(mb));
    }

    void Peer::sendPiece(uint32_t pieceIdx, uint32_t offset, uint32_t length)
    {
        std::shared_ptr<TorrentFragment> fragPtr = m_torrentState->getFragmentToUpload(pieceIdx, offset, length);
        if (!fragPtr.get())
            return;

        MutableBuffer mb(4 + 1 + 4 + 4 + length);
        mb << uint32_t(9 + length); // Length
        mb << uint8_t(7);           // Message ID
        mb << pieceIdx;
        mb << offset;
        mb.write((char*)fragPtr->Data, length);
        send(std::move(mb));
    }

    void Peer::sendRequest(uint32_t pieceIdx, uint32_t offset, uint32_t length)
    {
        LOG_DEBUG("torrent_protocol.network", "Sending request for piece ", pieceIdx, ", offset ", offset, ", length ", length);
        MutableBuffer mb(4 + 1 + 4 + 4 + 4);
        mb << uint32_t(13);     // Length
        mb << uint8_t(6);       // Message ID
        mb << pieceIdx;         // Piece
        mb << offset;           // Fragment Offset
        mb << length;           // Fragment Length
        send(std::move(mb));
    }

    void Peer::sendHave(uint32_t pieceIdx)
    {
        if (!m_sentHandshake || !m_recvdHandshake)
            return;

        if (m_isClosing || pieceIdx >= m_piecesHave.size())
            return;

        MutableBuffer mb(4 + 1 + 4);
        mb << uint32_t(5); // Length
        mb << uint8_t(4);  // Message ID
        mb << pieceIdx;
        send(std::move(mb));
    }
}

