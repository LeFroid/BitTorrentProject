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

#include <boost/filesystem.hpp>

#include "MainScreen.h"

#include "Button.h"
#include "Label.h"
#include "ObjectManager.h"
#include "Renderer.h"
#include "TextInputBox.h"
#include "TorrentTable.h"

#include "TorrentMgr.h"
#include "TorrentState.h"

namespace gui
{
    MainScreen::MainScreen(GUIObject *parent, std::string themeFile) :
        GUIObject(parent),
        m_inputBoxTorrentFile(nullptr),
        m_labelDownloadDir(nullptr),
        m_inputBoxDownloadDir(nullptr),
        m_buttonAddTorrent(nullptr),
        m_buttonChangeDownloadDir(nullptr),
        m_torrentTable(nullptr),
        m_theme(themeFile)
    {
        initialize();
    }

    void MainScreen::draw()
    {
        hide();

        Renderer *render = getRenderer();
        render->setColor(m_theme.getBackgroundColor());
        render->clearContent();

        // Draw children
        m_inputBoxTorrentFile->draw();
        m_buttonAddTorrent->draw();
        m_labelDownloadDir->draw();
        m_inputBoxDownloadDir->draw();
        m_buttonChangeDownloadDir->draw();
        m_torrentTable->draw();

        render->update();
        setHidden(false);
    }

    void MainScreen::initialize()
    {
        if (!getParent())
            return;

        const Position &objPos = getParent()->getPosition();
        const Size &objSize = getParent()->getSize();

        setPosition(objPos);
        setSize(objSize);

        ObjectManager *objMgr = ObjectManager::getInstance();

        // Create torrent input box & enter button
        m_inputBoxTorrentFile = objMgr->createObject<TextInputBox>(this, Position(objPos.x + 5, objPos.y + 10),
            Size(objSize.width / 2, objSize.height / 14), m_theme.getTextBoxColor(), "Torrent file path..");
        m_inputBoxTorrentFile->setFontPointSize(16);

        m_buttonAddTorrent = objMgr->createObject<Button>(this, Position(objPos.x + objSize.width * 9 / 16, objPos.y + 10),
            Size(objSize.width / 8, objSize.height / 14), m_theme.getButtonColor());
        m_buttonAddTorrent->setMouseHoverColor(m_theme.getButtonHoverColor());
        m_buttonAddTorrent->setFontPointSize(18);
        m_buttonAddTorrent->setText("Download");
        m_buttonAddTorrent->registerMouseEvent(MouseEvent::ReleasedLeft, std::bind(&MainScreen::downloadTorrent, this));
        m_buttonAddTorrent->setHint("Begin to download the torrent file.");

        // Create download directory widgets
        m_labelDownloadDir = objMgr->createObject<Label>(this, Position(objPos.x + 5, objPos.y + objSize.height / 10),
            Size(objSize.width / 4, objSize.height / 19), Color(255, 255, 255, 255), "Download Directory:");
        m_labelDownloadDir->setFontPointSize(12);

        m_inputBoxDownloadDir = objMgr->createObject<TextInputBox>(this, Position(objPos.x + 5, objPos.y + objSize.height / 8),
            Size(objSize.width / 4 - 5, objSize.height / 20), m_theme.getTextBoxColor());
        m_inputBoxDownloadDir->setFontPointSize(10);

        boost::filesystem::path downloadDir(eTorrentMgr.getDownloadDirectory());
        boost::filesystem::path downloadDirAbs = boost::filesystem::absolute(downloadDir);
        m_inputBoxDownloadDir->setText(downloadDirAbs.string());

        m_buttonChangeDownloadDir = objMgr->createObject<Button>(this, Position(objPos.x + 5, objPos.y + objSize.height / 5),
            Size(objSize.width / 8, objSize.height / 20), m_theme.getButtonColor());
        m_buttonChangeDownloadDir->setMouseHoverColor(m_theme.getButtonHoverColor());
        m_buttonChangeDownloadDir->setFontPointSize(18);
        m_buttonChangeDownloadDir->setText("Save");
        m_buttonChangeDownloadDir->registerMouseEvent(MouseEvent::ReleasedLeft, std::bind(&MainScreen::onChangeDownloadDir, this));
        m_buttonChangeDownloadDir->setHint("Click to confirm a change to the download directory for torrents.");

        // Create table
        m_torrentTable = objMgr->createObject<TorrentTable>(this, Position(objPos.x + objSize.width / 4, objPos.y + objSize.height / 8),
            Size((objSize.width * 3 / 4) - 20, (objSize.height * 7 / 8) - 10));

        setChildrenHidden(false);
    }

    void MainScreen::downloadTorrent()
    {
        const std::string &filePath = m_inputBoxTorrentFile->getText();
        if (filePath.empty())
            return;
        // Make sure the path is valid
        boost::filesystem::path pathCheck(filePath);
        boost::system::error_code ec;
        if (boost::filesystem::exists(pathCheck, ec))
        {
            std::shared_ptr<TorrentState> torrent = eTorrentMgr.addTorrent(filePath);
            if (torrent.get())
                m_torrentTable->addTorrent(torrent);
        }
        else
        {
            m_inputBoxTorrentFile->setText("Error: File not found");
            draw();
        }
    }

    void MainScreen::onChangeDownloadDir()
    {
        boost::filesystem::path downloadDir = m_inputBoxDownloadDir->getText();
        boost::system::error_code ec;
        if (boost::filesystem::is_directory(downloadDir, ec))
        {
            std::string downloadStr = downloadDir.string();
            eTorrentMgr.setDownloadDirectory(downloadStr);
        }
    }
}
