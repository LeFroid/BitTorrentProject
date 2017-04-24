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

#include <iomanip>
#include <sstream>

#include "TorrentCell.h"

#include "Label.h"
#include "ProgressBar.h"
#include "ObjectManager.h"
#include "TorrentFile.h"
#include "TorrentState.h"

namespace gui
{
    TorrentCell::TorrentCell(GUIObject *parent, Position position, Size size, Color color) :
        Box(parent, position, size, color),
        m_torrentState(nullptr),
        m_labelTorrentFile(nullptr),
        m_progressBar(nullptr),
        m_labelNumPeers(nullptr),
        m_labelDataDownloaded(nullptr),
        m_labelDataUploaded(nullptr),
        m_labelRatioPiecesHave(nullptr),
        m_labelFileSize(nullptr)
    {
    }

    void TorrentCell::assignTorrentState(std::shared_ptr<TorrentState> state)
    {
        m_torrentState = state;

        setChildrenHidden(false);

        // Only set file size label once
        m_labelFileSize->setText("Total Size: " + bytesToReadableFmt(m_torrentState->getTorrentFile()->getFileSize()));

        update();
    }

    void TorrentCell::update()
    {
        if (!m_torrentState.get())
            return;

        // Fetch relevant torrent info
        auto &torrentFile = m_torrentState->getTorrentFile();
        m_labelTorrentFile->setText(m_torrentState->getTorrentFileName());

        auto bytesDownloaded = m_torrentState->getNumPiecesHave() * torrentFile->getPieceLength();
        float progress = float(bytesDownloaded) / torrentFile->getFileSize();
        m_progressBar->setProgress(progress);

        m_labelRatioPiecesHave->setText(
            std::to_string(m_torrentState->getNumPiecesHave()) + " of " + std::to_string(torrentFile->getNumPieces()) + " pieces downloaded");

        m_labelNumPeers->setText(std::to_string(m_torrentState->getNumPeers()) + " peer connections");

        m_labelDataUploaded->setText("Upload: " + bytesToReadableFmt(m_torrentState->getNumBytesUploaded()));

        m_labelDataDownloaded->setText("Download: " + bytesToReadableFmt(bytesDownloaded));
    }

    void TorrentCell::draw()
    {
        update();
        Box::draw();
    }

    void TorrentCell::initialize()
    {
        const Position &parentPos = getPosition();
        const Size &parentSize = getSize();

        ObjectManager *objMgr = ObjectManager::getInstance();

        // Torrent file label
        Position tmpPos(parentPos.x + 15, parentPos.y + 5);
        Color textColor(0, 0, 0, 255);
        m_labelTorrentFile = objMgr->createObject<Label>(this, tmpPos, Size(parentSize.width - 10, parentSize.height / 4), textColor,
            std::string(), 24);

        // Progress bar
        tmpPos.x = parentPos.x + parentSize.width / 6;
        tmpPos.y = parentPos.y + parentSize.height / 3;
        Color colorIncomplete(150, 150, 150, 255);
        Color colorProgress(35, 148, 255, 255);
        m_progressBar = objMgr->createObject<ProgressBar>(this, tmpPos, Size((parentSize.width * 5 / 6) - 10, parentSize.height / 4), colorIncomplete, colorProgress);

        // Piece ratio label
        tmpPos.y += 5 + parentSize.height / 4;
        m_labelRatioPiecesHave = objMgr->createObject<Label>(this, tmpPos, Size(parentSize.width / 3, parentSize.height / 10), textColor,
            std::string(), 16);

        // File size label
        tmpPos.x = parentPos.x + parentSize.width * 3 / 4;
        m_labelFileSize = objMgr->createObject<Label>(this, tmpPos, Size(parentSize.width / 4, parentSize.height / 10), textColor,
            std::string(), 16);

        // Peers label
        tmpPos.x = parentPos.x + 15;
        tmpPos.y = parentPos.y + (parentSize.height * 5 / 6) - 5;
        m_labelNumPeers = objMgr->createObject<Label>(this, tmpPos, Size(parentSize.width / 4, (parentSize.height / 6) - 5), textColor,
            std::string(), 20);

        // Upload label
        tmpPos.x = parentPos.x + parentSize.width * 9 / 16;
        m_labelDataUploaded = objMgr->createObject<Label>(this, tmpPos, Size(parentSize.width / 5, (parentSize.height / 6) - 5), textColor,
            std::string(), 20);

        // Download label
        tmpPos.x = parentPos.x + parentSize.width * 3 / 4;
        m_labelDataDownloaded = objMgr->createObject<Label>(this, tmpPos, Size(parentSize.width / 5, (parentSize.height / 6) - 5), textColor,
            std::string(), 20);

        setChildrenHidden(true);
    }

    std::string TorrentCell::bytesToReadableFmt(const uint64_t &bytes)
    {
        double rounding = bytes;

        std::stringstream ss;
        ss << std::fixed << std::setprecision(2);
        if (bytes >= 1073741824) // >= 1GB
        {
            rounding /= 1073741824;
            ss << rounding << " GB";
        }
        else if (bytes >= 1048576) // >= 1MB
        {
            rounding /= 1048576;
            ss << rounding << " MB";
        }
        else if (bytes >= 1024) // >= 1 KB
        {
            rounding /= 1024;
            ss << rounding << " KB";
        }
        else
            ss << bytes << " bytes";

        return ss.str();
    }
}
