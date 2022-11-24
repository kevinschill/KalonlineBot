// DLLFix.cpp : Defines the exported functions for the DLL application.
//

#include "Header.h"
#include "detours.h"
/*

// Type definitions so we can create pointers to original functions
typedef HANDLE(__stdcall* CreateFileAPtr)(LPCSTR lpFileName,
	DWORD dwDesiredAccess,
	DWORD dwShareMode,
	LPSECURITY_ATTRIBUTES lpSecurityAttributes,
	DWORD dwCreationDisposition,
	DWORD dwFlagsAndAttributes,
	HANDLE hTemplateFile
	);
typedef DWORD(__stdcall* GetFileVersionInfoSizeAPtr)(LPCSTR lptstrFilename, LPDWORD lpdwHandle);
typedef BOOL(__stdcall* GetFileVersionInfoAPtr)(LPCSTR lptstrFilename, DWORD dwHandle, DWORD dwLen, LPVOID lpData);
typedef HMODULE(__stdcall* LoadLibraryAPtr)(LPCSTR lpLibFileName);

// Detour functions prototypes
HANDLE __stdcall Detour_CreateFileA(LPCSTR lpFileName,
	DWORD dwDesiredAccess,
	DWORD dwShareMode,
	LPSECURITY_ATTRIBUTES lpSecurityAttributes,
	DWORD dwCreationDisposition,
	DWORD dwFlagsAndAttributes,
	HANDLE hTemplateFile
);
DWORD	__stdcall Detour_GetFileVersionInfoSizeA(LPCSTR lptstrFilename, LPDWORD lpdwHandle);
BOOL	__stdcall Detour_GetFileVersionInfoA(LPCSTR lptstrFilename, DWORD dwHandle, DWORD dwLen, LPVOID lpData);
HMODULE __stdcall Detour_LoadLibraryA(LPCSTR lpLibFileName);

CreateFileAPtr				CreateFileA_Original;
GetFileVersionInfoSizeAPtr	GetFileVersionInfoSizeA_Original;
GetFileVersionInfoAPtr		GetFileVersionInfoA_Original;
LoadLibraryAPtr				LoadLibraryA_Original;
extern int g_nDetourError;


HMODULE __stdcall Detour_LoadLibraryA(LPCSTR lpLibFileName)
{
	HMODULE hResult = LoadLibraryA_Original(lpLibFileName);




	if (strcmp(lpLibFileName, "VERSION.dll") == 0)
	{
		CRITICAL_SECTION critSection;
		InitializeCriticalSection(&critSection);
		EnterCriticalSection(&critSection);

		*(FARPROC*)&CreateFileA_Original = GetProcAddress(GetModuleHandleW(L"kernel32.dll"), "CreateFileA");
		*(FARPROC*)&GetFileVersionInfoA_Original = GetProcAddress(GetModuleHandleW(L"VERSION.dll"), "GetFileVersionInfoA");
		*(FARPROC*)&GetFileVersionInfoSizeA_Original = GetProcAddress(GetModuleHandleW(L"VERSION.dll"), "GetFileVersionInfoSizeA");


		DetourRestoreAfterWith();
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		DetourAttach(&(PVOID&)CreateFileA_Original, Detour_CreateFileA);
		DetourAttach(&(PVOID&)GetFileVersionInfoA_Original, Detour_GetFileVersionInfoA);
		DetourAttach(&(PVOID&)GetFileVersionInfoSizeA_Original, Detour_GetFileVersionInfoSizeA);
		DetourDetach(&(PVOID&)LoadLibraryA_Original, Detour_LoadLibraryA);
		g_nDetourError = DetourTransactionCommit();

		LeaveCriticalSection(&critSection);

	}

	return hResult;
}

HANDLE __stdcall Detour_CreateFileA(LPCSTR lpFileName,
	DWORD dwDesiredAccess,
	DWORD dwShareMode,
	LPSECURITY_ATTRIBUTES lpSecurityAttributes,
	DWORD dwCreationDisposition,
	DWORD dwFlagsAndAttributes,
	HANDLE hTemplateFile
)
{
	bool bDetach = false;
	HANDLE hReturn = 0;

	if (strcmp(lpFileName, "dsound.dll") == NULL)
	{
		lpFileName = "dsound_.dll";
		bDetach = true;
	}

	hReturn = CreateFileA_Original(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);

	if (bDetach == true)
	{
		DetourRestoreAfterWith();
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		DetourDetach(&(PVOID&)CreateFileA_Original, Detour_CreateFileA);
	}

	return hReturn;
}

DWORD __stdcall Detour_GetFileVersionInfoSizeA(LPCSTR lptstrFilename, LPDWORD lpdwHandle)
{
	bool bDetach = false;
	DWORD dwReturn = 0;
	if (strcmp(lptstrFilename, "dsound.dll") == 0)
	{
		lptstrFilename = "dsound_.dll";
		bDetach = true;
	}

	dwReturn = GetFileVersionInfoSizeA_Original(lptstrFilename, lpdwHandle);

	if (bDetach == true)
	{
		DetourRestoreAfterWith();
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		DetourDetach(&(PVOID&)GetFileVersionInfoSizeA_Original, Detour_GetFileVersionInfoSizeA);
	}

	return dwReturn;
}


BOOL  __stdcall Detour_GetFileVersionInfoA(LPCSTR lptstrFilename, DWORD dwHandle, DWORD dwLen, LPVOID lpData)
{
	bool bDetach = false;
	DWORD dwReturn = 0;
	if (strcmp(lptstrFilename, "dsound.dll") == 0)
	{
		lptstrFilename = "dsound_.dll";
		bDetach = true;
	}

	dwReturn = GetFileVersionInfoA_Original(lptstrFilename, dwHandle, dwLen, lpData);

	if (bDetach == true)
	{
		DetourRestoreAfterWith();
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		DetourDetach(&(PVOID&)GetFileVersionInfoA_Original, Detour_GetFileVersionInfoA);
	}

	return dwReturn;
}
*/