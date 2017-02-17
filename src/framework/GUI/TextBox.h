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

#include <string>

#include "Box.h"
#include "FontHolder.h"

namespace gui 
{    
    /**
     * @class TextBox
     * @author Timothy Vaccarelli
     * @date 01/05/2016
     * @file TextBox.h
     * @brief Represents a box with any kind of text within it
     */
    
    class TextBox : public Box, public FontHolder
    {
    public:
        /// TextBox constructor, specifies the parent object, position, size of the box and its color
        TextBox(GUIObject *parent, Position position, Size size, Color boxColor);
        
        /**
         * TextBox constructor- specifies the parent object, starting coordinates, size of the box, box color,
         * text contained within, the font used to render the text, font point size, and the color of the text (defaults to black)
         */
        TextBox(GUIObject *parent, Position position, Size size, Color boxColor, std::string text,
            FontType fontType = DROID_SANS, int fontPointSize = 24, Color textColor = { 0, 0, 0, 255 });
        
        /// TextBox destructor
        ~TextBox();
        
        /// Returns a reference to the text contained within the box
        const std::string& getText() const;
        
        /// Returns a reference to the color of the text within the box
        const Color& getTextColor() const;
        
        /// Sets the text to be displayed within the box
        virtual void setText(std::string text);
        
        /// Sets the color of the text in the box to the specified parameter
        void setTextColor(Color textColor);

    /// GUIObject virtual function implementations
    public:
        /// Destroys all resources used by the text box
        virtual void destroy() override;
    
        /// Attempts to create and draw the text box
        virtual void draw() override;
        
    protected:
        /// The text that is presented within the text box
        std::string m_text;
        
        /// The color of the text that will be displayed
        Color m_textColor;
    };
}

