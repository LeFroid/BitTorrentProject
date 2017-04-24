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

#include "TorrentTable.h"

#include "ObjectManager.h"
#include "Renderer.h"
#include "TorrentCell.h"
#include "VBoxLayout.h"

#include "TorrentState.h"

namespace gui
{
    Uint32 tableUpdateTimerCallback(Uint32 interval, void *param)
    {
        TorrentTable *obj = (TorrentTable*)param;
        if (obj)
            obj->timerCallbackUpdate();
        return interval;
    }

    void updateTable(void *param)
    {
        TorrentTable *obj = (TorrentTable*)param;
        if (obj)
            obj->updateCells();
    }

    TorrentTable::TorrentTable(GUIObject *parent, Position position, Size size) :
        GUIObject(parent, position, size),
        m_vboxCells(nullptr),
        m_cells()
    {
        m_vboxCells = ObjectManager::getInstance()->createObject<VBoxLayout>(this, m_position, m_size);

        // start timer on 500ms interval
        m_updateTimerID = SDL_AddTimer(500, tableUpdateTimerCallback, this);
    }

    TorrentTable::~TorrentTable()
    {
        SDL_RemoveTimer(m_updateTimerID);
    }

    void TorrentTable::addTorrent(std::shared_ptr<TorrentState> torrent)
    {
        TorrentCell *cell = ObjectManager::getInstance()->createObject<TorrentCell>(this, getPosition(), Size(m_size.width, m_size.height / 6),
            Color(255, 255, 255, 255));
        cell->setHidden(false);
        m_cells.push_back(cell->getObjectID());

        m_vboxCells->addItem(cell);
        cell->initialize();
        cell->assignTorrentState(torrent);
        draw();
    }

    void TorrentTable::draw()
    {
        m_vboxCells->draw();
        setHidden(false);
    }

    void TorrentTable::updateCells()
    {
        ObjectManager *objMgr = ObjectManager::getInstance();
        for (uint32_t cellID : m_cells)
        {
            TorrentCell *cell = static_cast<TorrentCell*>(objMgr->getObject(cellID));
            if (cell)
            {
                cell->setHidden(false);
                cell->setChildrenHidden(false);
                cell->update();
                draw();
            }
        }

        getRenderer()->update();
    }

    void TorrentTable::timerCallbackUpdate()
    {
        // Push update table event onto event handler
        SDL_Event event;
        SDL_UserEvent userevent;

        userevent.type = SDL_USEREVENT;
        userevent.code = 0;
        userevent.data1 = (void*)&updateTable;
        userevent.data2 = this;

        event.type = SDL_USEREVENT;
        event.user = userevent;

        SDL_PushEvent(&event);
    }
}
