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

#include <SDL.h>

#include "GUIEvents.h"
#include "GUIProperties.h"

#include "Singleton.h"

namespace gui 
{
    class GUIObject;
    class Window;
    
    /**
     * @class EventHandler
     * @author Timothy Vaccarelli
     * @date 01/10/2016
     * @file EventHandler.h
     * @brief Handles all relevant events that the application can receive from the SDL backend
     */
    class EventHandler : public Singleton<EventHandler>
    {
        friend class Singleton<EventHandler>;

    public:
        /// Constructs the event handler
        EventHandler();
        
        /// Sets the pointer to the main window, used for handling UI events
        void setWindowPtr(Window *mainWindow);

        /// Executes a loop that continues so long as there are events to be handled
        void pollForEvents();
        
        /// Returns the ID of the object that the mouse is presently hovering on
        const GUIObjectID &getObjWithMouseID();

    private:
        /// Determines and returns the mouse event, given the SDL enum
        MouseEvent getMouseEventName(const SDL_Event& event);
        
        /// Handles keyboard related events
        void handleKeyboardEvent(SDL_Event& event);
        
        /// Handles mouse related events
        void handleMouseEvent(SDL_Event& event);
        
        /// Handles window related events
        void handleWindowEvent(SDL_Event& event);
        
        /// Returns true if a given position falls within the area of the specified object, false if else
        bool isPositionWithinObject(Position pos, GUIObject *object);
        
        /**
         * Recursively searches down via child objects for a GUI Object that
         * is on the coordinates of a mouse event and also implements the mouse
         * event currently being handled (hover, click, etc).
         * @return A pointer to the GUIObject that has a mouse event to be invoked, or nullptr if none exists
         */
        GUIObject *getMouseHandlerObject(GUIObjectID currentObjectID, SDL_Event& event, MouseEvent& eventName);
        
        /// The main application window, which contains all other GUI Objects
        Window *m_window;
        
        /// The last object that had the mouse's focus
        GUIObjectID m_lastMouseFocusObject;
        
        /// This is used to contain any text that is sent to the event system when an object has keyboard focus
        std::string m_inputText;
    };
}

