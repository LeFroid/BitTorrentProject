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

namespace gui 
{
    /// Window related events (sub-events of SDL_WINDOWEVENT)
    enum class WindowEvent
    {
        Shown,       // SDL_WINDOWEVENT_SHOWN
        Hidden,      // SDL_WINDOWEVENT_HIDDEN
        Exposed,     // SDL_WINDOWEVENT_EXPOSED      (implies the window should be redrawn)
        Moved,       // SDL_WINDOWEVENT_MOVED
        SizeChanged, // SDL_WINDOWEVENT_SIZE_CHANGED (the window size has changed either by the app or a window manager)
        Resized,     // SDL_WINDOWEVENT_RESIZED      (window has been resized by an external factor, must be handled by us)
        Minimized,   // SDL_WINDOWEVENT_MINIMIZED
        Maximized,   // SDL_WINDOWEVENT_MAXIMIZED
        Restored,    // SDL_WINDOWEVENT_RESTORED     (restored to original size and position)
        Enter,       // SDL_WINDOWEVENT_ENTER        (window has gained mouse focus)
        Leave,       // SDL_WINDOWEVENT_LEAVE        (window has lost mouse focus)
        FocusGained, // SDL_WINDOWEVENT_FOCUS_GAINED (window has gained *keyboard* focus)
        FocusLost,   // SDL_WINDOWEVENT_FOCUS_LOST   (window has lost *keyboard* focus)
        Close        // SDL_WINDOWEVENT_CLOSE        (window manager has requested the window to be closed)
    };
    
    /// Mouse related events (direct events)
    enum class MouseEvent
    {
        Moved,        // SDL_MOUSEMOTION
        PressedLeft,  // SDL_MOUSEBUTTONDOWN         (pressed down, left mouse button)
        PressedRight, // SDL_MOUSEBUTTONDOWN         (pressed down, right mouse button)
        ReleasedLeft, // SDL_MOUSEBUTTONUP           (released after press, left mouse button)
        ReleasedRight,// SDL_MOUSEBUTTONUP           (released after press, right mouse button)
        Wheel,        // SDL_MOUSEWHEEL              (wheel has been moved)
        LostFocus     // Custom)                     (used to inform the object it has lost focus from the mouse)
    };
    
    /// Keyboard related events (direct events)
    enum class KeyboardEvent
    {
        Pressed,     // SDL_KEYDOWN
        Released,    // SDL_KEYUP
        TextEditing, // SDL_TEXTEDITING              (composition editing)
        TextInput,   // SDL_TEXTINPUT
        LostFocus,   // Custom                       (used to inform an object that it no longer is waiting on input from the keyboard)
    };
}
