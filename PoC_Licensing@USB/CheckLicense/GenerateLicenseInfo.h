#pragma once

extern LPSTR GenKeyHexHmac(HANDLE, LPBYTE, DWORD, LPBYTE, DWORD);
extern DWORD GetDriveSN(HANDLE, WCHAR, LPDWORD, LPSTR *);