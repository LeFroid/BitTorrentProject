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

#include "LogHelper.h"
#include "Renderer.h"
#include "TextBox.h"

namespace gui 
{
    TextBox::TextBox(GUIObject *parent, Position position, Size size, Color boxColor) :
        Box(parent, position, size, boxColor),
        FontHolder(),
        m_text(),
        m_textColor(Color(0, 0, 0, 255))
    {
    }
    
    TextBox::TextBox(GUIObject *parent, Position position, Size size, Color boxColor, std::string text,
        FontType fontType, int fontPointSize, Color textColor) :
        Box(parent, position, size, boxColor),
        FontHolder(fontType, fontPointSize),
        m_text(text),
        m_textColor(textColor)
    {
    }

    TextBox::~TextBox()
    {
        destroy();
    }
    
    void TextBox::destroy()
    {
    }
    
    void TextBox::draw()
    {        
        Renderer *render = getRenderer();
        if (render == nullptr)
        {
            LOG_ERROR("framework.gui", "In TextBox::draw - Unable to fetch the rendering object on a text box. SDL Error: ", SDL_GetError());
            return;
        }
        
        // Draw the box
        if (!render->drawBox(getSize(), getPosition(), getColor()))
            LOG_ERROR("framework.gui", "In TextBox::draw - Unable to make the renderer draw a box. SDL Error: ", SDL_GetError());
            
        // Draw the text
        if (!getText().empty())
        {
            SDL_Rect objRect = getObjectRect();
            objRect.x += 5;
            objRect.y += 2;
            getFont()->printText(getText(), getTextColor(), getFontPointSize(), objRect, getRenderer());
        }
        setHidden(false);
    }
    
    const std::string& TextBox::getText() const
    {
        return m_text;
    }
    
    const Color& TextBox::getTextColor() const
    {
        return m_textColor;
    }
    
    void TextBox::setText(std::string text)
    {
        m_text = text;
    }
    
    void TextBox::setTextColor(Color textColor)
    {
        m_textColor = textColor;
    }
}

