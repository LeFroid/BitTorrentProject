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
#include "Socket.h"

namespace network
{
    /**
     * @class Listener
     * @brief Listens on a given port for incoming connections, accepting them
     *        if their info_hash is found in the \ref TorrentMgr info hash map
     */
    class Listener
    {
    public:
        /// Listener constructor - requires a reference to an io service, port number
        /// and maximum number of connections to allow
        explicit Listener(boost::asio::io_service &ioService);

        /**
         * @brief start Begins accepting incoming connections
         * @param port Port number
         * @param maxConnections The maximum number of pending connections to have queued at a time
         */
        void start(uint16_t port, int maxConnections);

    private:
        /// Accepts the next connection
        void accept();

    private:
        /// Reference to the io service being used
        boost::asio::io_service &m_ioService;

        /// TCP acceptor
        boost::asio::ip::tcp::acceptor m_acceptor;
    };
}
