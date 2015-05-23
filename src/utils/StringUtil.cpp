#include "StringUtil.h"
#include <stdarg.h>
#include <algorithm>
#include <functional>

#define FORMAT_BLOCK_SIZE 512 // # of bytes for initial allocation for printf


// hack to check only first byte of UTF-8 character
// without this hack "TrimX" functions failed on Win32 and OS X with UTF-8 strings
static int isspace_c(char c)
{
	return (c & 0x80) == 0 && ::isspace(c);
}

int CStringUtil::Replace(std::string &str, const std::string &oldStr, const std::string &newStr)
{
	if (oldStr.empty())
		return 0;

	int replacedChars = 0;
	size_t index = 0;

	while (index < str.size() && (index = str.find(oldStr, index)) != std::string::npos)
	{
		str.replace(index, oldStr.size(), newStr);
		index += newStr.size();
		replacedChars++;
	}

	return replacedChars;
}

std::string& CStringUtil::TrimRight(std::string &str)
{
	str.erase(std::find_if(str.rbegin(), str.rend(), std::not1(std::ptr_fun(isspace_c))).base(), str.end());
	return str;
}


std::string CStringUtil::FormatV(const char *fmt, va_list args)
{
	if (!fmt || !fmt[0])
		return "";

	int size = FORMAT_BLOCK_SIZE;
	va_list argCopy;

	while (1)
	{
		char *cstr = reinterpret_cast<char*>(malloc(sizeof(char) * size));
		if (!cstr)
			return "";

		va_copy(argCopy, args);
		int nActual = vsnprintf(cstr, size, fmt, argCopy);
		va_end(argCopy);

		if (nActual > -1 && nActual < size) // We got a valid result
		{
			std::string str(cstr, nActual);
			free(cstr);
			return str;
		}
		free(cstr);
#ifndef _WIN32
		if (nActual > -1)                   // Exactly what we will need (glibc 2.1)
			size = nActual + 1;
		else                                // Let's try to double the size (glibc 2.0)
			size *= 2;
#else  // TARGET_WINDOWS
		va_copy(argCopy, args);
		size = _vscprintf(fmt, argCopy);
		va_end(argCopy);
		if (size < 0)
			return "";
		else
			size++; // increment for null-termination
#endif // _WIN32
	}

	return ""; // unreachable
}

std::string CStringUtil::Format(const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	std::string str = FormatV(fmt, args);
	va_end(args);

	return str;
}

// Simple conversion between wide and narrow strings (only safe for ASCII!):

template<typename TIn, typename TOut>
static std::basic_string<TOut> ConvertString(const std::basic_string<TIn>& s)
{
	std::basic_string<TOut> out;
	out.reserve(s.length());

	for (std::basic_string<TIn>::const_iterator i = s.begin(); i != s.end(); ++i)
	{
		out += static_cast<TOut>(*i);
	}

	return out;
}

std::string CStringUtil::WideToAnsi(const std::wstring& s)
{
	return ConvertString<wchar_t, char>(s);
}

std::wstring CStringUtil::AnsiToWide(const std::string& s)
{
	return ConvertString<char, wchar_t>(s);
}