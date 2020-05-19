#include "stdafx.h"
#include "GetComputerInfoHelper.h"

GetComputerInfo::GetComputerInfo()
{
}

GetComputerInfo::~GetComputerInfo()
{
}
//获取本地IP
int GetComputerInfo::getLocalIP(std::string & localIP)
{
	try
	{
		localIP.resize(32);
		char *ret;
		char host_name[256] = "";
		WSADATA wsaData;
		WSAStartup(0x101, &wsaData);

		if (!gethostname(host_name, 256))
		{
			ret = inet_ntoa(*((struct in_addr*)gethostbyname(host_name)->h_addr));
			localIP = ret;
		}
		WSACleanup();
		return 0;
	}
	catch (const std::exception)
	{
		WSACleanup();
		return 1;
	}
}

//获取公网IP
int GetComputerInfo::getInternalIP(std::string &internalIP)
{
	internalIP.resize(32);
	TCHAR szTempPath[_MAX_PATH] = { 0 }, szTempFile[MAX_PATH] = { 0 };
	std::string buffer;
	GetTempPath(MAX_PATH, szTempPath);
	UINT nResult = GetTempFileName(szTempPath, _T("~ex"), 0, szTempFile);
	int ret = URLDownloadToFile(NULL, _T("http://api.ipify.org"), szTempFile, BINDF_GETNEWESTVERSION, NULL);
	if (ret == S_FALSE)
		return 0;
	FILE *fp;
	if (_wfopen_s(&fp, szTempFile, _T("rb")) != 0) {
		return 0;
	}
	fseek(fp, 0, SEEK_END);//得到文件大小
	int ilength = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	if (ilength > 0)
	{
		buffer.resize(ilength);
		fread(&buffer[0], sizeof(TCHAR), ilength, fp);
		fclose(fp);
		DeleteFile(_T("ip.ini"));
		internalIP = buffer;
		return 1;
	}
	else
	{
		fclose(fp);
		return 0;
	}
}

//获取MAC地址
int GetComputerInfo::getMac(std::string & mac)
{
	NCB ncb;
	char macChar[100];
	typedef struct _ASTAT_
	{
		ADAPTER_STATUS   adapt;
		NAME_BUFFER   NameBuff[30];
	}ASTAT, *PASTAT;

	ASTAT Adapter;

	typedef struct _LANA_ENUM
	{
		UCHAR   length;
		UCHAR   lana[MAX_LANA];
	}LANA_ENUM;

	LANA_ENUM lana_enum;
	UCHAR uRetCode;
	memset(&ncb, 0, sizeof(ncb));
	memset(&lana_enum, 0, sizeof(lana_enum));
	ncb.ncb_command = NCBENUM;
	ncb.ncb_buffer = (unsigned char *)&lana_enum;
	ncb.ncb_length = sizeof(LANA_ENUM);
	uRetCode = Netbios(&ncb);

	if (uRetCode != NRC_GOODRET)
		return uRetCode;

	for (int lana = 0; lana < lana_enum.length; lana++)
	{
		ncb.ncb_command = NCBRESET;
		ncb.ncb_lana_num = lana_enum.lana[lana];
		uRetCode = Netbios(&ncb);
		if (uRetCode == NRC_GOODRET)
			break;
	}

	if (uRetCode != NRC_GOODRET)
		return uRetCode;

	memset(&ncb, 0, sizeof(ncb));
	ncb.ncb_command = NCBASTAT;
	ncb.ncb_lana_num = lana_enum.lana[0];
	strcpy((char*)ncb.ncb_callname, "*");
	ncb.ncb_buffer = (unsigned char *)&Adapter;
	ncb.ncb_length = sizeof(Adapter);
	uRetCode = Netbios(&ncb);

	if (uRetCode != NRC_GOODRET)
		return uRetCode;

	sprintf(macChar, "%02X%02X%02X%02X%02X%02X",
		Adapter.adapt.adapter_address[0],
		Adapter.adapt.adapter_address[1],
		Adapter.adapt.adapter_address[2],
		Adapter.adapt.adapter_address[3],
		Adapter.adapt.adapter_address[4],
		Adapter.adapt.adapter_address[5]);
	mac = macChar;
	return 0;
}

