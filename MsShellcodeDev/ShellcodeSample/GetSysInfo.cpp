#include "framework.h"
#include "GetSysInfo.h"
#include <shlwapi.h>
#include <LM.h>

#pragma comment(lib, "shlwapi")
#pragma comment(lib, "netapi32")

LPWSTR GetSysInfo(HANDLE hHeap)
{
	WCHAR pwszStr[MAX_PATH];
	LPWSTR pwszRet;
	DWORD dwBufLen;
	LPSTREAM pStream = /*/shlwapi.dll#12/*/SHCreateMemStream(0, 0);
	/*/shlwapi.dll#212/*/IStream_Write(pStream, L"Computer Name: ", 30);
	dwBufLen = MAX_PATH;
	/*/kernel32.dll/*/GetComputerNameW(pwszStr, &dwBufLen);
	/*/shlwapi.dll#212/*/IStream_Write(pStream, pwszStr, dwBufLen * sizeof(WCHAR));
	NETSETUP_JOIN_STATUS dwJoinType;
	pwszRet = NULL;
	/*/netapi32.dll/*/NetGetJoinInformation(NULL, &pwszRet, &dwJoinType);
	if (dwJoinType >= NetSetupWorkgroupName)
	{
		if (dwJoinType == NetSetupWorkgroupName)
			/*/shlwapi.dll#212/*/IStream_Write(pStream, L"\r\nWorkgroup Name: ", 36);
		else
			/*/shlwapi.dll#212/*/IStream_Write(pStream, L"\r\nDomain Name: ", 30);
		dwBufLen = /*/kernel32.dll/*/lstrlenW(pwszRet);
		/*/shlwapi.dll#212/*/IStream_Write(pStream, pwszRet, dwBufLen * sizeof(WCHAR));
	}
	/*/netapi32.dll/*/NetApiBufferFree(pwszRet);
	pwszRet = NULL;
	dwBufLen = MAX_PATH;
	/*/advapi32.dll/*/GetUserNameW(pwszStr, &dwBufLen);
	/*/shlwapi.dll#212/*/IStream_Write(pStream, L"\r\nUser Name: ", 26);
	/*/shlwapi.dll#212/*/IStream_Write(pStream, pwszStr, dwBufLen * sizeof(WCHAR));
	ULARGE_INTEGER uiSize;
	/*/shlwapi.dll#214/*/IStream_Size(pStream, &uiSize);
	pwszRet = (LPWSTR)/*/kernel32.dll/*/HeapAlloc(hHeap, HEAP_ZERO_MEMORY, uiSize.LowPart);
	/*/shlwapi.dll#213/*/IStream_Reset(pStream);
	/*/shlwapi.dll#184/*/IStream_Read(pStream, pwszRet, uiSize.LowPart);
	/*/shlwapi.dll#169/*/IUnknown_AtomicRelease((LPVOID*)&pStream);
	return pwszRet;
}