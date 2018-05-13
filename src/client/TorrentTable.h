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

#include "GUIObject.h"

class TorrentState;

namespace gui
{
    class VBoxLayout;

    /**
     * @class TorrentTable
     * @brief Stores \ref TorrentCells, each (potentially) displaying the progress of
     *        a torrent file download/upload
     */
    class TorrentTable : public GUIObject
    {
    public:
        /// Constructor - requires parent object, a positon and size
        TorrentTable(GUIObject *parent, Position position, Size size);

        /// Torrent table destructor
        virtual ~TorrentTable();

        /// Adds a new cell to the table, which will report the stats regarding the
        /// given TorrentState pointer
        void addTorrent(std::shared_ptr<TorrentState> torrent);

        /// Updates any cells in the table
        void updateCells();

        /// Called by the table update timer; callback will push a user event to call updateCells() method
        void timerCallbackUpdate();

    public:
        /// Draws the torrent table and its cells onto the parent
        void draw() override;

    private:
        /// Vertical layout manager
        VBoxLayout *m_vboxCells;

        /// Stores the object ids of each cell in the table
        std::vector<GUIObjectID> m_cells;

        /// Timer ID for table update event
        SDL_TimerID m_updateTimerID;
    };
}
