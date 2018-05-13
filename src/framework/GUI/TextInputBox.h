/*
Copyright (c) 2016-2017, Timothy Vaccarelli
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

#include "EventHandler.h"
#include "TextBox.h"

namespace gui 
{
    /**
     * @class TextInputBox
     * @author Timothy Vaccarelli
     * @date 01/19/2016
     * @file TextInputBox.h
     * @brief Represents a box that is used to receive text input from the user when activated
     */
    class TextInputBox : public TextBox
    {
        friend class EventHandler;
    public:
        /// TextInputBox constructor. Required parameters are parent object, position and size. Optionals are box color, text, font name, font size, text color
        TextInputBox(GUIObject *parent, Position position, Size size, Color boxColor = { 255, 255, 255, 255 },
            std::string text = std::string(), FontType fontType = DROID_SANS, int fontPointSize = 24,
            Color textColor = { 0, 0, 0, 255 });
        
        /// TextInputBox destructor
        virtual ~TextInputBox();

    protected:        
        /// Called when this has focus and text has been entered by the user
        virtual void onTextInput(std::string text) override;
        
        /// Called when this object attempts to get the keyboard focus
        void gainKeyboardFocus();
    };
}

