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
#include <map>
#include <string>
#include <vector>
#include <SDL.h>

#include "GUIEvents.h"
#include "GUIProperties.h"

namespace gui 
{
    class EventHandler;
    class GUIObject;
    class ObjectManager;
    class Renderer;
    
    // Callback function typedefs
    typedef std::function<void()> MouseEventCallback;
    
    // Container typedefs
    typedef std::map< MouseEvent, MouseEventCallback > MouseEventMap;
    typedef std::vector<GUIObjectID> GUIObjectContainer;
    
    /**
     * @class GUIObject
     * @author Timothy Vaccarelli
     * @date 01/04/2016
     * @file GUIObject.h
     * @brief The base class for all graphical objects
     */
    class GUIObject
    {
        friend class EventHandler;
        friend class ObjectManager;
        
    public:
        /// Default constructor
        GUIObject(GUIObject *parent = nullptr);
        
        /// Constructs the object with a given parent, starting position and size
        GUIObject(GUIObject *parent, Position position, Size size, bool overrideBounds = false);
        
        /// GUIObject destructor
        virtual ~GUIObject();
        
        /// Adds a child object to the children container
        void addChild(GUIObjectID childID);
        
        /// Returns an iterator pointing to the first element in the child object container
        GUIObjectContainer::iterator getChildIterBegin();
        
        /// Returns an iterator pointing to the last element in the child object container
        GUIObjectContainer::iterator getChildIterEnd();
        
        /// Returns the number of children that belong to this object
        std::size_t getNumChildren();
        
        /// Returns the object's unique identifier
        const GUIObjectID& getObjectID() const;
        
        /// Returns the parent of this object, nullptr if none exists
        GUIObject *getParent() const;
        
        /// Returns the position of the object
        const Position& getPosition();
        
        /// Returns the rendering object
        virtual Renderer *getRenderer();
        
        /// Returns the size of the object
        const Size& getSize();
        
        /// Returns true if this object uses the specified mouse event as the trigger to a callback function
        bool hasMouseEvent(MouseEvent event) const;
        
        /// Returns true if this object has a parent, false if it does not
        bool hasParent() const;
        
        /// Returns true if the object is hidden, false if else
        bool isHidden() const;
        
        /**
         * @brief Assigns a callback function for a mouse event being triggered on this object
         * @param event The mouse event, which when triggered activates the callback function
         * @param callback The function to execute on the mouse event being triggered
         */
        void registerMouseEvent(MouseEvent event, MouseEventCallback&& callback);
        
        /**
         * @brief Assigns a callback function for a mouse event being triggered on this object
         * @param event The mouse event, which when triggered activates the callback function
         * @param shortcut A keyboard shortcut which may also trigger the callback function
         * @param callback The function to execute on the mouse event being triggered
         */
        void registerMouseEvent(MouseEvent event, SDL_Keycode shortcut, MouseEventCallback&& callback);
        
        /// Removes a child object from the child container
        void removeChild(GUIObjectID childID);
        
        /// Removes and destroys all child objects relative to this object
        void removeChildren();

        /// Sets a hint associated with the object, which binds to the Mouse Hover event and displays
        /// a box containing the hint's text next to the object being hovered on by the mouse
        void setHint(std::string text);
        
        /// Sets the calling object's parent to the given object, referenced by its identifier
        void setParent(GUIObjectID parentID);
        
        /// Sets the calling object's parent to the given object
        void setParent(GUIObject *parent);

        /// Changes the position of the current object. draw() must be called to take effect
        void setPosition(Position newPosition, bool verifyBounds = true);

        /// Changes the size of the current object. draw() must be called to take effect
        void setSize(Size newSize, bool verifyBounds = true);
        
        /// Sets the data member m_isHiding to the given value
        void setHidden(bool value);

        /// Sets the children of this object to be hidden if value = true, not hidden if false
        void setChildrenHidden(bool value);

        /// The following two methods are *not* to be called by the user, only public b/c of SDL c-style callbacks

        /// Callback which is activated by a timer after the event is started to show an object hint
        void objectHintTimerCallback();

        /// Called to actually display the hint, after a given time span has passed and object is still active
        void displayHint();

    /// Virtual Functions
    public:
        /// Draw the object on its parent
        virtual void draw() = 0; // note to self: getRenderer()->update() must be called to actually draw the object. Also set hiding to false on draw
        
        /// Deallocates any SDL related resources that were consumed
        virtual void destroy() { }
        
        /// Hides the object from sight
        virtual void hide();
        
    protected:
        /// Called if the object has gained keyboard focus and received text input
        virtual void onTextInput(std::string text) { }
        
        /// Called if the object once had keyboard focus, but then lost it
        virtual void onLostKeyboardFocus() { }
        
    private:
        /// Confirms that the size and position of this object are <= the size and position of its parent
        void confirmSizeAndPos();
        
        /// Confirms that the position of the object is within the area of its parent
        void confirmPosition();
        
        /// Confirms that the size of the object is <= that of its parent
        void confirmSize();
        
        /// Called by the event handler when the requirements are met to invoke a mouse event on this object
        void invokeMouseEvent(MouseEvent event);
    
        /// If a parent object was specified, this adds the calling object to its parent's container of children
        void registerWithParent();
        
    protected:
        /// Attempts to create a new 32 bit, rgba surface that represents the object, which can be used for about anything
        SDL_Surface *createObjectSurface();
    
        /// Returns the size and position of this object in a SDL_Rect structure
        SDL_Rect getObjectRect();
        
        /// Container of all child objects relative to this one
        GUIObjectContainer m_children;
        
        /// Mouse event handler map
        MouseEventMap m_mouseHandler;
        
        /// Parent object
        GUIObject *m_parent;
        
        /// Position of the object
        Position m_position;
        
        /// Size of the object
        Size m_size;
        
        /// True if the object is hiding, false if else
        bool m_isHiding;
        
        /// The object's unique identifier
        GUIObjectID m_objectID;

    private:
        /// Method which is called to display the object's hint
        void showObjectHint();

        /// Method which is called to hide the object's hint
        void hideObjectHint();

        /// Mouse hover / lost focus callbacks, saved if setHint(...) is called and
        /// the previously existing mouse events still need to be called.
        MouseEventCallback m_mouseHoverWithHint, m_mouseLostFocusWithHint;

        /// The size of the object's hint box (if any)
        Size m_hintBoxSize;

        /// The text (if any) that is displayed on an object's hint activation via mouse hover
        std::string m_hintText {};

        /// True if currently showing hint box, false if not
        bool m_isShowingHint;

        /// ID for the timer which is responsible for determining if enough time has passed
        /// between a mouse hovering an object, and hovering long enough to display the hint
        SDL_TimerID m_mouseHoverTimer;
    };
}

