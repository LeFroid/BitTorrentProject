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

#include "FontHolder.h"
#include "GUIObject.h"

namespace gui 
{
    /**
     * @class Label
     * @author Timothy Vaccarelli
     * @date 01/18/2016
     * @file Label.h
     * @brief The label class represents an area of text which does not require a container other than a window object
     */
    class Label : public GUIObject, public FontHolder
    {
    public:
        /// Constructs a label object with a given parent, position, size, font color, and optional text and font parameters
        Label(GUIObject *parent, Position position, Size size, Color fontColor, std::string text = std::string(),
            int fontPointSize = 24, FontType fontType = DROID_SANS);
        
        /// Label destructor
        virtual ~Label();
        
        /// Returns the current text contained in the label object
        const std::string &getText() const;
        
        /// Returns a reference to the color of the label's text
        const Color& getTextColor() const;
        
        /// Sets the label's text to the given string
        void setText(std::string text);
        
        /// Sets the label's text color to the given parameter
        void setTextColor(Color textColor);
        
    public:
        /// Deallocates any used resources
        void destroy() override;
        
        /// Draws the label on to its parent
        void draw() override;
    
    protected:
        /// The text that is presented within the text box
        std::string m_text;
        
        /// The color of the text that will be displayed
        Color m_textColor;
    };
}

