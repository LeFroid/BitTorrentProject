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

#include <algorithm>
#include <utility>

#include "EventHandler.h"
#include "GUIObject.h"
#include "Font.h"
#include "FontStorage.h"
#include "Keybinder.h"
#include "LogHelper.h"
#include "ObjectManager.h"
#include "Renderer.h"
#include "Window.h"

namespace gui 
{
    GUIObject::GUIObject(GUIObject *parent) :
        m_children(),
        m_mouseHandler(),
        m_parent(parent),
        m_position(),
        m_size(),
        m_isHiding(true),
        m_objectID(0)
    {
    }
    
    GUIObject::GUIObject(GUIObject *parent, Position position, Size size, bool overrideBounds) :
        m_children(),
        m_mouseHandler(),
        m_parent(parent),
        m_position(position),
        m_size(size),
        m_isHiding(true),
        m_objectID(0)
    {   
        if (!overrideBounds)
            confirmSizeAndPos();
    }

    GUIObject::~GUIObject()
    {
        removeChildren();
    }
    
    void GUIObject::addChild(GUIObjectID childID)
    {
        m_children.push_back(childID);
    }
    
    void GUIObject::confirmSizeAndPos()
    {
        confirmPosition();
        confirmSize();
    }
    
    void GUIObject::confirmPosition()
    {
        if (!hasParent())
            return;
        
        GUIObject *parent = getParent();
        
        // Child objects cannot be larger or outside of the boundaries of the parent objects
        const Position& parentPos = parent->getPosition();
        
        // Set size / position to that of parent if it goes over the limits
        if (m_position.x < parentPos.x)
            m_position.x = parentPos.x;
            
        if (m_position.y < parentPos.y)
            m_position.y = parentPos.y;
        
        // Confirm child object positions as well
        for (auto itr = getChildIterBegin(); itr != getChildIterEnd(); ++itr)
        {
            GUIObject *obj = ObjectManager::getInstance()->getObject((*itr));
            if (obj != nullptr)
                obj->confirmPosition();
        }
    }
    
    void GUIObject::confirmSize()
    {
        if (!hasParent())
            return;
        
        GUIObject *parent = getParent();
        
        // Child objects cannot be larger or outside of the boundaries of the parent objects
        const Size& parentSize = parent->getSize();
        
        if (m_size.width > parentSize.width)
            m_size.width = parentSize.width;
            
        if (m_size.height > parentSize.height)
            m_size.height = parentSize.height;
        
        int parentWidthWithCoords = parentSize.width + parent->getPosition().x;
        if (m_size.width + m_position.x > parentWidthWithCoords)
            m_size.width -= m_size.width + m_position.x - parentWidthWithCoords;
            
        int parentHeightWithCoords = parentSize.height + parent->getPosition().y;
        if (m_size.height + m_position.y > parentHeightWithCoords)
            m_size.height -= m_size.height + m_position.y - parentHeightWithCoords;
            
        // Confirm child object sizes as well
        for (auto itr = getChildIterBegin(); itr != getChildIterEnd(); ++itr)
        {
            GUIObject *obj = ObjectManager::getInstance()->getObject((*itr));
            if (obj != nullptr)
                obj->confirmSize();
        }
    }
    
    SDL_Surface *GUIObject::createObjectSurface()
    {
        SDL_Surface *surface = SDL_CreateRGBSurface(0, getSize().width, getSize().height, 32, 0, 0, 0, 0);
        if (surface == nullptr)
        {
            LOG_WARNING("framework.gui", "Unable to create a new SDL Surface. Error message: ", SDL_GetError());
            return nullptr;
        }
        
        // Let the surface blend into the whole
        if (SDL_SetSurfaceBlendMode(surface, SDL_BLENDMODE_BLEND) != 0)
            LOG_WARNING("framework.gui", "Unable to set the blend mode of a new SDL Surface. Error message: ", SDL_GetError());
            
        return surface;
    }
    
    GUIObjectContainer::iterator GUIObject::getChildIterBegin()
    {
        return m_children.begin();
    }
    
    GUIObjectContainer::iterator GUIObject::getChildIterEnd()
    {
        return m_children.end();
    }
    
    std::size_t GUIObject::getNumChildren()
    {
        return m_children.size();
    }
    
    const GUIObjectID& GUIObject::getObjectID() const
    {
        return m_objectID;
    }
    
