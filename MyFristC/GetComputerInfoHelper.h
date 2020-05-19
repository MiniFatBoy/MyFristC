#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#pragma comment(lib, "urlmon.lib")
#pragma comment(lib, "Iphlpapi.lib")
#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib, "netapi32.lib")
#pragma comment(lib, "comsuppw.lib")
#pragma comment(lib, "wbemuuid.lib")

#/*include <string>*/
#include <WinSock2.h>
#include <tchar.h>
#include <WS2tcpip.h>
#include <WinInet.h>
#include <HttpExt.h>
#include <windef.h>
#include <Iphlpapi.h>
#include <wincon.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <Nb30.h>
#include <cstring>
#include <vector>
#include <intrin.h>
#include <array>
#include <comutil.h>
#include <Wbemidl.h>


class GetComputerInfo
{
public:
	 GetComputerInfo();
	~ GetComputerInfo();
	static int getHostName(std::string &hostName); //��ȡ������
	static int getLocalIP(std::string &localIP); //��ȡ����IP
	static int getInternalIP(std::string &internalIP); //��ȡ����IP
	static int getMac(std::string &mac); //��ȡMAC��ַ
	static int getCPUProcessorID(std::string &cpuID); //��ȡCPU	ID
	static BOOL getSysHardVolumeNumber(const std::string root,std::string &hardVolumeNum); //��ȡc�̾���
	static std::string getSysDir(); //��ȡϵͳ�̷�
	static std::string TcharToChar(TCHAR * tchar);
	static std::string getDriveStrings();
	static bool checkDrive(std::string drives, std::string userPath);
private:

};


 