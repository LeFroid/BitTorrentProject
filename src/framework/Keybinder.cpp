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

#include "Keybinder.h"

namespace gui 
{
    Keybinder::Keybinder() :
        m_eventKeyDownMap(),
        m_eventKeyUpMap()
    {
    }
    
    void Keybinder::invokeKeyEvent(SDL_Keycode key, KeyboardEvent event)
    {
        if (event == KeyboardEvent::Pressed)
        {
            auto result = m_eventKeyDownMap.find(key);
            if (result != m_eventKeyDownMap.end())
                result->second();
        }
        else if (event == KeyboardEvent::Released)
        {
            auto result = m_eventKeyUpMap.find(key);
            if (result != m_eventKeyUpMap.end())
                result->second();
        }
    }
    
    bool Keybinder::isKeyRegistered(SDL_Keycode key, KeyboardEvent event)
    {
        if (event == KeyboardEvent::Pressed) // key down
        {
            return isKeyDownRegistered(key);
        }
        else if (event == KeyboardEvent::Released) // key up
        {
            return isKeyUpRegistered(key);
        }
        
        return false;
    }
    
    bool Keybinder::isKeyDownRegistered(const SDL_Keycode& key)
    {
        return (m_eventKeyDownMap.find(key) != m_eventKeyDownMap.end());
    }
    
    bool Keybinder::isKeyUpRegistered(const SDL_Keycode& key)
    {
        return (m_eventKeyUpMap.find(key) != m_eventKeyUpMap.end());
    }
    
    void Keybinder::registerKeyEvent(SDL_Keycode key, KeyboardEvent event, EventCallback&& callback, bool ignoreExisting)
    {
        if (!ignoreExisting)
        {
            // Check if the key is already registered for something
            if (isKeyRegistered(key, event))
                return;
        }
            
        if (event == KeyboardEvent::Pressed)
        {
            m_eventKeyDownMap[key] = std::move(callback);
        }
        else if (event == KeyboardEvent::Released)
        {
            m_eventKeyUpMap[key] = std::move(callback);
        }
    }
    
    void Keybinder::removeKeyEvent(SDL_Keycode key, KeyboardEvent event)
    {
        if (event == KeyboardEvent::Pressed)
        {
            m_eventKeyDownMap.erase(key);
        }
        else if (event == KeyboardEvent::Released)
        {
            m_eventKeyUpMap.erase(key);
        }
    }
}

