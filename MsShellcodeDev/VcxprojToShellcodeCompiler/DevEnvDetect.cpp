#include "framework.h"
#include "DevEnvDetect.h"
#include <commdlg.h>
#include <shellapi.h>
#include <Shobjidl.h>
#include <Propkey.h>
#include <atlbase.h>

DWORD GetVcVars32Path(HANDLE hHeap, LPBOOL pbNeedSDK, LPDWORD pdwVerMsDev,  LPWSTR * ppwszMsDev, LPWSTR * ppwszVcBat)
{
	DWORD dwRet = ERSC_NOERROR, dwType = REG_SZ, dwSize = MAX_PATH * sizeof(WCHAR);
	WCHAR pwszPath[MAX_PATH];
	*pdwVerMsDev = 0;
	pwszPath[0] = L'\0';
	HKEY hKey;
	LONG lRet = RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\devenv.exe", 0, KEY_READ, &hKey);
	if (!lRet)
	{
		RegQueryValueExW(hKey, NULL, NULL, &dwType, (LPBYTE)pwszPath, &dwSize);
		RegCloseKey(hKey);
		if (lstrlenW(pwszPath))
		{
			if (pwszPath[0] == L'\"')
			{
				pwszPath[lstrlenW(pwszPath) - 1] = L'\0';
				lstrcpyW(pwszPath, pwszPath + 1);
			}
			*pdwVerMsDev = GetFileProperties(hHeap, pwszPath, ppwszMsDev);
			if (*pdwVerMsDev)
			{
				if (*pdwVerMsDev > 14)
					*pbNeedSDK = TRUE;
				else
					*pbNeedSDK = FALSE;
				lstrcpyW(StrRStrIW(pwszPath, NULL, L"Common7"), L"VC\\");
				lstrcatW(pwszPath, *pbNeedSDK ? L"Auxiliary\\Build\\vcvars32.bat" : L"bin\\vcvars32.bat");
				if (PathFileExistsW(pwszPath))
				{
					dwSize = lstrlenW(pwszPath) * sizeof(WCHAR) + 2;
					*ppwszVcBat = (LPWSTR)HeapAlloc(hHeap, HEAP_ZERO_MEMORY, dwSize);
					lstrcpyW(*ppwszVcBat, pwszPath);
				}
				else
					dwRet = ERSC_NOMSVC;
			}
			else
				dwRet = ERSC_OLDMSDEV;
		}
		else
			dwRet = ERSC_NOMSDEV;
	}
	if (dwRet)
	{
		HeapFree(hHeap, 0, *ppwszMsDev);
		*ppwszMsDev = NULL;
		HeapFree(hHeap, 0, *ppwszVcBat);
		*ppwszVcBat = NULL;
	}
	return dwRet;
}

