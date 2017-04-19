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

#include <memory>
#include <string>
#include <SDL.h>

#include "GUIObject.h"

namespace gui 
{
    class Renderer;
    
    /**
     * @class Window
     * @author Timothy Vaccarelli
     * @date 01/04/2016
     * @file Window.h
     * @brief Object that is responsible for drawing the window on the client's screen. This maintains ownership of the renderer
     */
    class Window : public GUIObject
    {
    public:
        /**
         * @brief Window constructor with minimal parameters. Creates and draws the window
         * @param title  Title of the window
         * @param width  Width, in pixels, of the window
         * @param height Height, in pixels, of the window
         * @param fullscreen Defaults to false; if true this sets the window to fullscreen mode
         */
        Window(std::string title, int width, int height, bool fullscreen = false);
        
        /// Window destructor
        ~Window();
        
        /// Returns true if a child object has the keyboard's focus, halting keybinding events. False if else
        bool childHasKeyboardFocus();
        
        /// Removes all existing, visible objects from the window, returning true on success
        bool clearContent();
        
        /// Returns the identifier of the child object (if any) that currently has keyboard focus
        const GUIObjectID& getChildWithKeyboardFocus() const;
        
        /// Returns a raw pointer to the rendering object
        Renderer *getRenderer() override;
        
        /// Handles an event that has been sent to the window by the event system
        void handleEvent(SDL_WindowEvent& event);
        
        /// Called when a child object is done accepting keyboard / text input
        void relinquishKeyboardFocus();
        
        /// Called by a child object that is requesting keyboard focus for text input and such
        void requestKeyboardFocus(GUIObjectID objectID);
        
        /// Sets the background color of the window to the color specified here
        void setBackgroundColor(Color bgColor);

        /// Removes all child objects, with the exception of the object of the ID specified
        void removeChildrenExcept(GUIObjectID child);
    
        /// Redraws the children objects of this window, scaling their size to the parent if applicable
        void redraw(bool scaleSize = false, Size oldSize = {0, 0});

    /// GUIObject virtual function implementations
    public:
        /// Attempts to create and draw the window
        virtual void draw() override;
        
        /// Closes and destroys the window
        void destroy() override;
        
    private:
        /// Redraws the children of the specified object, if they are not hidden
        void redraw(GUIObject *object);

        /// Modifies the scale of the given child object
        void changeScale(GUIObject *object, const float &widthRatio, const float &heightRatio);
        
        /// Sets the keyboard focus object to the object of the provided ID
        void setKeyboardFocusObject(GUIObjectID objectID);
        
    private:
        /// Window flags
        SDL_WindowFlags m_flags;
        
        /// Window title
        std::string m_title;
        
        /// SDL window pointer
        SDL_Window *m_window;
        
        /// Window rendering object
        std::shared_ptr<Renderer> m_renderer;
        
        /**
         * If a child widget currently has keyboard focus (text input, etc), this
         * is equal to that object's unique identifier. If no object has keyboard
         * focus, this is equal to the Window's identifier (which is 0).
         */
        GUIObjectID m_childWithKeyboardFocus;
    };
}

