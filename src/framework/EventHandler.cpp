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

#include <cstdint>
#include "EventHandler.h"
#include "Keybinder.h"
#include "ObjectManager.h"
#include "Window.h"

namespace gui
{
    EventHandler::EventHandler() :
        m_window(nullptr),
        m_lastMouseFocusObject(0),
        m_inputText()
    {
    }

    void EventHandler::setWindowPtr(Window *mainWindow)
    {
        m_window = mainWindow;
    }
    
    void EventHandler::pollForEvents()
    {
        SDL_Event event;
        bool quitLoop = false;
        
        while (!quitLoop)
        {
            while (SDL_PollEvent(&event))
            //while (SDL_WaitEvent(&event))
            {
                switch (event.type)
                {
                    case SDL_WINDOWEVENT:
                        handleWindowEvent(event);
                        break;
                    case SDL_MOUSEMOTION:
                    case SDL_MOUSEBUTTONDOWN:
                    case SDL_MOUSEBUTTONUP:
                    case SDL_MOUSEWHEEL:
                        handleMouseEvent(event);
                        break;
                    case SDL_KEYDOWN:
                    case SDL_KEYUP:
                    case SDL_TEXTEDITING:
                    case SDL_TEXTINPUT:
                        handleKeyboardEvent(event);
                        break;
                    case SDL_USEREVENT:
                    {
                        // Mouse hover callback
                        void (*x)(void*) = (void (*)(void*)) event.user.data1;
                        x(event.user.data2);
                        break;
                    }
                    case SDL_QUIT:
                        quitLoop = true;
                        break;
                    default:
                        break;
                }
            }
            SDL_Delay(5);
        }
    }

    const GUIObjectID &EventHandler::getObjWithMouseID()
    {
        return m_lastMouseFocusObject;
    }
    
    MouseEvent EventHandler::getMouseEventName(const SDL_Event& event)
    {
        MouseEvent eventName;
        if (event.type == SDL_MOUSEBUTTONDOWN)
        {
            if (event.button.button == SDL_BUTTON_LEFT)
                eventName = MouseEvent::PressedLeft;
            else if (event.button.button == SDL_BUTTON_RIGHT)
                eventName = MouseEvent::PressedRight;
        }
        else if (event.type == SDL_MOUSEBUTTONUP)
        {
            if (event.button.button == SDL_BUTTON_LEFT)
                eventName = MouseEvent::ReleasedLeft;
            else if (event.button.button == SDL_BUTTON_RIGHT)
                eventName = MouseEvent::ReleasedRight;
        }
        else if (event.type == SDL_MOUSEMOTION)
            eventName = MouseEvent::Moved;
        else if (event.type == SDL_MOUSEWHEEL)
            eventName = MouseEvent::Wheel;
            
        return eventName;
    }
    
    void EventHandler::handleKeyboardEvent(SDL_Event& event)
    {
        Keybinder *keybinder = Keybinder::getInstance();
                
        // If an object requested keyboard focus, stop the keybinder object from doing anything
        bool haltKeybinder = m_window->childHasKeyboardFocus();
        
        GUIObject *focusedObject = nullptr;
        if (haltKeybinder)
            focusedObject = ObjectManager::getInstance()->getObject(m_window->getChildWithKeyboardFocus());
        
        if (event.type == SDL_KEYDOWN)
        {
            if (haltKeybinder)
            {
                switch (event.key.keysym.sym)
                {
                    // Remove the last character that was entered
                    case SDLK_BACKSPACE:
                        m_inputText.pop_back();
                        if (focusedObject) 
                            focusedObject->onTextInput(m_inputText);
                        break;
                    // Remove keyboard focus from the object that has it
                    case SDLK_RETURN:
                    case SDLK_RETURN2:
                    case SDLK_KP_ENTER:
                        m_window->relinquishKeyboardFocus();
                        if (focusedObject)
                            focusedObject->onLostKeyboardFocus();
                        break;
                }
            }
            else
                keybinder->invokeKeyEvent(event.key.keysym.sym, KeyboardEvent::Pressed);
        }
        else if (event.type == SDL_KEYUP)
        {
            if (!haltKeybinder)
                keybinder->invokeKeyEvent(event.key.keysym.sym, KeyboardEvent::Released);
        }
        else if (haltKeybinder && event.type == SDL_TEXTINPUT)
        {
            m_inputText.append(event.text.text);
            if (focusedObject)
                focusedObject->onTextInput(m_inputText);
        }
    }
    