    GUIObject* GUIObject::getParent() const
    {
        return m_parent;//ObjectManager::getInstance()->getObject(m_parent);
    }
    
    const Position& GUIObject::getPosition()
    {
        return m_position;
    }
    
    SDL_Rect GUIObject::getObjectRect()
    {
        return { m_position.x, m_position.y, m_size.width, m_size.height };
    }
    
    Renderer *GUIObject::getRenderer()
    {
        GUIObject *tmp = this;
        
        // The top level object contains the actual renderer
        while (tmp->hasParent())
            tmp = tmp->getParent();
            
        return static_cast<Window*>(tmp)->getRenderer();
    }
    
    const Size& GUIObject::getSize()
    {
        return m_size;
    }
    
    bool GUIObject::hasMouseEvent(MouseEvent event) const
    {
        return (m_mouseHandler.find(event) != m_mouseHandler.end());
    }
    
    bool GUIObject::hasParent() const
    {
        return m_parent != nullptr;//ObjectManager::getInstance()->existsObject(m_parent);
    }
    
    void GUIObject::hide()
    {
        setHidden(true);
        
        for (auto& i : m_children)
        {
            GUIObject *child = ObjectManager::getInstance()->getObject(i);
            if (child)
                child->hide();
        }
   
        getRenderer()->clearContent();
    }
    
    bool GUIObject::isHidden() const
    {
        return m_isHiding;
    }
    
    void GUIObject::removeChild(GUIObjectID childID)
    {
        auto child = std::find(m_children.begin(), m_children.end(), childID);
        if (child != m_children.end())
            m_children.erase(child);
    }
    
    void GUIObject::removeChildren()
    {
        for (auto itr = getChildIterBegin(); itr != getChildIterEnd(); ++itr)
        {
            ObjectManager::getInstance()->deleteObject((*itr));
        }
        m_children.clear();
    }
    
    void GUIObject::registerWithParent()
    {
        if (hasParent())
            getParent()->addChild(getObjectID());
    }
    
    void GUIObject::registerMouseEvent(MouseEvent event, MouseEventCallback&& callback)
    {
        m_mouseHandler[event] = std::move(callback);
    }
    
    void GUIObject::registerMouseEvent(MouseEvent event, SDL_Keycode shortcut, MouseEventCallback&& callback)
    {
        m_mouseHandler[event] = std::move(callback);
        
        EventCallback kbCallback = m_mouseHandler.at(event);
        Keybinder::getInstance()->registerKeyEvent(shortcut, KeyboardEvent::Released, std::move(kbCallback), true);
    }

    void GUIObject::setHint(std::string text)
    {
        m_isShowingHint = false;

        // Check for previous mouse events, save them
        if (m_hintText.empty())
        {
            if (!m_mouseHoverWithHint && m_mouseHandler.find(MouseEvent::Moved) != m_mouseHandler.end())
                m_mouseHoverWithHint = std::move(m_mouseHandler.at(MouseEvent::Moved));

            if (!m_mouseLostFocusWithHint && m_mouseHandler.find(MouseEvent::LostFocus) != m_mouseHandler.end())
                m_mouseLostFocusWithHint = std::move(m_mouseHandler.at(MouseEvent::LostFocus));
        }

        Font *hintFont = FontStorage::getInstance()->getFont(DROID_SANS);
        hintFont->setPointSize(18);

        // Get the window object, set hint box size to 1/4 window's width
        GUIObject *win = ObjectManager::getInstance()->getObject(0);

        m_hintBoxSize.width = win->getSize().width / 4;
        SDL_Rect tmpRect = { 0, 0, m_hintBoxSize.width - 10, 0 };

        // Add proper amount of newlines to the text to fit the hint box
        m_hintText = hintFont->reformatTextBlock(text, tmpRect);

        // Calculate height of hint box by (number of newlines) * font->getLineSkip()
        int numLines = std::count(m_hintText.begin(), m_hintText.end(), '\n');
        m_hintBoxSize.height = numLines * hintFont->getLineSkip() + 10;

        // Register events and restore font size to original
        registerMouseEvent(MouseEvent::Moved, std::bind(&GUIObject::showObjectHint, this));
        registerMouseEvent(MouseEvent::LostFocus, std::bind(&GUIObject::hideObjectHint, this));
    }

    /* C callbacks for displaying object hints */
    Uint32 objHintTimerCallback(Uint32 /*interval*/, void *param)
    {
        GUIObject *obj = (GUIObject*)param;
        obj->objectHintTimerCallback();
        return 0;
    }

