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

#include <boost/optional.hpp>
#include <boost/property_tree/ptree.hpp>
#include <string>

/**
 * @class Configuration
 * @brief Used to read or write configurable settings
 */
class Configuration
{
public:
    /// Default constructor
    Configuration() = default;

    /// Loads the (JSON) file with the given name into the property tree
    void loadFile(const std::string &fileName);

    /// Returns the value associated with the given path/key, or an
    /// uninitialized boost::optional<T> if not found
    template <class T>
    boost::optional<T> getValue(const std::string &path)
    {
        return m_pTree.get_optional<T>(path);
    }

    template <class T>
    void setValue(const std::string &path, const T &value)
    {
        m_pTree.put(path, value);
    }

private:
    /// Property tree containing configuration data
    boost::property_tree::ptree m_pTree;
};
