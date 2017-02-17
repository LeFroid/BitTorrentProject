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

#include <fstream>
#include <iostream>
#include <mutex>
#include <string>
#include <ctime>
#include <sstream>

enum LogLevel
{
    LOG_FATAL   = 0,    // Used if the application crashed for some reason
    LOG_INFO    = 1,    // Standard log level
    LOG_WARNING = 2,    // Used when something non-critical went wrong
    LOG_ERROR   = 3,    // Used when something serious occurred
    LOG_DEBUG   = 4,    // Used for development purposes
};

/**
 * @class Log
 * 
 * A logging class that prints messages out to the console
 * and also a specified text file as needed
 */
class Log
{
public:
    /// Constructs the log object and opens the specified file
    Log(const std::string& logFile);
    
    /// Constructs the log object without opening any log files
    Log();
    
    /// Closes the log file, if one was opened
    ~Log();
    
    /// Opens a file for logging
    void OpenFile(const std::string& logFile);
    
    /// Closes a log file
    void CloseFile();
    
    /// Sets the log level. See enum `LogLevel`
    void SetLogLevel(int logLevel)
    {
        m_logLevel = logLevel;
    }
    
    /// Prints a formatted string to the console and log file
    template <typename Text, typename... Extra>
    void OutString(Text msg, Extra... extras)
    {
        std::cout << msg;
        if (m_logFile.is_open())
            m_logFile << msg;
       
        OutString(extras...);
    }
    
    /// Base function for template recursion
    void OutString()
    {
        std::cout << std::endl;
        
        if (m_logFile.is_open())
        {
            m_logFile << "\n";
            m_logFile.flush();
        }
    }
    
    /// Prints out the given string at the info level.
    template <typename Text, typename... Extra>
    void OutInfo(Text msg, Extra... extras)
    {
        if (m_logLevel < LOG_INFO)
            return;
        
        std::string currentTime = GetTimestamp();
        
        m_mutex.lock();
        
        std::cout << currentTime;
        
        if (m_logFile.is_open())
            m_logFile << currentTime;
        
        OutString(msg, extras...);
        
        m_mutex.unlock();
    }
    
    /// Prints out the given string at the warning level.
    template <typename Text, typename... Extra>
    void OutWarning(Text msg, Extra... extras)
    {
        if (m_logLevel < LOG_WARNING)
            return;
            
        std::string currentTime = GetTimestamp();
        
        m_mutex.lock();
        
        std::cout << currentTime << "[Warning]: ";
        
        if (m_logFile.is_open())
            m_logFile << currentTime << "[Warning]: ";
        
        OutString(msg, extras...);
    
        m_mutex.unlock();
    }
    
    /// Prints out the given string at the error level.
    template <typename Text, typename... Extra>
    void OutError(Text msg, Extra... extras)
    {
        if (m_logLevel < LOG_ERROR)
            return;
        
        std::string currentTime = GetTimestamp();
        
        m_mutex.lock();
        
        std::cout << currentTime << "[Error]: ";
       
        if (m_logFile.is_open())
            m_logFile << currentTime << "[Error]: ";
        
        OutString(msg, extras...);
        
        m_mutex.unlock();
    }
    
    /// Prints out the given string at the debugging level.
    template <typename Text, typename... Extra>
    void OutDebug(Text msg, Extra... extras)
    {
        if (m_logLevel < LOG_DEBUG)
            return;
        
        std::string currentTime = GetTimestamp();
        
        m_mutex.lock();
       
        std::cout << currentTime << "[Debug]: ";
       
        if (m_logFile.is_open())
            m_logFile << currentTime << "[Debug]: ";
        
        OutString(msg, extras...);
        
        m_mutex.unlock();
    }
    
    /// Prints out the given string at the fatal level.
    template <typename Text, typename... Extra>
    void OutFatal(Text msg, Extra... extras)
    {
        std::string currentTime = GetTimestamp();
        
        m_mutex.lock();
        
        std::cout << currentTime << "[FATAL]: ";
        
        if (m_logFile.is_open())
            m_logFile << currentTime << "[FATAL]: ";
        
        OutString(msg, extras...);
        
        m_mutex.unlock();
    }
    
protected:
    /// Returns the current time as a formatted string
    std::string GetTimestamp();
    
private:
    /// The log level. Anything of this level or lower is reported
    int      m_logLevel;
    
    /// The log file, as specified upon class construction
    std::ofstream m_logFile;
    
    /// Used for timestamps
    time_t   m_rawTime;
    
    /// Used such that the console and log file are not corrupted
    std::mutex m_mutex;
};

