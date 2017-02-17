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

#include "Label.h"
#include "LogHelper.h"
#include "Renderer.h"

namespace gui 
{
    Label::Label(GUIObject *parent, Position position, Size size, Color fontColor, std::string text, int fontPointSize,
        FontType fontType) :
            GUIObject(parent, position, size),
            FontHolder(fontType, fontPointSize),
            m_text(text),
            m_textColor(fontColor)
    {
    }
     
    Label::~Label()
    {
        destroy();
    }
    
    const std::string &Label::getText() const
    {
        return m_text;
    }
    
    const Color& Label::getTextColor() const
    {
        return m_textColor;
    }
    
    void Label::setText(std::string text)
    {
        m_text = text;
    }

    void Label::setTextColor(Color textColor)
    {
        m_textColor = textColor;
    }
    
    void Label::destroy()
    {
    }
    
    void Label::draw()
    {        
        // Draw the text
        if (!getText().empty())
            getFont()->printText(getText(), getTextColor(), getFontPointSize(), getObjectRect(), getRenderer());

        setHidden(false);
    }
}