    void EventHandler::handleMouseEvent(SDL_Event& event)
    {
        MouseEvent eventName = getMouseEventName(event);
        GUIObject *handler = getMouseHandlerObject(m_window->getObjectID(), event, eventName);
        
        if (handler)
            handler->invokeMouseEvent(eventName);
        
        // Check if any one object has lost mouse focus, inform the object
        if (eventName == MouseEvent::Moved)
        {
            uint32_t tmpID = (handler == nullptr) ? 0 : handler->getObjectID();
            
            if (m_lastMouseFocusObject != tmpID)
            {
                GUIObject *lastActiveObj = ObjectManager::getInstance()->getObject(m_lastMouseFocusObject);
                if (lastActiveObj != nullptr)
                    lastActiveObj->invokeMouseEvent(MouseEvent::LostFocus);
            }
            
            m_lastMouseFocusObject = tmpID;
        }
        else if (eventName == MouseEvent::ReleasedLeft && m_window->childHasKeyboardFocus())
        {
            // Check if an object has just lost keyboard focus, and inform it if so
            GUIObject *focusedObject = ObjectManager::getInstance()->getObject(m_window->getChildWithKeyboardFocus());
            
            if (focusedObject)
            {
                if (!isPositionWithinObject(Position(event.button.x, event.button.y), focusedObject))
                {
                    m_window->relinquishKeyboardFocus();
                    focusedObject->onLostKeyboardFocus();
                }
            }
        }
    }
    
    void EventHandler::handleWindowEvent(SDL_Event& event)
    {
        if (m_window)
            m_window->handleEvent(event.window);
    }
    
    bool EventHandler::isPositionWithinObject(Position pos, GUIObject *object)
    {
        if (!object)
            return false;
        
        const Position& objPos = object->getPosition();
        const Size& objSize    = object->getSize();
            
        if (pos.x < objPos.x || pos.x > (objPos.x + objSize.width) 
            || pos.y < objPos.y || pos.y > (objPos.y + objSize.height))
        {
            return false; 
        }
        
        return true;
    }
    
    GUIObject *EventHandler::getMouseHandlerObject(GUIObjectID currentObjectID, SDL_Event& event, MouseEvent& eventName)
    {
        ObjectManager *objMgr = ObjectManager::getInstance();
        
        GUIObject *currentObject = objMgr->getObject(currentObjectID);
        
        if (!currentObject)
            return nullptr;

        GUIObject *retVal = nullptr;
            
        if (!currentObject->isHidden() && currentObject->hasMouseEvent(eventName))
        {
            // Get mouse coordinates
            Position mousePos;
            
            if (eventName == MouseEvent::Moved || eventName == MouseEvent::Wheel)
            {
                // Using SDL_MouseMotionEvent structure (event.motion)
                mousePos.x = event.motion.x;
                mousePos.y = event.motion.y;
            }
            else
            {
                // Using SDL_MouseButtonEvent structure (event.button)
                mousePos.x = event.button.x;
                mousePos.y = event.button.y;
            }
            
            // Check if object is applicable at its current position
            if (!isPositionWithinObject(mousePos, currentObject))
                return nullptr;
            
            // So far so good
            retVal = currentObject;
        }

        // See if any children qualify for the mouse event
        for (auto it = currentObject->getChildIterBegin(); it != currentObject->getChildIterEnd(); ++it)
        {
            GUIObject *tmp = getMouseHandlerObject(*it, event, eventName);
            if (tmp != nullptr)
                retVal = tmp;
        }
        
        return retVal;
    }
}

