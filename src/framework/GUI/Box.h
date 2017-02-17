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

#include "GUIObject.h"

namespace gui 
{
    /**
     * @class Box
     * @author Timothy Vaccarelli
     * @date 01/05/2016
     * @file Box.h
     * @brief Represents a simple rectangle with an assigned color, size and position
     */
    class Box : public GUIObject
    {
    public:
        /// Constructs a box with a given parent, position, size and color
        Box(GUIObject *parent, Position position, Size size, Color color);
        
        /// Returns the color of the box
        const Color& getColor() const;
        
        /// Returns the color of the box, when the mouse is hovering above it
        const Color& getMouseHoverColor() const;
        
        /// Sets the color of the box to that of the parameter
        void setColor(Color boxColor);
        
        /// Defines a color to be assigned to the box when the mouse is hovering above it
        void setMouseHoverColor(Color hoverColor);
        
    public:
        /// Attempts to create and draw the box
        virtual void draw() override;
    
    protected:
        /// Informs the box that it has gained focus from the mouse
        void informGainOfMouseFocus();
        
        /// Informs the box that it has lost focus from the mouse
        void informLossOfMouseFocus();
    
    private:
        /// The color of the box
        Color m_color;
        
        /// The color of the box, while being hovered over
        Color m_hoverColor;
        
        /// True if the mouse is hovering over the box, false if else
        bool m_isMouseHovering;
    };
}

