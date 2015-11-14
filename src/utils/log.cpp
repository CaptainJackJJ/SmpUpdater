/*
 *      Copyright (C) 2005-2014 Team XBMC
 *      http://xbmc.org
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with XBMC; see the file COPYING.  If not, see
 *  <http://www.gnu.org/licenses/>.
 *
 */

#include "log.h"

#include <inttypes.h>
#include <windows.h>
#include "StringUtil.h"
#include <assert.h>

static const char* const levelNames[] =
{"DEBUG", "INFO", "NOTICE", "WARNING", "ERROR", "SEVERE", "FATAL", "NONE"};

// add 1 to level number to get index of name
static const char* const logLevelNames[] =
{ "LOG_LEVEL_NONE" /*-1*/, "LOG_LEVEL_NORMAL" /*0*/, "LOG_LEVEL_DEBUG" /*1*/, "LOG_LEVEL_DEBUG_FREEMEM" /*2*/ };

std::recursive_mutex CLog::m_reMutex;
PlatformInterfaceForCLog CLog::m_platform;
int         CLog::m_repeatCount;
int         CLog::m_repeatLogLevel;
std::string CLog::m_repeatLine;
int         CLog::m_logLevel;
int         CLog::m_extraLogLevels;
int         CLog::m_logLimitedSize;

CLog::CLog()
{}

CLog::~CLog()
{}

void CLog::Close()
{
	m_reMutex.lock();
  m_platform.CloseLogFile();
  m_repeatLine.clear();
	m_reMutex.unlock();
}


void CLog::Log(int loglevel, const char *format, ...)
{
  if (IsLogLevelLogged(loglevel))
  {
    va_list va;
    va_start(va, format);
		LogString(loglevel, CStringUtil::FormatV(format, va));
    va_end(va);

	if (loglevel == LOGERROR)
	  assert(0);
  }
}

void CLog::LogFunction(int loglevel, const char* functionName, const char* format, ...)
{
  if (IsLogLevelLogged(loglevel))
  {
    std::string fNameStr;
    if (functionName && functionName[0])
      fNameStr.assign(functionName).append(": ");
    va_list va;
    va_start(va, format);
		LogString(loglevel, fNameStr + CStringUtil::FormatV(format, va));
    va_end(va);
  }
}


void CLog::LogString(int logLevel, const std::string& logString)
{
	m_reMutex.lock();
  std::string strData(logString);
	CStringUtil::TrimRight(strData);
  if (!strData.empty())
  {
    if (m_repeatLogLevel == logLevel && m_repeatLine == strData)
    {
      m_repeatCount++;
      return;
    }
    else if (m_repeatCount)
    {
			std::string strData2 = CStringUtil::Format("Previous line repeats %d times.",
                                                m_repeatCount);
      PrintDebugString(strData2);
      WriteLogString(m_repeatLogLevel, strData2);
      m_repeatCount = 0;
    }
    
    m_repeatLine = strData;
    m_repeatLogLevel = logLevel;

    PrintDebugString(strData);

    WriteLogString(logLevel, strData);
  }
	m_reMutex.unlock();
}

bool CLog::Init(const std::wstring& dir, const std::wstring& FileName)
{
	m_reMutex.lock();

	m_logLimitedSize = LOG_LIMITED_SIZE;

	m_repeatCount = 0;
	m_repeatLogLevel = -1;
	m_logLevel = LOG_LEVEL_DEBUG;
	m_extraLogLevels = 0;

  // the log folder location is initialized in the CAdvancedSettings
  // constructor and changed in CApplication::Create()


	bool bOpen = m_platform.OpenLogFile(dir + FileName + L".log", m_logLimitedSize);

	if (bOpen)
	{
		SYSTEMTIME time;
		GetLocalTime(&time);

		CLog::Log(LOGNOTICE, "=================================================================");
		CLog::Log(LOGNOTICE, "===================================================================");
		CLog::Log(LOGNOTICE, "============== %d-%02d-%02d, %02.2d:%02.2d, log service starting ==============",
			time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute);

	}
	else
	{
		PrintDebugString("Plcore::CLog init fail");
	}

	m_reMutex.unlock();
	return bOpen;
}

void CLog::MemDump(char *pData, int length)
{
  Log(LOGDEBUG, "MEM_DUMP: Dumping from %p", pData);
  for (int i = 0; i < length; i+=16)
  {
		std::string strLine = CStringUtil::Format("MEM_DUMP: %04x ", i);
    char *alpha = pData;
    for (int k=0; k < 4 && i + 4*k < length; k++)
    {
      for (int j=0; j < 4 && i + 4*k + j < length; j++)
      {
				std::string strFormat = CStringUtil::Format(" %02x", (unsigned char)*pData++);
        strLine += strFormat;
      }
      strLine += " ";
    }
    // pad with spaces
    while (strLine.size() < 13*4 + 16)
      strLine += " ";
    for (int j=0; j < 16 && i + j < length; j++)
    {
      if (*alpha > 31)
        strLine += *alpha;
      else
        strLine += '.';
      alpha++;
    }
    Log(LOGDEBUG, "%s", strLine.c_str());
  }
}

void CLog::SetLogLevel(int level)
{
	m_reMutex.lock();
  if (level >= LOG_LEVEL_NONE && level <= LOG_LEVEL_MAX)
  {
    m_logLevel = level;
    CLog::Log(LOGNOTICE, "Log level changed to \"%s\"", logLevelNames[m_logLevel + 1]);
  }
  else
    CLog::Log(LOGERROR, "%s: Invalid log level requested: %d", __FUNCTION__, level);
	m_reMutex.unlock();
}

int CLog::GetLogLevel()
{
  return m_logLevel;
}

void CLog::SetExtraLogLevels(int level)
{
	m_reMutex.lock();
  m_extraLogLevels = level;
	m_reMutex.unlock();
}

bool CLog::IsLogLevelLogged(int loglevel)
{
  const int extras = (loglevel & ~LOGMASK);
  if (extras != 0 && (m_extraLogLevels & extras) == 0)
    return false;

#if defined(_DEBUG) || defined(PROFILE)
  return true;
#else
  if (m_logLevel >= LOG_LEVEL_DEBUG)
    return true;
  if (m_logLevel <= LOG_LEVEL_NONE)
    return false;

  // "m_logLevel" is "LOG_LEVEL_NORMAL"
  return (loglevel & LOGMASK) >= LOGNOTICE;
#endif
}


void CLog::PrintDebugString(const std::string& line)
{
#if defined(_DEBUG) || defined(PROFILE)
  m_platform.PrintDebugString(line);
#endif // defined(_DEBUG) || defined(PROFILE)
}



bool CLog::WriteLogString(int logLevel, const std::string& logString)
{
	static const char* prefixFormat = "%02.2d:%02.2d:%02.2d T:%" PRIu64 " %7s: ";

  std::string strData(logString);
  /* fixup newline alignment, number of spaces should equal prefix length */
	CStringUtil::Replace(strData, "\n", "\n                                            ");

  int hour, minute, second;
  m_platform.GetCurrentLocalTime(hour, minute, second);
  
	strData = CStringUtil::Format(prefixFormat,
                                  hour,
                                  minute,
                                  second,
																	(uint64_t)::GetCurrentThreadId(),
																	levelNames[logLevel]) + "  " + strData;

  return m_platform.WriteStringToLog(strData);
}
