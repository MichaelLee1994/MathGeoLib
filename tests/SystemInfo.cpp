#include <string>

#include "SystemInfo.h"

#if defined(LINUX) || defined(__APPLE__)

#include <string>
#include <sstream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

std::string TrimLeft(std::string str)
{
	return str.substr(str.find_first_not_of(" \n\r\t"));
}

std::string TrimRight(std::string str)
{
	str.erase(str.find_last_not_of(" \n\r\t")+1);
	return str;
}

std::string Trim(std::string str)
{
	return TrimLeft(TrimRight(str));
}

// http://stackoverflow.com/questions/646241/c-run-a-system-command-and-get-output
std::string RunProcess(const char *cmd)
{
	FILE *fp = popen(cmd, "r");
	if (!fp)
		return std::string();
    
	std::stringstream ss;
	char str[1035];
	while(fgets(str, sizeof(str)-1, fp))
		ss << str;
    
	pclose(fp);
    
	return TrimRight(ss.str()); // Trim the end of the result to remove \n.
}

#endif

#if defined(WIN32) && !defined(__MINGW32__) && !defined(WIN8RT)

#include <windows.h>
#include <intrin.h>
#include <iphlpapi.h>

#include <tchar.h>
#include <stdio.h>
#include <strsafe.h>

#include <sstream>
#include <iostream>

#pragma comment(lib, "User32.lib")
#pragma comment(lib, "IPHLPAPI.lib")

#define BUFSIZE 256

typedef void (WINAPI *PGNSI)(LPSYSTEM_INFO);
typedef BOOL (WINAPI *PGPI)(DWORD, DWORD, DWORD, DWORD, PDWORD);

