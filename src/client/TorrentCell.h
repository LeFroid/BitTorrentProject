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

#include "Box.h"

class TorrentState;
class TorrentTable;

namespace gui
{
    class Label;
    class ProgressBar;

    /**
     * @class TorrentCell
     * @brief Displays
     */
    class TorrentCell : public Box
    {
        friend class TorrentTable;

    public:
        /// TorrentCell constructor - input is parent (table), position, size and background color
        TorrentCell(GUIObject *parent, Position position, Size size, Color color);

        /// TorrentCell destructor
        ~TorrentCell() {}

        /// Assigns the given torrent state to the cell - information regarding the torrent will be displayed
        /// with the various child objects belonging to this cell
        void assignTorrentState(std::shared_ptr<TorrentState> state);

        /// Called to update the cell with the most recent information regarding the torrent assigned to it.
        /// Does nothing if not assigned to a torrent file
        void update();

    public:
        /// Draws the torrent cell onto its parent
        virtual void draw() override;

    protected:
        /// Initializes child objects belonging to the cell
        void initialize();

    private:
        /// Given a number of bytes, returns a string containing a user-friendly
        /// representation of the size. For example: 1048576 => "1 MB", or 1024 => "1 KB"
        std::string bytesToReadableFmt(const uint64_t &bytes);

    private:
        /// TorrentState associated with the cell
        std::shared_ptr<TorrentState> m_torrentState;

        /// Label displaying the name of the .torrent file
        Label *m_labelTorrentFile;

        /// Progress bar showing how much of the torrent has been downloaded to the client
        ProgressBar *m_progressBar;

        /// Label displaying the number of peers that are engaged in uploading and/or downloading pieces of the torrent
        Label *m_labelNumPeers;

        /// Label displaying the amount of data that has been downloaded onto the client machine [X (Bytes|KB|MB|GB)]
        Label *m_labelDataDownloaded;

        /// Label displaying the amount of data that has been uploaded to peers in the swarm [X (Bytes|KB|MB|GB)]
        Label *m_labelDataUploaded;

        /// Label displaying the ratio of pieces that have been downloaded to the total number of pieces (ex: 100/3200 pieces)
        Label *m_labelRatioPiecesHave;

        /// Label displaying the total size of the file, once downloaded
        Label *m_labelFileSize;
    };
}
