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

#include <cstdlib>
#include <string>

/**
 * This file contains structures that are common to several
 * different aspects of the graphical user interface.
 */
 
namespace gui 
{
    // Object unique identifier type
    typedef uint32_t GUIObjectID;
    
    /// Contains the properties of colors used for the interface
    struct Color
    {
        uint8_t red;
        uint8_t green;
        uint8_t blue;
        uint8_t alpha;
        
        /// Default color constructor
        Color() : red(0), green(0), blue(0), alpha(0) {}
        
        /// Constructs a color with the given red, green, blue and alpha values
        Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a) :
           red(r), green(g), blue(b), alpha(a) {}
           
        /// Constructs a color, given a hex code of the format "0x000000"
        Color(std::string hexCode)
        {
            unsigned long tmp = std::strtoul(hexCode.c_str(), 0, 16);
            red     = (tmp >> 16) & 0xFF;
            green   = (tmp >> 8) & 0xFF;
            blue    = tmp & 0xFF;
            alpha   = 255;
        }

        /// Returns true if the two colors are equal
        bool operator==(const Color& other) const
        {
            return (red == other.red && green == other.green && blue == other.blue && alpha == other.alpha);
        }
    };
     
    /// Used to represent the position of an object in the GUI
    struct Position
    {
        int x;  // Originating x-coordinate
        int y;  // Originating y-coordinate
        
        Position() : x(0), y(0) {}
        
        Position(int _x, int _y) : x(_x), y(_y) {}
    };
    
    /// Used to represent the size of an object
    struct Size
    {
        int width;   // The width, in pixels, of an object
        int height;  // The height, in pixels, of an object
        
        Size() : width(0), height(0) {}
        
        Size(int _width, int _height) : width(_width), height(_height) {}
        
        bool operator==(const Size& other) const
        {
            return (width == other.width && height == other.height);
        }
    };
 }
 
