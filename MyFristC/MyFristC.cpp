// MyFristC.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <regex>
#include <iostream>
#include <set>
#include <commdlg.h>
#include <atlstr.h>
#include <comdef.h>
#include <Wbemidl.h>
#include <comutil.h>
#include "GetComputerInfoHelper.h"
#include "Book.h"
#include "MD5Checksum.h"
#include "md5.h"
#include <Windows.h>
#include <Psapi.h>
#include <TlHelp32.h>
# pragma comment(lib, "wbemuuid.lib")
# pragma comment(lib, "comsuppw.lib")


#define OUTBEGIN cout << //输出头
#define OUTEND << endl // 输出尾

using namespace std;

void hardIDTest(std::string &hardID)
{
	HRESULT hres;

	// Step 1: --------------------------------------------------
	// Initialize COM. ------------------------------------------

	hres = CoInitializeEx(0, COINIT_MULTITHREADED);
	if (FAILED(hres))
	{
		cout << "Failed to initialize COM library. Error code = 0x"
			<< hex << hres << endl;
		return;                  // Program has failed.
	}

	// Step 2: --------------------------------------------------
	// Set general COM security levels --------------------------
	// Note: If you are using Windows 2000, you need to specify -
	// the default authentication credentials for a user by using
	// a SOLE_AUTHENTICATION_LIST structure in the pAuthList ----
	// parameter of CoInitializeSecurity ------------------------

	hres = CoInitializeSecurity(
		NULL,
		-1,                          // COM authentication
		NULL,                        // Authentication services
		NULL,                        // Reserved
		RPC_C_AUTHN_LEVEL_DEFAULT,   // Default authentication 
		RPC_C_IMP_LEVEL_IMPERSONATE, // Default Impersonation  
		NULL,                        // Authentication info
		EOAC_NONE,                   // Additional capabilities 
		NULL                         // Reserved
	);


	if (FAILED(hres))
	{
		cout << "Failed to initialize security. Error code = 0x"
			<< hex << hres << endl;
		CoUninitialize();
		return;                    // Program has failed.
	}

	// Step 3: ---------------------------------------------------
	// Obtain the initial locator to WMI -------------------------

	IWbemLocator *pLoc = NULL;

	hres = CoCreateInstance(
		CLSID_WbemLocator,
		0,
		CLSCTX_INPROC_SERVER,
		IID_IWbemLocator, (LPVOID *)&pLoc);

	if (FAILED(hres))
	{
		cout << "Failed to create IWbemLocator object."
			<< " Err code = 0x"
			<< hex << hres << endl;
		CoUninitialize();
		return;                 // Program has failed.
	}

	// Step 4: -----------------------------------------------------
	// Connect to WMI through the IWbemLocator::ConnectServer method

	IWbemServices *pSvc = NULL;

	// Connect to the root\cimv2 namespace with
	// the current user and obtain pointer pSvc
	// to make IWbemServices calls.
	hres = pLoc->ConnectServer(
		_bstr_t(L"ROOT\\CIMV2"), // Object path of WMI namespace
		NULL,                    // User name. NULL = current user
		NULL,                    // User password. NULL = current
		0,                       // Locale. NULL indicates current
		NULL,                    // Security flags.
		0,                       // Authority (e.g. Kerberos)
		0,                       // Context object 
		&pSvc                    // pointer to IWbemServices proxy
	);

	if (FAILED(hres))
	{
		cout << "Could not connect. Error code = 0x"
			<< hex << hres << endl;
		pLoc->Release();
		CoUninitialize();
		return;                // Program has failed.
	}

	cout << "Connected to ROOT\\CIMV2 WMI namespace" << endl;


	// Step 5: --------------------------------------------------
	// Set security levels on the proxy -------------------------

	hres = CoSetProxyBlanket(
		pSvc,                        // Indicates the proxy to set
		RPC_C_AUTHN_WINNT,           // RPC_C_AUTHN_xxx
		RPC_C_AUTHZ_NONE,            // RPC_C_AUTHZ_xxx
		NULL,                        // Server principal name 
		RPC_C_AUTHN_LEVEL_CALL,      // RPC_C_AUTHN_LEVEL_xxx 
		RPC_C_IMP_LEVEL_IMPERSONATE, // RPC_C_IMP_LEVEL_xxx
		NULL,                        // client identity
		EOAC_NONE                    // proxy capabilities 
	);

	if (FAILED(hres))
	{
		cout << "Could not set proxy blanket. Error code = 0x"
			<< hex << hres << endl;
		pSvc->Release();
		pLoc->Release();
		CoUninitialize();
		return;               // Program has failed.
	}

	// Step 6: --------------------------------------------------
	// Use the IWbemServices pointer to make requests of WMI ----

	// For example, get the name of the operating system
	IEnumWbemClassObject* pEnumerator = NULL;
	hres = pSvc->ExecQuery(
		bstr_t("WQL"),
		bstr_t("SELECT * FROM win32_logicaldisk where DeviceID=\"c:\""),//获取C盘卷标号
																		//bstr_t("SELECT * FROM win32_Processor"),//获取CPU序列号
		WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
		NULL,
		&pEnumerator);

	if (FAILED(hres))
	{
		cout << "Query for operating system name failed."
			<< " Error code = 0x"
			<< hex << hres << endl;
		pSvc->Release();
		pLoc->Release();
		CoUninitialize();
		return;               // Program has failed.
	}

	// Step 7: -------------------------------------------------
	// Get the data from the query in step 6 -------------------

	IWbemClassObject *pclsObj;
	ULONG uReturn = 0;

	while (pEnumerator)
	{
		HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1,
			&pclsObj, &uReturn);

		if (0 == uReturn)
		{
			break;
		}

		VARIANT vtProp;

		// Get the value of the Name property
		hr = pclsObj->Get(L"VolumeSerialNumber", 0, &vtProp, 0, 0);//获取C盘卷标号
																   //hr = pclsObj->Get(L"ProcessorId", 0, &vtProp, 0, 0);//获取CPU序列号
		wcout << " OS Name : " << vtProp.bstrVal << endl;
		hardID = _com_util::ConvertBSTRToString(vtProp.bstrVal);
		VariantClear(&vtProp);
	}

	// Cleanup
	// ========

	pSvc->Release();
	pLoc->Release();
	pEnumerator->Release();
	pclsObj->Release();
	CoUninitialize();

}


