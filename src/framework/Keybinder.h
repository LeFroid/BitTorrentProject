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

#include <functional>
#include <unordered_map>
#include <utility>
#include <SDL.h>

#include "GUIEvents.h"
#include "Singleton.h"

namespace gui 
{
    typedef std::function<void()> EventCallback;
    
    typedef std::unordered_map< SDL_Keycode, EventCallback > KeyboardEventMap;
    
    /**
     * @class Keybinder
     * @author Timothy Vaccarelli
     * @date 01/11/2016
     * @file Keybinder.h
     * @brief Intermediary between the Event Handler and GUI Objects which binds key input data to trigger specified functions
     */
    class Keybinder : public Singleton<Keybinder>
    {
        friend class Singleton<Keybinder>;
    
    public:
        /// Default constructor
        Keybinder();
        
        /**
         * @brief Invokes the callback function that is bound to a given key and keyboard event
         * @param key The key that was called
         * @param event The event that occurred
         */
        void invokeKeyEvent(SDL_Keycode key, KeyboardEvent event);
        
        /**
         * @brief Determines if a specified keycode is already in use for a keyboard event
         * @param key The SDL keycode in question
         * @param event The keyboard event in question (key press down, key release, etc)
         * @return True if the key is in use, false if else.
         */
        bool isKeyRegistered(SDL_Keycode key, KeyboardEvent event);
        
        /**
         * @brief Attempts to register a keycode as taken for a certain event and bind it to a callback function
         * @param key The keycode that will be used to trigger the callback
         * @param event The event, during which the callback will be triggered
         * @param callback Callback function to be executed when the keycode and event type signal themselves
         * @param ignoreExisting If set to true, this will override any existing key bindings that are already set (defualts to false)
         */
        void registerKeyEvent(SDL_Keycode key, KeyboardEvent event, EventCallback&& callback, bool ignoreExisting = false);
        
        /**
         * @brief Attempts to remove any callback functions assigned to a key at a certain event
         * @param key The key that will be cleared of its registry
         * @param event The event type that is being dealt with (key down, key up, etc)
         */
        void removeKeyEvent(SDL_Keycode key, KeyboardEvent event);
        
    private:
        /// Checks the key down event registry for the given key, returning true if it's in the map.
        bool isKeyDownRegistered(const SDL_Keycode& key);
        
        /// Checks the key up (released) event registry for the given key, returning true if it's in the map.
        bool isKeyUpRegistered(const SDL_Keycode& key);
        
        /// Map of keycodes to callbacks for KeyboardEvent::Pressed events
        KeyboardEventMap m_eventKeyDownMap;
        
        /// Map of keycodes to callbacks for KeyboardEvent::Released events
        KeyboardEventMap m_eventKeyUpMap;
    };
}

