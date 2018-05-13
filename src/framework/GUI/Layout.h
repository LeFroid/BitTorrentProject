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

#include "GUIObject.h"

namespace gui 
{
    /// Represents the margins of a layout object
    struct Margins
    {
        int Top;       /// Top margin, in pixels, for the container's items. Defaults to 5
        int Bottom;    /// Bottom margin, in pixels, for a container's items. Defaults to 5
        int Left;      /// Left margin, in pixels, for the container's items. Defaults to 5
        int Right;     /// Right margin, in pixels, for the container's items. Defaults to 5
        
        /// Default margins constructor
        Margins() : Top(5), Bottom(5), Left(5), Right(5) { }
        
        /// Margins constructor, with specified values
        Margins(int top, int bottom, int left, int right) :
            Top(top),
            Bottom(bottom),
            Left(left),
            Right(right)
        {
        }
    };
    
    /**
     * @class Layout
     * @author Timothy Vaccarelli
     * @date 02/02/2016
     * @file Layout.h
     * @brief This is the base class for all geometry handling objects
     */
    class Layout : public GUIObject
    {
    public:
        /**
         * Layout constructor, specifying only a parent object.
         * This sets the size and positon of the layout object to that of its parent.
         * There is an optional parameter for spacing between objects, which defaults to 5 pixels
         */
        Layout(GUIObject *parent, int spacing = 5);
        
        /// Layout constructor, specifying a parent object, position of the layout object, a size and optional param for spacing
        Layout(GUIObject *parent, Position position, Size size, int spacing = 5);

        /// Layout destructor
        virtual ~Layout() {}
        
        /// Returns a pointer to the GUIObject at a specific index in the layout. If there is no object at that index, returns nullptr
        GUIObject *getItemAt(int index);
        
        /// Returns the margins of the layout handler
        const Margins& getMargins() const;
        
        /// Returns the spacing, in pixels, between objects of the layout
        const int& getSpacing() const;
        
        /// Sets the margins of the layout handler to the specified value
        void setMargins(Margins margins);
        
        /// Sets the margins of the layout handler to the specified values
        void setMargins(int top, int bottom, int left, int right);
        
        /// Sets the spacing in between objects to the given parameter
        void setSpacing(int spacing);
        
    public:
        /// Pure virtual function that adds objects to the container
        virtual void addItem(GUIObject *item) = 0;
        
        /// Virtual function that draws all of its children
        virtual void draw() override;
        
    protected:
        /// Called when an item is added or spacing / margins have been changed
        virtual void update() = 0;
        
    protected:
        /// The layout object's margins
        Margins m_margins;
        
        /// The spacing in between objects that are a part of the layout
        int m_spacing;
    };
}

