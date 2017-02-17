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

#include "FontStorage.h"

namespace gui 
{
    /**
     * @class FontHolder
     * @brief Represents an object that holds a pointer to a font, for
     *        the rendering of text.
     * @ingroup FontSystem
     */
    class FontHolder
    {
    public:
        /// Constructs a font holder - which fetches a font pointer from the
        /// storage system given a font type and a font point size
        FontHolder(FontType fontType = DROID_SANS, int fontPointSize = 18);

        /// Returns the font currently used by the holder
        Font *getFont();

        /// Returns the font's point size
        int getFontPointSize() const;

        /// Returns the type of font being used by the holder of the font
        const FontType &getFontType() const;

        /// Sets the holder's font to one of the given font types
        void setFontType(FontType fontType);

        /// Sets the holder's font point size to the given parameter
        void setFontPointSize(int size);

    protected:
        /// Pointer to the font being used
        Font *m_font;

        /// The font point size used by the font
        int m_fontPointSize;

        /// The type of font being used
        FontType m_fontType;
    };
}