string TcharToChar(TCHAR * tchar)
{
	string sz;
	for (TCHAR * s = tchar; *s; s += _tcslen(s) + 1)
	{
		char * _char = new char[100];
		int iLength;
		//获取字节长度    
		iLength = WideCharToMultiByte(CP_ACP, 0, s, -1, NULL, 0, NULL, NULL);
		//将tchar值赋给_char      
		WideCharToMultiByte(CP_ACP, 0, s, -1, _char, iLength, NULL, NULL);
		string t(_char);
		sz.append(t);
		free(_char);
	}
	return sz;
}

string getDriveStrings()
{
	TCHAR szBuf[100];
	memset(szBuf, 0, 100);
	DWORD len = GetLogicalDriveStrings(sizeof(szBuf) / sizeof(TCHAR), szBuf);
	string s = TcharToChar(szBuf);
	return s;
}

bool checkDrive(string drives, string userPath)
{
	int position;
	char driveTag = userPath.at(0);
	if (driveTag > 'a' && driveTag < 'z') {
		driveTag -= 32;
	}
	if ((driveTag > 'A' && driveTag<'Z') || (driveTag>'a' && driveTag < 'z'))
	{
		position = drives.find(driveTag);
		if (position == drives.npos)
		{
			cout << "输入的盘符不存在，请重新输入！" << endl;
			return false;
		}
		else {
			return true;
		}
	}
	else
	{
		cout << "输入盘符非法，请重新输入！" << endl;
		return false;
	}
}

