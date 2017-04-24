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

#include "Configuration.h"
#include "GUIProperties.h"
#include <string>

/**
 * @class Theme
 * @brief Corresponds to the GUI colorscheme specified in bitclient_gui.json
 */
class Theme
{
public:
    /// Constructs the theme object given the path to the theme configuration file
    explicit Theme(const std::string &cfgFile);

    /// Returns the background color of the main window
    const gui::Color &getBackgroundColor() const;

    /// Returns the background color of a text box
    const gui::Color &getTextBoxColor() const;

    /// Returns the background color of a button
    const gui::Color &getButtonColor() const;

    /// Returns the mouse hover color of a button
    const gui::Color &getButtonHoverColor() const;

private:
    /// Background color
    gui::Color m_bgColor;

    /// Text box color
    gui::Color m_textBoxColor;

    /// Button color
    gui::Color m_buttonColor;

    /// Button hover color
    gui::Color m_buttonHoverColor;
};