BOOL GetOSDisplayString( LPTSTR pszOS)
{
   OSVERSIONINFOEX osvi;
   SYSTEM_INFO si;
   PGNSI pGNSI;
   PGPI pGPI;
   BOOL bOsVersionInfoEx;
   DWORD dwType;

   ZeroMemory(&si, sizeof(SYSTEM_INFO));
   ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));

   osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

   bOsVersionInfoEx = GetVersionEx ((OSVERSIONINFO *) &osvi);
   if (!bOsVersionInfoEx)
      return 1;

   // Call GetNativeSystemInfo if supported or GetSystemInfo otherwise.

   pGNSI = (PGNSI) GetProcAddress(
      GetModuleHandle(TEXT("kernel32.dll")), 
      "GetNativeSystemInfo");
   if(NULL != pGNSI)
      pGNSI(&si);
   else GetSystemInfo(&si);

   if ( VER_PLATFORM_WIN32_NT==osvi.dwPlatformId && 
        osvi.dwMajorVersion > 4 )
   {
      StringCchCopy(pszOS, BUFSIZE, TEXT("Microsoft "));

      // Test for the specific product.

      if ( osvi.dwMajorVersion == 6 )
      {
         if( osvi.dwMinorVersion == 0 )
         {
            if( osvi.wProductType == VER_NT_WORKSTATION )
                StringCchCat(pszOS, BUFSIZE, TEXT("Windows Vista "));
            else StringCchCat(pszOS, BUFSIZE, TEXT("Windows Server 2008 " ));
         }

         if ( osvi.dwMinorVersion == 1 )
         {
            if( osvi.wProductType == VER_NT_WORKSTATION )
                StringCchCat(pszOS, BUFSIZE, TEXT("Windows 7 "));
            else StringCchCat(pszOS, BUFSIZE, TEXT("Windows Server 2008 R2 " ));
         }
         
		 if (osvi.dwMinorVersion == 2)
		 {
            if( osvi.wProductType == VER_NT_WORKSTATION )
                StringCchCat(pszOS, BUFSIZE, TEXT("Windows 8 "));
		 }
         pGPI = (PGPI) GetProcAddress(
            GetModuleHandle(TEXT("kernel32.dll")), 
            "GetProductInfo");

         pGPI( osvi.dwMajorVersion, osvi.dwMinorVersion, 0, 0, &dwType);

         switch( dwType )
         {
            case PRODUCT_ULTIMATE:
               StringCchCat(pszOS, BUFSIZE, TEXT("Ultimate Edition" ));
               break;
            case PRODUCT_HOME_PREMIUM:
               StringCchCat(pszOS, BUFSIZE, TEXT("Home Premium Edition" ));
               break;
            case PRODUCT_HOME_BASIC:
               StringCchCat(pszOS, BUFSIZE, TEXT("Home Basic Edition" ));
               break;
            case PRODUCT_ENTERPRISE:
               StringCchCat(pszOS, BUFSIZE, TEXT("Enterprise Edition" ));
               break;
            case PRODUCT_BUSINESS:
               StringCchCat(pszOS, BUFSIZE, TEXT("Business Edition" ));
               break;
            case PRODUCT_STARTER:
               StringCchCat(pszOS, BUFSIZE, TEXT("Starter Edition" ));
               break;
            case PRODUCT_CLUSTER_SERVER:
               StringCchCat(pszOS, BUFSIZE, TEXT("Cluster Server Edition" ));
               break;
            case PRODUCT_DATACENTER_SERVER:
               StringCchCat(pszOS, BUFSIZE, TEXT("Datacenter Edition" ));
               break;
            case PRODUCT_DATACENTER_SERVER_CORE:
               StringCchCat(pszOS, BUFSIZE, TEXT("Datacenter Edition (core installation)" ));
               break;
            case PRODUCT_ENTERPRISE_SERVER:
               StringCchCat(pszOS, BUFSIZE, TEXT("Enterprise Edition" ));
               break;
            case PRODUCT_ENTERPRISE_SERVER_CORE:
               StringCchCat(pszOS, BUFSIZE, TEXT("Enterprise Edition (core installation)" ));
               break;
            case PRODUCT_ENTERPRISE_SERVER_IA64:
               StringCchCat(pszOS, BUFSIZE, TEXT("Enterprise Edition for Itanium-based Systems" ));
               break;
            case PRODUCT_SMALLBUSINESS_SERVER:
               StringCchCat(pszOS, BUFSIZE, TEXT("Small Business Server" ));
               break;
            case PRODUCT_SMALLBUSINESS_SERVER_PREMIUM:
               StringCchCat(pszOS, BUFSIZE, TEXT("Small Business Server Premium Edition" ));
               break;
            case PRODUCT_STANDARD_SERVER:
               StringCchCat(pszOS, BUFSIZE, TEXT("Standard Edition" ));
               break;
            case PRODUCT_STANDARD_SERVER_CORE:
               StringCchCat(pszOS, BUFSIZE, TEXT("Standard Edition (core installation)" ));
               break;
            case PRODUCT_WEB_SERVER:
               StringCchCat(pszOS, BUFSIZE, TEXT("Web Server Edition" ));
               break;
         }
      }

      if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 2 )
      {
         if( GetSystemMetrics(SM_SERVERR2) )
            StringCchCat(pszOS, BUFSIZE, TEXT( "Windows Server 2003 R2, "));
         else if ( osvi.wSuiteMask==VER_SUITE_STORAGE_SERVER )
            StringCchCat(pszOS, BUFSIZE, TEXT( "Windows Storage Server 2003"));
         else if ( osvi.wSuiteMask==0x00008000 )
//         else if ( osvi.wSuiteMask==VER_SUITE_WH_SERVER )
            StringCchCat(pszOS, BUFSIZE, TEXT( "Windows Home Server"));
         else if( osvi.wProductType == VER_NT_WORKSTATION &&
                  si.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_AMD64)
         {
            StringCchCat(pszOS, BUFSIZE, TEXT( "Windows XP Professional x64 Edition"));
         }
         else StringCchCat(pszOS, BUFSIZE, TEXT("Windows Server 2003, "));

         // Test for the server type.
         if ( osvi.wProductType != VER_NT_WORKSTATION )
         {
            if ( si.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_IA64 )
            {
                if( osvi.wSuiteMask & VER_SUITE_DATACENTER )
                   StringCchCat(pszOS, BUFSIZE, TEXT( "Datacenter Edition for Itanium-based Systems" ));
                else if( osvi.wSuiteMask & VER_SUITE_ENTERPRISE )
                   StringCchCat(pszOS, BUFSIZE, TEXT( "Enterprise Edition for Itanium-based Systems" ));
            }

            else if ( si.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_AMD64 )
            {
                if( osvi.wSuiteMask & VER_SUITE_DATACENTER )
                   StringCchCat(pszOS, BUFSIZE, TEXT( "Datacenter x64 Edition" ));
                else if( osvi.wSuiteMask & VER_SUITE_ENTERPRISE )
                   StringCchCat(pszOS, BUFSIZE, TEXT( "Enterprise x64 Edition" ));
                else StringCchCat(pszOS, BUFSIZE, TEXT( "Standard x64 Edition" ));
            }

            else
            {
                if ( osvi.wSuiteMask & VER_SUITE_COMPUTE_SERVER )
                   StringCchCat(pszOS, BUFSIZE, TEXT( "Compute Cluster Edition" ));
                else if( osvi.wSuiteMask & VER_SUITE_DATACENTER )
                   StringCchCat(pszOS, BUFSIZE, TEXT( "Datacenter Edition" ));
                else if( osvi.wSuiteMask & VER_SUITE_ENTERPRISE )
                   StringCchCat(pszOS, BUFSIZE, TEXT( "Enterprise Edition" ));
                else if ( osvi.wSuiteMask & VER_SUITE_BLADE )
                   StringCchCat(pszOS, BUFSIZE, TEXT( "Web Edition" ));
                else StringCchCat(pszOS, BUFSIZE, TEXT( "Standard Edition" ));
            }
         }
      }

      if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 1 )
      {
         StringCchCat(pszOS, BUFSIZE, TEXT("Windows XP "));
         if( osvi.wSuiteMask & VER_SUITE_PERSONAL )
            StringCchCat(pszOS, BUFSIZE, TEXT( "Home Edition" ));
         else StringCchCat(pszOS, BUFSIZE, TEXT( "Professional" ));
      }

      if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 0 )
      {
         StringCchCat(pszOS, BUFSIZE, TEXT("Windows 2000 "));

         if ( osvi.wProductType == VER_NT_WORKSTATION )
         {
            StringCchCat(pszOS, BUFSIZE, TEXT( "Professional" ));
         }
         else 
         {
            if( osvi.wSuiteMask & VER_SUITE_DATACENTER )
               StringCchCat(pszOS, BUFSIZE, TEXT( "Datacenter Server" ));
            else if( osvi.wSuiteMask & VER_SUITE_ENTERPRISE )
               StringCchCat(pszOS, BUFSIZE, TEXT( "Advanced Server" ));
            else StringCchCat(pszOS, BUFSIZE, TEXT( "Server" ));
         }
      }

       // Include service pack (if any) and build number.

      if( _tcslen(osvi.szCSDVersion) > 0 )
      {
          StringCchCat(pszOS, BUFSIZE, TEXT(" ") );
          StringCchCat(pszOS, BUFSIZE, osvi.szCSDVersion);
      }

      TCHAR buf[80];

      StringCchPrintf( buf, 80, TEXT(" (build %d)"), (int)osvi.dwBuildNumber);
      StringCchCat(pszOS, BUFSIZE, buf);

      if ( osvi.dwMajorVersion >= 6 )
      {
         if ( si.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_AMD64 )
            StringCchCat(pszOS, BUFSIZE, TEXT( ", 64-bit" ));
         else if (si.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_INTEL )
            StringCchCat(pszOS, BUFSIZE, TEXT(", 32-bit"));
      }
      
      return TRUE; 
   }

   else
   {  
      printf( "This sample does not support this version of Windows.\n");
      return FALSE;
   }
}

