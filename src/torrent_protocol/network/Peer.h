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
#include <ctime>
#include "Socket.h"

class TorrentState;

namespace network
{
    /**
     * @class Peer
     * @brief Represents a single remote entity in the swarm, connected
     *        to the local client through a TCP socket
     */
    class Peer : public Socket
    {
    public:
        /// Constructs a new peer object, given a reference to an io_service
        explicit Peer(boost::asio::io_service &ioService, Socket::Mode mode);

        /// Constructs a peer by moving the given tcp socket
        Peer(boost::asio::ip::tcp::socket &&socket);

        /// Peer destructor
        ~Peer();

        /// Sets the shared pointer to the torrent state for the p2p connection
        void setTorrentState(std::shared_ptr<TorrentState> state);

        //void requestPiece(uint64_t pieceNum);

    protected:
        /// Called after the local client has successfully initiated a connection with a remote peer
        virtual void onConnect() override;

        /// Called after a successful read operation
        virtual void onRead() override;

    /// Functions to handle incoming data
    private:
        /// Handles the handshake message sent by the peer
        void readHandshake();

        /// Handles the unchoke message sent by the peer
        void readUnchoked();

        /// Handles the bitfield message and contents sent by the peer
        void readBitfield(uint32_t length);

        /// Handles the request message when received from the peer
        void readRequest();

    /// Functions to handle sending of data
    private:
        /// Sends the client's handshake to the peer
        void sendHandshake();

        /// Sends the interested message to the peer
        void sendInterested();

        /// Sends a fragment of the piece at the given index, with an offset of the piece and
        /// specified length to the peer 
        void sendPiece(uint32_t pieceIdx, uint32_t offset, uint32_t length);

        /// Sends a request to the peer for a fragment of the given piece with the specified
        /// fragment offset and length
        void sendRequest(uint32_t pieceIdx, uint32_t offset, uint32_t length);

    private:
        /// The peer's identifier
        char m_peerID[20];

        /// The timestamp of when the last message was received
        time_t m_timeLastMessage;

        /// True if the peer is choking the client, false if else
        bool m_chokedBy;

        /// True if the client is choking the peer, false if else
        bool m_amChoking;

        /// True if the peer is interested in the client, false if else
        bool m_peerInterested;

        /// True if the client is interested in the peer, false if else
        bool m_amInterested;

        /// Used to determine if handshake has been received from the remote peer
        bool m_recvdHandshake;

        /// Used to determine if handshake has been sent by the client
        bool m_sentHandshake;

        /// Bitset representing the pieces of the torrent file this peer has
        boost::dynamic_bitset<> m_piecesHave;

        /// Torrent state pointer
        std::shared_ptr<TorrentState> m_torrentState;
    };
}
