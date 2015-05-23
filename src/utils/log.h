#pragma once


#define LOG_LEVEL_NONE         -1 // nothing at all is logged
#define LOG_LEVEL_NORMAL        0 // shows notice, error, severe and fatal
#define LOG_LEVEL_DEBUG         1 // shows all
#define LOG_LEVEL_DEBUG_FREEMEM 2 // shows all + shows freemem on screen
#define LOG_LEVEL_MAX           LOG_LEVEL_DEBUG_FREEMEM

// ones we use in the code
#define LOGDEBUG   0
#define LOGINFO    1
#define LOGNOTICE  2
#define LOGWARNING 3
#define LOGERROR   4
#define LOGSEVERE  5
#define LOGFATAL   6
#define LOGNONE    7

// extra masks - from bit 5
#define LOGMASKBIT  5
#define LOGMASK     ((1 << LOGMASKBIT) - 1)

#define LOGSAMBA    (1 << (LOGMASKBIT + 0))
#define LOGCURL     (1 << (LOGMASKBIT + 1))
#define LOGCMYTH    (1 << (LOGMASKBIT + 2))
#define LOGFFMPEG   (1 << (LOGMASKBIT + 3))
#define LOGRTMP     (1 << (LOGMASKBIT + 4))
#define LOGDBUS     (1 << (LOGMASKBIT + 5))
#define LOGJSONRPC  (1 << (LOGMASKBIT + 6))
#define LOGAUDIO    (1 << (LOGMASKBIT + 7))
#define LOGAIRTUNES (1 << (LOGMASKBIT + 8))
#define LOGUPNP     (1 << (LOGMASKBIT + 9))
#define LOGCEC      (1 << (LOGMASKBIT + 10))
#define LOGVIDEO    (1 << (LOGMASKBIT + 11))

#include <string>

#include "utils/win32/Win32InterfaceForCLog.h"
typedef class CWin32InterfaceForCLog PlatformInterfaceForCLog;

#include "utils/params_check_macros.h"

#include <mutex> 

class CLog
{
public:
  CLog();
  ~CLog(void);
  static void Close();
  static void Log(int loglevel, PRINTF_FORMAT_STRING const char *format, ...) PARAM2_PRINTF_FORMAT;
  static void LogFunction(int loglevel, IN_OPT_STRING const char* functionName, PRINTF_FORMAT_STRING const char* format, ...) PARAM3_PRINTF_FORMAT;
#define LogF(loglevel,format,...) LogFunction((loglevel),__FUNCTION__,(format),##__VA_ARGS__)
  static void MemDump(char *pData, int length);
	static bool Init(const std::string& path, const std::string& FileName);
  static void PrintDebugString(const std::string& line); // universal interface for printing debug strings
  static void SetLogLevel(int level);
  static int  GetLogLevel();
  static void SetExtraLogLevels(int level);
  static bool IsLogLevelLogged(int loglevel);

protected:

  static void LogString(int logLevel, const std::string& logString);
  static bool WriteLogString(int logLevel, const std::string& logString);


private:


private:
	static std::recursive_mutex m_reMutex;
	static PlatformInterfaceForCLog m_platform;
	static int         m_repeatCount;
	static int         m_repeatLogLevel;
	static std::string m_repeatLine;
	static int         m_logLevel;
	static int         m_extraLogLevels;
};