std::string GetOSDisplayString()
{
    TCHAR szOS[BUFSIZE];

    if (GetOSDisplayString(szOS))
		 return szOS;
	 else
		 return "Unknown OS";
}

unsigned long long GetTotalSystemPhysicalMemory()
{
	MEMORYSTATUSEX statex;
	statex.dwLength = sizeof(statex);
	int ret = GlobalMemoryStatusEx(&statex);
	if (ret == 0)
		return 0;

	return (unsigned long long)statex.ullTotalPhys;
}

std::string GetProcessorBrandName()
{
    int CPUInfo[4] = {-1};

    // Calling __cpuid with 0x80000000 as the InfoType argument
    // gets the number of valid extended IDs.
    __cpuid(CPUInfo, 0x80000000);
    unsigned int nExIds = CPUInfo[0];

    if (nExIds < 0x80000004)
		 return "Unknown";

	 char CPUBrandString[0x40];
    memset(CPUBrandString, 0, sizeof(CPUBrandString));

    // Get the information associated with each extended ID.
    for (unsigned int i = 0x80000002; i <= nExIds && i <= 0x80000004; ++i)
    {
        __cpuid(CPUInfo, i);

        // Interpret CPU brand string and cache information.
        if  (i == 0x80000002)
            memcpy(CPUBrandString, CPUInfo, sizeof(CPUInfo));
        else if  (i == 0x80000003)
            memcpy(CPUBrandString + 16, CPUInfo, sizeof(CPUInfo));
        else if  (i == 0x80000004)
            memcpy(CPUBrandString + 32, CPUInfo, sizeof(CPUInfo));
    }

	 return CPUBrandString;
}