//获取CPI序列号
int GetComputerInfo::getCPUProcessorID(std::string & cpuID)
{
	try
	{
		std::array<int, 4> cpui;    //#include <array>
									// Calling __cpuid with 0x0 as the function_id argument  
									// gets the number of the highest valid function ID.  什么是function ID？
		__cpuid(cpui.data(), 0x0);    //cpui[0] = "funcition_id的最大值"
		int nIds_ = cpui[0];
		std::vector<std::array<int, 4>> data_;  //保存遍历到的所有cpui的值    
		for (int i = 0; i <= nIds_; ++i)
		{
			__cpuidex(cpui.data(), i, 0);
			data_.push_back(cpui);
		}
		//reinterpret_cast<int*>(vendor) //*reinterpret_cast<int*>(vendor)
		//索引0 0+4 0+8的值构成了CPU芯片的名称
		char vendor[0x20] = { 0 };
		*reinterpret_cast<int*>(vendor) = data_[0][1];
		*reinterpret_cast<int*>(vendor + 4) = data_[0][3];
		*reinterpret_cast<int*>(vendor + 8) = data_[0][2];  // vendor="GenuineIntel"    
		std::string vendor_ = vendor;
		bool isIntel_ = false;
		bool isAMD = false;
		if ("GenuineIntel" == vendor_)
		{
			isIntel_ = true;    //厂商为INTEL
		}
		else if ("AuthenticAMD" == vendor_)
		{
			isAMD = true;       //厂商为AMD
		}
		char vendor_serialnumber[0x14] = { 0 };
		sprintf_s(vendor_serialnumber, sizeof(vendor_serialnumber), "%08X%08X", data_[1][3], data_[1][0]);
		//用“wmic cpu get processorid”获取的结果进行比对，结果应该是一致的。    //vendor_serialnumber = "BFEBFBFF000406E3";    
		cpuID = vendor_serialnumber;
		return 0;
	}
	catch (const std::exception&)
	{
		return 1;
	}
	
}

//获取盘卷标号
BOOL GetComputerInfo::getSysHardVolumeNumber(const std::string root, std::string & hardVolumeNum)
{
	try
	{
		//check Drive is exist
		if (!checkDrive(getDriveStrings(),root)) 
		{
			return false;
		}
		HRESULT hres;
		// Step 1: --------------------------------------------------
		// Initialize COM. ------------------------------------------
		CoUninitialize();
		hres = CoInitializeEx(0, COINIT_MULTITHREADED);
		if (FAILED(hres))
		{
			return false;                  // Program has failed.
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

			CoUninitialize();
			return false;                    // Program has failed.
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

			CoUninitialize();
			return false;                 // Program has failed.
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

			pLoc->Release();
			CoUninitialize();
			return false;                // Program has failed.
		}

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
			pSvc->Release();
			pLoc->Release();
			CoUninitialize();
			return false;               // Program has failed.
		}

		// Step 6: --------------------------------------------------
		// Use the IWbemServices pointer to make requests of WMI ----

		// For example, get the name of the operating system

		std::string str = "SELECT * FROM win32_logicaldisk where DeviceID=\"" + root + ":\"";

		IEnumWbemClassObject* pEnumerator = NULL;
		hres = pSvc->ExecQuery(
			bstr_t("WQL"),
			bstr_t("SELECT * FROM Win32_DiskDrive"),//获取C盘卷标号
								//bstr_t("SELECT * FROM win32_Processor"),//获取CPU序列号
			WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
			NULL,
			&pEnumerator);

		if (FAILED(hres))
		{
			pSvc->Release();
			pLoc->Release();
			CoUninitialize();
			return false;               // Program has failed.
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
			hr = pclsObj->Get(L"SerialNumber", 0, &vtProp, 0, 0);//获取C盘卷标号
																	   //hr = pclsObj->Get(L"ProcessorId", 0, &vtProp, 0, 0);//获取CPU序列号
			hardVolumeNum = _com_util::ConvertBSTRToString(vtProp.bstrVal);
			VariantClear(&vtProp);
		}

		// Cleanup
		pSvc->Release();
		pLoc->Release();
		pEnumerator->Release();
		pclsObj->Release();
		CoUninitialize();
		return true;
	}
	catch (const std::exception&)
	{
		return false;
	}
	
}

//获取系统所在盘符
std::string GetComputerInfo::getSysDir()
{
	char str[MAX_PATH];
	GetSystemDirectory((LPWSTR)str, MAX_PATH);
	return str;
}

//获取主机名
int GetComputerInfo::getHostName(std::string & hostName)
{
	try
	{
		hostName.resize(256);
		char host_name[256] = "";
		WSADATA wsaData;
		WSAStartup(0x101, &wsaData);
		gethostname(host_name, 256);
		hostName = host_name;
		WSACleanup();
		return 0;
	}
	catch (const std::exception&)
	{
		return 1;
	}

}
 //字符转换
 std::string GetComputerInfo::TcharToChar(TCHAR * tchar)
{
	std::string sz;
	for (TCHAR * s = tchar; *s; s += _tcslen(s) + 1)
	{
		char * _char = new char[100];
		int iLength;
		//获取字节长度    
		iLength = WideCharToMultiByte(CP_ACP, 0, s, -1, NULL, 0, NULL, NULL);
		//将tchar值赋给_char      
		WideCharToMultiByte(CP_ACP, 0, s, -1, _char, iLength, NULL, NULL);
		std::string t(_char);
		sz.append(t);
		free(_char);
	}
	return sz;
}

 //获取所有的磁盘
 std::string GetComputerInfo::getDriveStrings()
{
	TCHAR szBuf[100];
	memset(szBuf, 0, 100);
	DWORD len = GetLogicalDriveStrings(sizeof(szBuf) / sizeof(TCHAR), szBuf);
	std::string s = TcharToChar(szBuf);
	return s;
}

 //检查磁盘是否存在
 bool GetComputerInfo::checkDrive(std::string drives, std::string userPath)
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
			return false;
		}
		else {
			return true;
		}
	}
	else
	{
		return false;
	}
}
