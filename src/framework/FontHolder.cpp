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

#include "FontHolder.h"

namespace gui 
{
    FontHolder::FontHolder(FontType fontType, int fontPointSize) :
        m_font(FontStorage::getInstance()->getFont(fontType, fontPointSize)),
        m_fontPointSize(fontPointSize),
        m_fontType(fontType)
    {
    }

    Font *FontHolder::getFont()
    {
        return m_font;
    }

    int FontHolder::getFontPointSize() const
    {
        return m_fontPointSize;
    }

    const FontType &FontHolder::getFontType() const
    {
        return m_fontType;
    }

    void FontHolder::setFontPointSize(int size)
    {
        if (size == m_fontPointSize)
            return;

        m_fontPointSize = size;
        m_font = FontStorage::getInstance()->getFont(m_fontType, size);
    }

    void FontHolder::setFontType(FontType fontType)
    {
        if (fontType == m_fontType)
            return;

        m_fontType = fontType;
        m_font = FontStorage::getInstance()->getFont(fontType, m_fontPointSize);
    }
}

