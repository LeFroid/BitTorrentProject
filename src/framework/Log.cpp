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

#include "Log.h"

using namespace std;

Log::Log(const string& logFile) : 
    m_logLevel(4)
{
    m_logFile.open(logFile.c_str());
}

Log::Log() : 
    m_logLevel(4)
{
}

Log::~Log()
{
    CloseFile();
}

void Log::OpenFile(const string& logFile)
{
    CloseFile();
    m_logFile.open(logFile.c_str());
}

void Log::CloseFile()
{
    if (m_logFile.is_open())
        m_logFile.close();
}

string Log::GetTimestamp()
{
    ostringstream timeStr;
    char timeBuffer[10];
    
    time(&m_rawTime);
    struct tm timeInfo;
    localtime_r(&m_rawTime, &timeInfo);
    
    strftime(timeBuffer, 10, "%R", &timeInfo);  // %R = 24 hour time
    timeStr << "[" << timeInfo.tm_mon + 1 << "/" << timeInfo.tm_mday << "/" << timeInfo.tm_year % 100 << ", " << timeBuffer << "] "; // [{Month}/{Day}/{Year} HH:MM]
    
    return timeStr.str();
}

