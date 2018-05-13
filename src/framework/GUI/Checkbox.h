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

#include "Box.h"

namespace gui 
{
    /**
     * @class Checkbox
     * @author Timothy Vaccarelli
     * @date 02/01/2016
     * @file Checkbox.h
     * @brief Represents a checkbox, which can be used for various purposes such as configuration setting markers
     */
    class Checkbox : public Box
    {
    public:
        /// The different display modes a checkbox can have.
        enum Mode
        {
            Box    = 0,
            Circle = 1
        };
        
    public:
        /// Checkbox constructor
        Checkbox(GUIObject *parent, Position position, Size size = { 16, 16 }, Color color = { 255, 255, 255, 255 }, Mode mode = Circle);

        /// Checkbox destructor
        virtual ~Checkbox() {}
        
        /// Returns the current display mode of the checkbox
        const Mode& getDisplayMode() const;
        
        /// Returns true if the checkbox is checked, false if else
        bool isChecked() const;
        
        /**
         * @brief Updates the box's status to either checked or unchecked
         * @param checked True if now checked, false if unchecked
         */
        void setChecked(bool checked);
        
        /// Sets the checkbox's display mode (must be redrawn after calling this)
        void setDisplayMode(Mode displayMode);
        
    public:
        /// Draws the checkbox on the screen
        virtual void draw() override;
        
    protected:
        /// Called when the checkbox has been clicked by the mouse
        void onMouseClick();
        
        /// Ensures the radius value has been set, if it is needed
        void setRadius();
        
        /// True if the checkbox has been checked, false if else
        bool m_isChecked;
        
        /// The current display mode of the checkbox
        Mode m_mode;
        
        /// The radius of the checkbox
        float m_radius;
    };
}

