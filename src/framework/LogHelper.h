/*
Copyright (c) 2014-2017, Timothy Vaccarelli
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

#include <memory>
#include <unordered_map>

#include "Log.h"

/**
 * @class LogHelper
 * @brief Stores a various amount of log instances for
 *        easy access with the log macros 
 */
class LogHelper
{
public:
    /// Default constructor
    LogHelper();
    
    /// LogHelper destructor
    ~LogHelper();
    
    /// Returns a pointer to the log instance that is associated with the parameter logName.
    /// This will create a new instance of the logging class if logName has no associated object
    Log* get(const std::string& logName);

    /// Sets the directory where log files will be stored
    void setLogDir(std::string logDir);
    
private:
    /// Map of systems to log instances
    std::unordered_map< std::string, std::shared_ptr<Log> > m_logMap;

    /// Directory where log files are stored
    std::string m_logDir;
};

extern LogHelper sLog;

// Example of logging macro: LOG_INFO("system.subsystem", "message")

#define LOG_INFO(name, ...)    sLog.get(name)->OutInfo(__VA_ARGS__)
#define LOG_WARNING(name, ...) sLog.get(name)->OutWarning(__VA_ARGS__)
#define LOG_DEBUG(name, ...)   sLog.get(name)->OutDebug(__VA_ARGS__)
#define LOG_ERROR(name, ...)   sLog.get(name)->OutError(__VA_ARGS__)
#define LOG_FATAL(name, ...)   sLog.get(name)->OutFatal(__VA_ARGS__)

