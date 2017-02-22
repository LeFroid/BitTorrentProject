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

#include "Encoder.h"

#include "BenDictionary.h"
#include "BenInt.h"
#include "BenList.h"
#include "BenString.h"

namespace bencoding
{
    void Encoder::clear()
    {
        m_data.clear();
    }

    const std::string &Encoder::getData() const
    {
        return m_data;
    }

    void Encoder::visit(BenDictionary &benObject)
    {
        BenString key;
        m_data.push_back('d');
        for (auto it = benObject.begin(); it != benObject.end(); ++it)
        {
            key.setValue(it->first);
            visit(key);
            it->second->accept(*this);
        }
        m_data.push_back('e');
    }

    void Encoder::visit(BenInt &benObject)
    {
        m_data += 'i' + std::to_string(benObject.getValue()) + 'e';
    }

    void Encoder::visit(BenList &benObject)
    {
        m_data.push_back('l');
        for (auto it = benObject.begin(); it != benObject.end(); ++it)
            (*it)->accept(*this);
        m_data.push_back('e');
    }

    void Encoder::visit(BenString &benObject)
    {
        const std::string strVal = benObject.getValue();
        m_data += std::to_string(strVal.size()) + ':' + strVal;
    }
}
