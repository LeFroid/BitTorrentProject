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

#include <functional>

#include "LogHelper.h"
#include "Renderer.h"
#include "TextInputBox.h"
#include "Window.h"

namespace gui 
{
    TextInputBox::TextInputBox(GUIObject *parent, Position position, Size size, Color boxColor, std::string text,
        FontType fontType, int fontPointSize, Color textColor) :
            TextBox(parent, position, size, boxColor, text, fontType, fontPointSize, textColor)
    {
        registerMouseEvent(MouseEvent::ReleasedLeft, std::bind(&TextInputBox::gainKeyboardFocus, this));
    }
    
    TextInputBox::~TextInputBox()
    {
    }
    
    void TextInputBox::onTextInput(std::string text)
    {
        if (isHidden())
            return;
        
        // If there was a preset text in the box and user hits enter, we need to handle this manually
        /*if (text.empty())
        {
            text = getText();
            text.pop_back();
        }*/
        
        setText(text);
        draw();
        getRenderer()->update();
    }

    void TextInputBox::gainKeyboardFocus()
    {
        if (!hasParent())
            return;
                
        // Get the top level parent, which would be the Window
        GUIObject *boxParent = getParent();
        while (boxParent->hasParent())
            boxParent = boxParent->getParent();
                
        static_cast<Window*>(boxParent)->requestKeyboardFocus(getObjectID());
    }
}

