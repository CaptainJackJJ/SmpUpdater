/*
*      Copyright (C) 2014 Team XBMC
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

#ifndef _WIN32
#error This file is for win32 platforms only
#endif //!_WIN32

#include "Win32InterfaceForCLog.h"
#include "utils/auto_buffer.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN 1
#endif // WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include "utils/StringUtil.h"

CWin32InterfaceForCLog::CWin32InterfaceForCLog() :
  m_hFile(INVALID_HANDLE_VALUE)
{ }

CWin32InterfaceForCLog::~CWin32InterfaceForCLog()
{
  if (m_hFile != INVALID_HANDLE_VALUE)
    CloseHandle(m_hFile);
}

bool CWin32InterfaceForCLog::OpenLogFile(const std::wstring& logFilePath, int nLogLimitedSize)
{
  if (m_hFile != INVALID_HANDLE_VALUE)
    return false; // file was already opened

  DWORD dwCreationDisposition = OPEN_ALWAYS;
  struct __stat64 info;
  memset(&info, 0, sizeof(__stat64));

  if (_wstat64(logFilePath.c_str(), &info) == 0)
  {
	//file size larger than nLogLimitedSize,creat a new file
	if (info.st_size > nLogLimitedSize)
	{
		dwCreationDisposition = TRUNCATE_EXISTING;
	}
  }

  m_hFile = CreateFileW(logFilePath.c_str(), GENERIC_WRITE, FILE_SHARE_READ, NULL,
	  dwCreationDisposition, FILE_ATTRIBUTE_NORMAL, NULL);
  if (m_hFile == INVALID_HANDLE_VALUE)
	  return false;

  //set file pointer to the file end
  if (OPEN_ALWAYS == dwCreationDisposition)
  {
	  SetFilePointer(m_hFile, info.st_size, 0, FILE_BEGIN);
  }

  if (TRUNCATE_EXISTING == dwCreationDisposition || 0 == info.st_size)
  {
	  static const unsigned char BOM[3] = { 0xEF, 0xBB, 0xBF };
	  DWORD written;
	  (void)WriteFile(m_hFile, BOM, sizeof(BOM), &written, NULL); // write BOM, ignore possible errors
  }

  return true;
}

void CWin32InterfaceForCLog::CloseLogFile(void)
{
  if (m_hFile != INVALID_HANDLE_VALUE)
  {
    CloseHandle(m_hFile);
    m_hFile = INVALID_HANDLE_VALUE;
  }
}

bool CWin32InterfaceForCLog::WriteStringToLog(const std::string& logString)
{
  if (m_hFile == INVALID_HANDLE_VALUE)
    return false;

  std::string strData(logString);
	CStringUtil::Replace(strData, "\n", "\r\n");
  strData += "\r\n";

  DWORD written;
  const bool ret = (WriteFile(m_hFile, strData.c_str(), strData.length(), &written, NULL) != 0) && written == strData.length();
  (void)FlushFileBuffers(m_hFile);

  return ret;
}

void CWin32InterfaceForCLog::PrintDebugString(const std::string& debugString)
{
#ifdef _DEBUG
  ::OutputDebugStringW(L"Debug Print: ");
  int bufSize = MultiByteToWideChar(CP_UTF8, 0, debugString.c_str(), debugString.length(), NULL, 0);
  XUTILS::auto_buffer buf(sizeof(wchar_t) * (bufSize + 1)); // '+1' for extra safety
  if (MultiByteToWideChar(CP_UTF8, 0, debugString.c_str(), debugString.length(), (wchar_t*)buf.get(), buf.size() / sizeof(wchar_t)) == bufSize)
    ::OutputDebugStringW(std::wstring((wchar_t*)buf.get(), bufSize).c_str());
  else
    ::OutputDebugStringA(debugString.c_str());
  ::OutputDebugStringW(L"\n");
#endif // _DEBUG
}

void CWin32InterfaceForCLog::GetCurrentLocalTime(int& hour, int& minute, int& second)
{
  SYSTEMTIME time;
  GetLocalTime(&time);
  hour = time.wHour;
  minute = time.wMinute;
  second = time.wSecond;
}