DWORD GetSDKsNumStr(HANDLE hHeap, PMSSDK_INFO pMsSDK_Info)
{
	DWORD dwRet = ERSC_NOERROR, dwType = REG_SZ, dwSize = MAX_PATH;
	CHAR pszPath[MAX_PATH];
	LPSTREAM pStrm = NULL;
	HANDLE hFind;
	WIN32_FIND_DATAA findData;
	pszPath[0] = '\0';
	HKEY hKey;
	LONG lRet = RegOpenKeyExA(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Microsoft SDKs\\Windows\\v10.0", 0, KEY_READ | KEY_WOW64_32KEY, &hKey);
	if (!lRet)
	{
		RegQueryValueExA(hKey, "InstallationFolder", NULL, &dwType, (LPBYTE)pszPath, &dwSize);
		RegCloseKey(hKey);
		if (lstrlenA(pszPath))
		{
			lstrcatA(pszPath, "Include\\*");
			__stosb((LPBYTE)&findData, 0, sizeof(WIN32_FIND_DATAA));
			hFind = FindFirstFileA(pszPath, &findData);
			dwType = 0;
			if (hFind != INVALID_HANDLE_VALUE)
			{
				PMSSDK_ITEM pItemNow = NULL;
				do
				{
					if (lstrlenA(findData.cFileName) > 2)
					{
						if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
						{
							pMsSDK_Info->dwNumSDKs += 1;
							PMSSDK_ITEM pItemNew = (PMSSDK_ITEM)HeapAlloc(hHeap, HEAP_ZERO_MEMORY, sizeof(MSSDK_ITEM));
							if (pMsSDK_Info->pMsSdk_Item)
							{
								pItemNow->pNext = pItemNew;
								pItemNow = pItemNew;
							}
							else
							{
								pItemNow = pItemNew;
								pMsSDK_Info->pMsSdk_Item = pItemNow;
							}
							pItemNow->pszSDK = (LPSTR)HeapAlloc(hHeap, HEAP_ZERO_MEMORY, lstrlenA(findData.cFileName) + 1);
							lstrcpyA(pItemNow->pszSDK, findData.cFileName);
						}
					}
					__stosb((LPBYTE)&findData, 0, sizeof(WIN32_FIND_DATAA));
				} while (FindNextFileA(hFind, &findData));
				FindClose(hFind);
			}
		}
	}
	if (!pMsSDK_Info->dwNumSDKs)
		dwRet = ERSC_NOSDK;

	return dwRet;
}

DWORD GetFileProperties(HANDLE hHeap, LPWSTR pwszPath, LPWSTR * ppwszMsDev)
{
	DWORD dwVerRet = 0;
	LPWSTR pwszRet = NULL;
	CComPtr<IShellItem2> pItem;
	HRESULT hr = SHCreateItemFromParsingName(pwszPath, NULL, IID_PPV_ARGS(&pItem));
	if (!FAILED(hr))
	{
		CComHeapPtr<WCHAR> pValueVersion, pValueDesc;
		hr = pItem->GetString(PKEY_FileVersion, &pValueVersion);
		if (!FAILED(hr))
		{
			dwVerRet = (DWORD)StrToIntW(pValueVersion.m_pData);
			if (dwVerRet < 14)
				dwVerRet = 0;
			else
			{
				hr = pItem->GetString(PKEY_FileDescription, &pValueDesc);
				if (!FAILED(hr))
				{
					*ppwszMsDev = (LPWSTR)HeapAlloc(hHeap, HEAP_ZERO_MEMORY, (lstrlenW(pValueDesc.m_pData) + lstrlenW(pValueVersion.m_pData) + 4) * 2);
					wsprintfW(*ppwszMsDev, L"%s (%s)", pValueDesc.m_pData, pValueVersion.m_pData);
					pValueDesc.Free();
				}
				else
					dwVerRet = 0;
			}
			pValueVersion.Free();
		}
		pItem.Release();
	}
	return dwVerRet;
}

BOOL GetOpenFilePath(HANDLE hHeap, HWND hDlg, LPWSTR* pwszPath)
{
	BOOL bRet = FALSE;
	IFileOpenDialog* pFileOpen;
	HRESULT hr;
	hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, IID_IFileOpenDialog, (LPVOID*)&pFileOpen);
	if (SUCCEEDED(hr))
	{
		COMDLG_FILTERSPEC rgSpec[] =
		{
			{ L"VC++ Project files", L"*.vcxproj" }
		};
		pFileOpen->SetFileTypes(1, rgSpec);
		//pFileOpen->SetFileTypes()
		hr = pFileOpen->Show(hDlg);
		if (SUCCEEDED(hr))
		{
			IShellItem* pItem;
			hr = pFileOpen->GetResult(&pItem);
			if (SUCCEEDED(hr))
			{
				PWSTR pwszTmpPath;
				hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pwszTmpPath);
				if (SUCCEEDED(hr))
				{
					if (*pwszPath)
						HeapFree(hHeap, 0, *pwszPath);
					size_t lenTmp = wcslen(pwszTmpPath) + 1;
					*pwszPath = (LPWSTR)HeapAlloc(hHeap, HEAP_ZERO_MEMORY, lenTmp * 2);
					lstrcpyW(*pwszPath, pwszTmpPath);
					CoTaskMemFree(pwszTmpPath);
					bRet = TRUE;
				}
				pItem->Release();
			}
		}
		pFileOpen->Release();
	}
	return bRet;
}

VOID DeletePath(HANDLE hHeap, LPWSTR pwszPath)
{
	if (PathFileExistsW(pwszPath))
	{
		int lenPath = lstrlenW(pwszPath) + 2;
		LPWSTR pwszFrom = (LPWSTR)HeapAlloc(hHeap, HEAP_ZERO_MEMORY, lenPath * 2);
		lstrcpyW(pwszFrom, pwszPath);
		SHFILEOPSTRUCTW stSHFile = { 0 };
		stSHFile.wFunc = FO_DELETE;
		stSHFile.pFrom = pwszFrom;
		stSHFile.fFlags = FOF_NOCONFIRMATION | FOF_SILENT;
		stSHFile.fAnyOperationsAborted = FALSE;
		SHFileOperationW(&stSHFile);
		HeapFree(hHeap, 0, pwszFrom);
	}
}