    void dispHint(void *param)
    {
        GUIObject *obj = (GUIObject*)param;
        obj->displayHint();
    }

    void GUIObject::showObjectHint()
    {
        if (m_isShowingHint || isHidden())
            return;

        if (m_mouseHoverWithHint)
            m_mouseHoverWithHint();

        m_isShowingHint = true;

        // set timer for 0.3 second to see if hint should be displayed
        m_mouseHoverTimer = SDL_AddTimer(300, objHintTimerCallback, this);
    }
    
    void GUIObject::objectHintTimerCallback()
    {
        // Make sure this is still being hovered over
        if (EventHandler::getInstance()->getObjWithMouseID() != getObjectID())
            return;

        // Push mouse hover event onto event handler
        SDL_Event event;
        SDL_UserEvent userevent;

        userevent.type = SDL_USEREVENT;
        userevent.code = 0;
        userevent.data1 = (void*)&dispHint;
        userevent.data2 = this;

        event.type = SDL_USEREVENT;
        event.user = userevent;

        SDL_PushEvent(&event);
    }

    void GUIObject::displayHint()
    {
        // Get window object
        GUIObject *win = ObjectManager::getInstance()->getObject(0);

        SDL_Rect hintRect = { m_position.x, m_position.y, m_hintBoxSize.width, m_hintBoxSize.height };

        // If box cannot fit towards the left side of the object,
        // it must fit towards the right since the total width = 1/4 of the window
        if (m_position.x - m_hintBoxSize.width < 0)
            hintRect.x += m_size.width + 5;
        else
            hintRect.x -= m_hintBoxSize.width;

        // Same logic applies to the vertical alignment of the hint box
        if (m_position.y + m_hintBoxSize.height > win->getSize().height)
            hintRect.y -= m_hintBoxSize.height;

        Renderer *render = getRenderer();

        // Render white box w/ black outline, black text, font size 18
        Position boxPos(hintRect.x, hintRect.y);
        render->drawBox(m_hintBoxSize, boxPos, Color(255, 255, 255, 255));
        render->drawRectOutline(m_hintBoxSize, boxPos, Color(0, 0, 0, 255));

        // Render font
        Color hintColor(0, 0, 0, 255);
        Font *hintFont = FontStorage::getInstance()->getFont(DROID_SANS);
        hintRect.x += 5;
        hintRect.w -= 5;
        hintRect.y += 2;
        hintRect.h -= 2;
        hintFont->printText(m_hintText, hintColor, 18, hintRect, render);

        render->update();
    }

    void GUIObject::hideObjectHint()
    {
        m_isShowingHint = false;

        if (m_mouseLostFocusWithHint)
            m_mouseLostFocusWithHint();

        // Get window object
        GUIObject *tmp = this;
        while (tmp->hasParent())
            tmp = tmp->getParent();

        Window *win = static_cast<Window*>(tmp);
        win->redraw();
    }

    void GUIObject::setParent(GUIObjectID parentID)
    {
        GUIObject *parent = ObjectManager::getInstance()->getObject(parentID);
        if (parent != nullptr)
            setParent(parent);
    }
    
    void GUIObject::setParent(GUIObject *parent)
    {
        GUIObject *oldParent = getParent();
        if (oldParent != nullptr)
            oldParent->removeChild(getObjectID());
        
        if (parent != nullptr)
            m_parent = parent; //->getObjectID();
        
        registerWithParent();
    }
    
    void GUIObject::setPosition(Position newPosition, bool verifyBounds)
    {
        m_position = newPosition;

        if (verifyBounds)
            confirmPosition();
    }
    
    void GUIObject::setSize(Size newSize, bool verifyBounds)
    {
        m_size = newSize;

        if (verifyBounds)
            confirmSize();
    }
    
    void GUIObject::invokeMouseEvent(MouseEvent event)
    {
        auto result = m_mouseHandler.find(event);
        if (result != m_mouseHandler.end())
            result->second();
    }
    
    void GUIObject::setHidden(bool value)
    {
        m_isHiding = value;
    }

    void GUIObject::setChildrenHidden(bool value)
    {
        ObjectManager *objMgr = ObjectManager::getInstance();
        for (const auto& it : m_children)
        {
            GUIObject *obj = objMgr->getObject(it);
            if (obj)
            {
                obj->setHidden(value);
                obj->setChildrenHidden(value);
            }
        }
    }
}

