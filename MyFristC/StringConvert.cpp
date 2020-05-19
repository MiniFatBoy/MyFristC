#include "stdafx.h"
#include "StringConvert.h"

StringConvert::StringConvert()
{
}

StringConvert::~StringConvert()
{
}

//将string转换宽字节
inline std::wstring StringConvert::StringToWString(const std::string & str)
{
	std::wstring wstr;
	int nLen = (int)str.length();
	wstr.resize(nLen, L' ');
	int nResult = MultiByteToWideChar(CP_ACP, 0, (LPCSTR)str.c_str(), nLen, (LPWSTR)wstr.c_str(), nLen);
	if (nResult == 0)
	{
		return std::wstring(L"");
	}
	return wstr;
}

//将宽字节转换string
inline std::string StringConvert::WStringToString(const std::wstring & wstr)
{
	std::string str;
	int nLen = (int)wstr.length();
	str.resize(nLen, ' ');
	int nResult = WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)wstr.c_str(), nLen, (LPSTR)str.c_str(), nLen, NULL, NULL);
	if (nResult == 0)
	{
		return std::string("");
	}
	return str;
}

//将宽字节转为UTF8
inline std::string StringConvert::UnicodeToUTF8(const std::wstring & wstr)
{
	char*     pElementText;
	int    iTextLen;
	// wide char to multi char
	iTextLen = WideCharToMultiByte(CP_UTF8,
		0,
		wstr.c_str(),
		-1,
		NULL,
		0,
		NULL,
		NULL);
	pElementText = new char[iTextLen + 1];
	memset((void*)pElementText, 0, sizeof(char) * (iTextLen + 1));
	::WideCharToMultiByte(CP_UTF8,
		0,
		wstr.c_str(),
		-1,
		pElementText,
		iTextLen,
		NULL,
		NULL);
	std::string str;
	str = pElementText;
	delete[] pElementText;
	return str;
}

//将utf8转为宽字节
inline std::wstring StringConvert::UTF8ToUnicode(const std::string & str)
{
	int  len = 0;
	len = str.length();
	int  unicodeLen = ::MultiByteToWideChar(CP_UTF8,
		0,
		str.c_str(),
		-1,
		NULL,
		0);
	wchar_t *  pUnicode;
	pUnicode = new  wchar_t[unicodeLen + 1];
	memset(pUnicode, 0, (unicodeLen + 1) * sizeof(wchar_t));
	::MultiByteToWideChar(CP_UTF8,
		0,
		str.c_str(),
		-1,
		(LPWSTR)pUnicode,
		unicodeLen);
	std::wstring  wstr;
	wstr = (wchar_t*)pUnicode;
	delete  pUnicode;
	return  wstr;
}

inline std::string StringConvert::ws2s(const std::wstring & ws)
{
	std::string curLocale = setlocale(LC_ALL, NULL); // curLocale = "C";
	setlocale(LC_ALL, "chs");
	const wchar_t* _Source = ws.c_str();
	size_t _Dsize = 2 * ws.size() + 1;
	char *_Dest = new char[_Dsize];
	memset(_Dest, 0, _Dsize);
	//	wcstombs(_Dest,_Source,_Dsize);
	size_t   i;
	wcstombs_s(&i, _Dest, _Dsize, _Source, _Dsize);
	std::string result = _Dest;
	delete[]_Dest;
	setlocale(LC_ALL, curLocale.c_str());
	return result;
}

inline std::wstring StringConvert::s2ws(const std::string & s)
{
	setlocale(LC_ALL, "chs");
	const char* _Source = s.c_str();
	size_t _Dsize = s.size() + 1;
	wchar_t *_Dest = new wchar_t[_Dsize];
	wmemset(_Dest, 0, _Dsize);
	//mbstowcs(_Dest,_Source,_Dsize);
	size_t   i;
	mbstowcs_s(&i, _Dest, _Dsize, _Source, _Dsize);
	std::wstring result = _Dest;
	delete[]_Dest;
	setlocale(LC_ALL, "C");
	return result;
}
