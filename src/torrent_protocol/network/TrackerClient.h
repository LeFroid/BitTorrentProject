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

#include <memory>
#include "ConnectionMgr.h"
#include "Peer.h"
#include "Socket.h"

namespace bencoding { class BenList; }

class TorrentState;

namespace network
{
    /**
     * @class TrackerClient
     * @brief Handles communications with a tracker service
     */
    class TrackerClient : public Socket
    {
    public:
        /// TrackerClient constructor
        explicit TrackerClient(boost::asio::io_service &ioService, Socket::Mode mode);

        /// Sets the Peer ID that will be transmitted to the tracker service
        void setPeerID(const char *peerID);

        /// Sets the shared pointer to the peer connection manager - used to connect to potential peers
        void setConnectionMgr(std::shared_ptr< ConnectionMgr<Peer> > connMgr);

        /// Sets the tracker client's shared_ptr to the Torrent State object
        /// which will be used to request peer information
        void setTorrentState(std::shared_ptr<TorrentState> state);

        /// Attempts to determine the endpoint of the tracker service, based on the Torrent File's announce URL
        boost::asio::ip::tcp::endpoint findTrackerEndpointTCP();

        /// Dummy method
        void sendPieceHave() { }

    private:
        /// Gets peer information from the given string, sending it in a more useful format to the TorrentState
        /// object
        void parsePeerString(const std::string &peerStr);

        /// Gets peer information from the given bencoded list of dictionaries, sending it to the TorrentState
        /// object.
        void parsePeerList(bencoding::BenList *peerList);

    protected:
        /// Called after forming initial connection with a tracker
        virtual void onConnect() override;

        /// Handles post-recv operations
        virtual void onRead() override;

    private:
        /// Peer ID
        const char *m_peerID;

        /// Shared pointer to the connection manager
        std::shared_ptr< ConnectionMgr<Peer> > m_connectionMgr;

        /// Shared pointer to the torrent state
        std::shared_ptr<TorrentState> m_torrentState;
    };
}
