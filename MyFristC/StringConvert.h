#pragma once
#include <string>
#include <Windows.h>

class StringConvert
{
public:
	StringConvert();
	~StringConvert();
	static std::wstring  StringToWString(const std::string &str);
	static std::string WStringToString(const std::wstring &wstr);
	static std::string UnicodeToUTF8(const std::wstring& wstr);
	static std::wstring UTF8ToUnicode(const std::string& str);
	static std::string ws2s(const std::wstring& ws);
	static std::wstring s2ws(const std::string& s);
private:

};