string Lpcwstr2String(LPCWSTR lps) {
	int len = WideCharToMultiByte(CP_ACP, 0, lps, -1, NULL, 0, NULL, NULL);
	if (len <= 0) {
		return "";
	}
	else {
		char *dest = new char[len];
		WideCharToMultiByte(CP_ACP, 0, lps, -1, dest, len, NULL, NULL);
		dest[len - 1] = 0;
		string str(dest);
		delete[] dest;
		return str;
	}
}

//新建一个对话窗口，选择文件
string get_path() {
	OPENFILENAME ofn;
	char szFile[300];

	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = NULL;
	ofn.lpstrFile = (LPWSTR)szFile;
	ofn.lpstrFile[0] = '\0';
	LPTSTR        lpstrCustomFilter;
	DWORD         nMaxCustFilter;
	ofn.nFilterIndex = 1;
	LPTSTR        lpstrFile;
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = L"ALL\0*.*\0Text\0*.TXT\0";
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;

	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	string path_image = "";
	if (GetOpenFileName(&ofn)) {
		path_image = Lpcwstr2String(ofn.lpstrFile);
		return path_image;
	}
	else {
		return "";
	}
}
//获取进程是否运行
bool findProcess(string processName) 
{
	int i = 0;
	PROCESSENTRY32 pe32;
	pe32.dwSize = sizeof(pe32);
	HANDLE hProcessSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessSnap == INVALID_HANDLE_VALUE)
	{
		i += 0;
	}
	BOOL bMore = ::Process32First(hProcessSnap, &pe32);
	while (bMore)
	{
		//printf(" 进程名称：%s \n", pe32.szExeFile);  
		if (stricmp(processName.c_str(),_bstr_t( pe32.szExeFile)) == 0)
		{
			//printf("进程运行中");  
			i += 1;
		}
		bMore = ::Process32Next(hProcessSnap, &pe32);
	}
	if (i>1) {           //大于1，排除自身  
		return true;
	}
	else {
		return false;
	}

}

 
int main(int index)
{

	if (findProcess("QQ.exe"))
	{
		OUTBEGIN "I GET YOU !" OUTEND;
	}
	system("TASKKILL /F /IM QQ.exe");
	string ssss = get_path();
	//char lp[128] = { 0 };
	//GetHDSerial(lp);
	bool baa = checkDrive(getDriveStrings(), "C");
	OUTBEGIN "我的第一个C++程序" OUTEND;
	/*OUTBEGIN lp OUTEND;*/
	CString md5Str = CMD5Checksum::GetMD5("C:\\Users\\Administrator\\Desktop\\工作账户.txt");
	std::string aa = md5file("C:\\工作账户.txt");
	WinExec("wmic CPU get ProcessorID",SW_HIDE);
	string ss, sl, sname ,sbcpu ,sMac ,sCUP,sHard;
	//GetComputerInfo::getHostName(sname);
	//GetComputerInfo::getLocalIP(sl);
	GetComputerInfo::getInternalIP(ss); 
	GetComputerInfo::getMac(sMac);
	GetComputerInfo::getSysHardVolumeNumber(GetComputerInfo::getSysDir(),sHard);
	GetComputerInfo::getCPUProcessorID(sCUP);
	OUTBEGIN sCUP <<"Hard"<<sHard OUTEND;
	OUTBEGIN "主机名："<<sname<<"内网ip"<< sl << "mac："<< sMac <<"外网ip"<<ss OUTEND;
	book b;
	b.setPrice(25.90);
	b.setTitle("水浒传");
	OUTBEGIN "你所购买的书籍为"<<b.getTitle()<<"，价格为"<<b.getPrice() OUTEND;
	OUTBEGIN &b OUTEND;
	set <int> s;
	s.insert(1);
	s.insert(s.begin(), 9);
	s.insert(1);
	s.insert(s.end(), 4);
	s.insert(5);
	s.insert(6);
	s.insert(7);
	s.insert(9);
	s.insert(0);
	if(s.find(0) == s.end())
	{
		OUTBEGIN "No Find" OUTEND;
	}
	else
	{
		OUTBEGIN "Finded" OUTEND;
	}
	getchar();
    return 0;
}