std::string GetProcessorCPUIDString()
{
	int CPUInfo[4] = {-1};

	// __cpuid with an InfoType argument of 0 returns the number of
	// valid Ids in CPUInfo[0] and the CPU identification string in
	// the other three array elements. The CPU identification string is
	// not in linear order. The code below arranges the information 
	// in a human readable form.
	__cpuid(CPUInfo, 0);
//	unsigned nIds = CPUInfo[0];
	char CPUString[0x20] = {};
	*((int*)CPUString) = CPUInfo[1];
	*((int*)(CPUString+4)) = CPUInfo[3];
	*((int*)(CPUString+8)) = CPUInfo[2];

	return CPUString;
}

std::string GetProcessorExtendedCPUIDInfo()
{
    int CPUInfo[4] = {-1};

    // __cpuid with an InfoType argument of 0 returns the number of
    // valid Ids in CPUInfo[0] and the CPU identification string in
    // the other three array elements. The CPU identification string is
    // not in linear order. The code below arranges the information 
    // in a human readable form.
    __cpuid(CPUInfo, 0);
    unsigned nIds = CPUInfo[0];
    char CPUString[0x20];
    memset(CPUString, 0, sizeof(CPUString));
    *((int*)CPUString) = CPUInfo[1];
    *((int*)(CPUString+4)) = CPUInfo[3];
    *((int*)(CPUString+8)) = CPUInfo[2];

	 if (nIds == 0)
		 return CPUString;

    __cpuid(CPUInfo, 1);

	int nSteppingID = CPUInfo[0] & 0xf;
	int nModel = (CPUInfo[0] >> 4) & 0xf;
	int nFamily = (CPUInfo[0] >> 8) & 0xf;
//	int nProcessorType = (CPUInfo[0] >> 12) & 0x3;
	int nExtendedmodel = (CPUInfo[0] >> 16) & 0xf;
	int nExtendedfamily = (CPUInfo[0] >> 20) & 0xff;
//	int nBrandIndex = CPUInfo[1] & 0xff;

	std::stringstream ss;
	ss << CPUString << ", " << "Stepping: " << nSteppingID << ", Model: " << nModel <<
		", Family: " << nFamily << ", Ext.model: " << nExtendedmodel << ", Ext.family: " << nExtendedfamily << ".";

	return ss.str();
}

int GetMaxSimultaneousThreads()
{
    SYSTEM_INFO sysinfo;
    GetSystemInfo(&sysinfo);
    return sysinfo.dwNumberOfProcessors;
}

