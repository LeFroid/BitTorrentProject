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
#include "ClientBase.h"

class TorrentFile;

namespace network
{
    /**
     * @class TrackerClient
     * @brief Handles communications with a tracker service
     */
    class TrackerClient : public ClientBase
    {
    public:
        /// TrackerClient constructor
        explicit TrackerClient(boost::asio::io_service &ioService, Socket::Mode mode);

        /// Sets the tracker client's shared_ptr to the Torrent File object
        /// which will be used to request peer information from
        void setTorrentFile(std::shared_ptr<TorrentFile> file);

    protected:
        /// Called after forming initial connection with a tracker
        virtual void onConnect() override;

        /// Handles post-recv operations
        virtual void onRead() override;

    private:
        /// Shared pointer to the torrent file being used
        std::shared_ptr<TorrentFile> m_torrentFile;
    };
}
