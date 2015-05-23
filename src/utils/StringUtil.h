#pragma once

#include <string>

class CStringUtil
{
public:
	CStringUtil();
	~CStringUtil();

	static int Replace(std::string &str, const std::string &oldStr, const std::string &newStr);
	static std::string& TrimRight(std::string &str);
	static std::string FormatV(const char *fmt, va_list args);
	static std::string Format(const char *fmt, ...);
	static std::string WideToAnsi(const std::wstring& s);
	static std::wstring AnsiToWide(const std::string& s);
};