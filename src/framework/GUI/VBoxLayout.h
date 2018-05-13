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

#include "Layout.h"

namespace gui 
{
    /**
     * @class VBoxLayout
     * @author Timothy Vaccarelli
     * @date 02/03/2016
     * @file VBoxLayout.h
     * @brief Vertical box layout object, aligns the size of its sub-items automatically.
     */
    class VBoxLayout : public Layout
    {
    public:
        /// VBoxLayout, specifying a parent and (optional) spacing between objects
        VBoxLayout(GUIObject *parent, int spacing = 5);
        
        /// VBoxLayout constructor, specifying a parent object, position of the layout object, a size and optional param for spacing
        VBoxLayout(GUIObject *parent, Position position, Size size, int spacing = 5);

        /// VBoxLayout destructor
        virtual ~VBoxLayout() {}
        
    public:
        /// Adds a child object to the horizontal box layout
        virtual void addItem(GUIObject *item) override;
        
    protected:
        /// Updates the size & position of layout items.
        virtual void update() override;
        
        /// Calculates excess height in case a widget requests more than the default allocated space
        int getExcessHeight();
    };
}