unsigned long GetCPUSpeedFromRegistry(unsigned long dwCPU)
{
	HKEY hKey;
	DWORD dwSpeed;

	// Get the key name
	TCHAR szKey[256] = {};
	_sntprintf(szKey, sizeof(szKey)/sizeof(TCHAR)-1,
		TEXT("HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\%d\\"), (int)dwCPU);

	// Open the key
	if(RegOpenKeyEx(HKEY_LOCAL_MACHINE,szKey, 0, KEY_QUERY_VALUE, &hKey) != ERROR_SUCCESS)
	{
		return 0;
	}

	// Read the value
	DWORD dwLen = 4;
	if(RegQueryValueEx(hKey, TEXT("~MHz"), NULL, NULL, (LPBYTE)&dwSpeed, &dwLen) != ERROR_SUCCESS)
	{
		RegCloseKey(hKey);
		return 0;
	}

	// Cleanup and return
	RegCloseKey(hKey);
	return dwSpeed;
}
#elif defined(LINUX)

std::string GetOSDisplayString()
{
	return RunProcess("lsb_release -ds") + " " + RunProcess("uname -mrs");
}

std::string FindLine(const std::string &inStr, const char *lineStart)
{
	int lineStartLen = strlen(lineStart);
	size_t idx = inStr.find(lineStart);
	if (idx == std::string::npos)
		return std::string();
	idx += lineStartLen;
	size_t lineEnd = inStr.find("\n", idx);
	if (lineEnd == std::string::npos)
		return inStr.substr(idx);
	else
		return inStr.substr(idx, lineEnd-idx);
}

unsigned long long GetTotalSystemPhysicalMemory()
{
	std::string r = RunProcess("cat /proc/meminfo");
	std::string memTotal = FindLine(r, "MemTotal:");
	int mem = 0;
	int n = sscanf(memTotal.c_str(), "%d", &mem);
	if (n == 1)
		return (unsigned long long)mem * 1024;
	else
		return 0;
}

std::string GetProcessorBrandName()
{
	std::string r = RunProcess("cat /proc/cpuinfo");
	return Trim(FindLine(FindLine(r, "vendor_id"),":"));
}

std::string GetProcessorCPUIDString()
{
	std::string r = RunProcess("cat /proc/cpuinfo");
	return Trim(FindLine(FindLine(r, "model name"),":"));
}

std::string GetProcessorExtendedCPUIDInfo()
{
	std::string r = RunProcess("cat /proc/cpuinfo");
	std::string stepping = Trim(FindLine(FindLine(r, "stepping"),":"));
	std::string model = Trim(FindLine(FindLine(r, "model"),":"));
	std::string family = Trim(FindLine(FindLine(r, "cpu family"),":"));

	std::stringstream ss;
	ss << GetProcessorBrandName() << ", " << "Stepping: " << stepping << ", Model: " << model <<
		", Family: " << family;
	return ss.str();
}

int GetMaxSimultaneousThreads()
{
	std::string r = RunProcess("lscpu");
	r = TrimRight(FindLine(r, "CPU(s):"));
	int numCPUs = 0;
	int n = sscanf(r.c_str(), "%d", &numCPUs);
	return (n == 1) ? numCPUs : 0;
}

unsigned long GetCPUSpeedFromRegistry(unsigned long /*dwCPU*/)
{
	std::string r = RunProcess("lscpu");
	r = TrimRight(FindLine(r, "CPU MHz:"));
	int mhz = 0;
	int n = sscanf(r.c_str(), "%d", &mhz);
	return (n == 1) ? (unsigned long)mhz : 0;
}

#elif defined(EMSCRIPTEN)

#include <emscripten.h>
extern "C" {
	extern const char *browser_info();
}
std::string GetOSDisplayString() { return browser_info(); }
unsigned long long GetTotalSystemPhysicalMemory() { return (unsigned long long)emscripten_run_script_int("TOTAL_MEMORY"); }
std::string GetProcessorBrandName() { return "n/a"; } 
std::string GetProcessorCPUIDString() { return "n/a"; }
std::string GetProcessorExtendedCPUIDInfo() { return "n/a"; }
unsigned long GetCPUSpeedFromRegistry(unsigned long dwCPU) { return 1; }
int GetMaxSimultaneousThreads() { return 1; }

#elif defined(__APPLE__)

