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

#include "Box.h"
#include "ProgressBar.h"

#include "ObjectManager.h"

namespace gui
{
    ProgressBar::ProgressBar(GUIObject *parent, Position position, Size size, Color backgroundColor, Color progressColor) :
        GUIObject(parent, position, size),
        m_boxProgress(nullptr),
        m_boxIncomplete(nullptr)
    {
        initialize(backgroundColor, progressColor);
    }

    void ProgressBar::setProgress(float partComplete)
    {
        // Check bounds
        if (partComplete < 0.0f || partComplete > 100.0f)
            return;

        const Position &parentPos = getPosition();
        const Size &totalSize = getSize();

        Size progressSize((int)ceil(partComplete * totalSize.width), totalSize.height);
        m_boxProgress->setSize(progressSize, false);

        m_boxIncomplete->setPosition(Position(parentPos.x + progressSize.width, parentPos.y), false);
        m_boxIncomplete->setSize(Size(totalSize.width - progressSize.width, totalSize.height));
    }

    void ProgressBar::draw()
    {
        m_boxIncomplete->draw();
        m_boxProgress->draw();

        setHidden(false);
    }

    void ProgressBar::initialize(const Color &bgColor, const Color &progressColor)
    {
        const Position &parentPos = getPosition();
        const Size &parentSize = getSize();

        ObjectManager *objMgr = ObjectManager::getInstance();

        m_boxIncomplete = objMgr->createObject<Box>(this, parentPos, Size(parentSize), bgColor);
        m_boxProgress = objMgr->createObject<Box>(this, parentPos, Size(1, parentSize.height), progressColor);
    }
}
