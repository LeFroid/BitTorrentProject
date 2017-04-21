/*
Copyright (c) 2017, Timothy Vaccarelli
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
    class Box;

    class ProgressBar : public GUIObject
    {
    public:
        /// ProgressBar constructor - input is parent (TorrentCell), position, size, background color and
        /// progression (completed) color
        ProgressBar(GUIObject *parent, Position position, Size size, Color backgroundColor, Color progressColor);

        /**
         * @brief Updates the ratio of complete-ness of the progress bar
         * @param partComplete A floating-point integer in the range [0.0, 1.0] representing the percent completion
         */
        void setProgress(float partComplete);

    public:
        /// Draws the progress bar onto its parent
        void draw() override;

    private:
        /// Initializes the child objects of the screen
        void initialize(const Color &bgColor, const Color &progressColor);

    private:
        /// Colored box that is used to show progression of a download
        Box *m_boxProgress;

        /// Colored box that is used to show the amount of progress that is yet to be made
        Box *m_boxIncomplete;
    };
}