std::string GetOSDisplayString()
{
    std::string uname = RunProcess("uname -mrs");
    
    // http://stackoverflow.com/questions/11072804/mac-os-x-10-8-replacement-for-gestalt-for-testing-os-version-at-runtime/11697362#11697362
    std::string systemVer = RunProcess("cat /System/Library/CoreServices/SystemVersion.plist");
    size_t idx = systemVer.find("<key>ProductVersion</key>");
    if (idx == std::string::npos)
        return uname;
    idx = systemVer.find("<string>", idx);
    if (idx == std::string::npos)
        return uname;
    idx += strlen("<string>");
    size_t endIdx = systemVer.find("</string>", idx);
    if (endIdx == std::string::npos)
        return uname;
    std::string marketingVersion = Trim(systemVer.substr(idx, endIdx-idx));

    uname += " Mac OS X " + marketingVersion;
    int majorVer = 0, minorVer = 0;
    int n = sscanf(marketingVersion.c_str(), "%d.%d", &majorVer, &minorVer);
    if (n != 2)
        return uname;
    switch (majorVer * 100 + minorVer)
    {
        case 1001: return uname + " Puma";
        case 1002: return uname + " Jaguar";
        case 1003: return uname + " Panther";
        case 1004: return uname + " Tiger";
        case 1005: return uname + " Leopard";
        case 1006: return uname + " Snow Leopard";
        case 1007: return uname + " Lion";
        case 1008: return uname + " Mountain Lion";
        default: return uname;
    }
}

#include <sys/types.h>
#include <sys/sysctl.h>

std::string sysctl_string(const char *sysctl_name)
{
    char str[128] = {};
    size_t size = sizeof(str)-1;
    sysctlbyname(sysctl_name, str, &size, NULL, 0);
    return str;    
}

int sysctl_int32(const char *sysctl_name)
{
    int32_t val = 0;
    size_t size = sizeof(val);
    sysctlbyname(sysctl_name, &val, &size, NULL, 0);
    return (int)val;
}

int64_t sysctl_int64(const char *sysctl_name)
{
    int64_t val = 0;
    size_t size = sizeof(val);
    sysctlbyname(sysctl_name, &val, &size, NULL, 0);
    return val;
}

unsigned long long GetTotalSystemPhysicalMemory()
{
    return (unsigned long long)sysctl_int64("hw.memsize");
}

std::string GetProcessorBrandName()
{
    return sysctl_string("machdep.cpu.vendor");
}

std::string GetProcessorCPUIDString()
{
    return sysctl_string("machdep.cpu.brand_string");
}

std::string GetProcessorExtendedCPUIDInfo()
{
    char str[1024];
    sprintf(str, "%s, Stepping: %d, Model: %d, Family: %d, Ext.model: %d, Ext.family: %d.", GetProcessorCPUIDString().c_str(), sysctl_int32("machdep.cpu.stepping"), sysctl_int32("machdep.cpu.model"), sysctl_int32("machdep.cpu.family"), sysctl_int32("machdep.cpu.extmodel"), sysctl_int32("machdep.cpu.extfamily"));
    return str;
}

unsigned long GetCPUSpeedFromRegistry(unsigned long /*dwCPU*/)
{
    int64_t freq = sysctl_int64("hw.cpufrequency");
    return (unsigned long)(freq / 1000 / 1000);
}

// Returns the maximum number of threads the CPU can simultaneously accommodate.
// E.g. for dualcore hyperthreaded Intel CPUs, this returns 4.
int GetMaxSimultaneousThreads()
{
    return (int)sysctl_int32("machdep.cpu.thread_count");
}

#else

#warning SystemInfo.cpp not implemented for the current platform!

std::string GetOSDisplayString() { return ""; }
unsigned long long GetTotalSystemPhysicalMemory() { return 0; }
std::string GetProcessorBrandName() { return ""; }
std::string GetProcessorCPUIDString() { return ""; }
std::string GetProcessorExtendedCPUIDInfo() { return ""; }
unsigned long GetCPUSpeedFromRegistry(unsigned long /*dwCPU*/) { return 0; }
int GetMaxSimultaneousThreads() { return 0; }

#endif
