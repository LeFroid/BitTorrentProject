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

#include "Theme.h"

namespace gui
{
    class Button;
    class Label;
    class TextInputBox;
    class TorrentTable;

    /**
     * @class MainScreen
     * @brief Handles the layout of all objects on the main window, their refresh rate, etc.
     */
    class MainScreen : public GUIObject
    {
    public:
        /// Constructs the main screen with a pointer to the window, and the path to the gui theme file
        MainScreen(GUIObject *parent, std::string themeFile);

        /// MainScreen destructor
        ~MainScreen() {}

    public:
        /// Draws the screen onto the window
        void draw() override;

    private:
        /// Initializes the objects belonging to the screen
        void initialize();

        /// Attempts to begin the download process of a torrent file
        void downloadTorrent();

        /// Called when the button is clicked to signal a change in the download directory
        void onChangeDownloadDir();

    private:
        /// Text input box, used to enter the path of a .torrent file to begin downloading
        TextInputBox *m_inputBoxTorrentFile;

        /// Label explaining the download directory input box
        Label *m_labelDownloadDir;

        /// Text input box, used to show and/or modify the directory in which torrent files are downloaded
        TextInputBox *m_inputBoxDownloadDir;

        /// Button used to begin downloading the torrent file associated with the input in the text box
        Button *m_buttonAddTorrent;

        /// Button used to confirm a change to the download directory
        Button *m_buttonChangeDownloadDir;

        /// Table which stores the cells representing the progress and stats of each torrent file being downloaded
        TorrentTable *m_torrentTable;

        /// GUI theme object
        Theme m_theme;
    };
}
