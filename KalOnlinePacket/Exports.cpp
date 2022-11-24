#include "Windows.h"
#include <string>
#include "Exports.h"


FARPROC IExports::p[];
volatile unsigned long IExports::_p;

void IExports::Initialize()
{
	wchar_t* _system = new wchar_t[MAX_PATH];
	GetSystemDirectoryW(_system, MAX_PATH);
	std::wstring _syspath = _system;
	delete _system;
	_syspath += L"\\version.dll";
	this->hL = LoadLibraryW(_syspath.c_str());

	p[0] = GetProcAddress(this->hL, "GetFileVersionInfoA");
	p[1] = GetProcAddress(this->hL, "GetFileVersionInfoByHandle");
	p[2] = GetProcAddress(this->hL, "GetFileVersionInfoExW");
	p[3] = GetProcAddress(this->hL, "GetFileVersionInfoSizeA");
	p[4] = GetProcAddress(this->hL, "GetFileVersionInfoExW");
	p[5] = GetProcAddress(this->hL, "GetFileVersionInfoSizeW");
	p[6] = GetProcAddress(this->hL, "GetFileVersionInfoW");
	p[7] = GetProcAddress(this->hL, "VerFindFileA");
	p[8] = GetProcAddress(this->hL, "VerFindFileW");
	p[9] = GetProcAddress(this->hL, "VerInstallFileA");
	p[10] = GetProcAddress(this->hL, "VerInstallFileW");
	p[11] = GetProcAddress(this->hL, "VerLanguageNameA");
	p[12] = GetProcAddress(this->hL, "VerLanguageNameW");
	p[13] = GetProcAddress(this->hL, "VerQueryValueA");
	p[14] = GetProcAddress(this->hL, "VerQueryValueW");
}
extern "C" __declspec(naked) void __stdcall __E__0__() { IExports::_p = (unsigned long)IExports::p[0]; __asm jmp IExports::_p }
extern "C" __declspec(naked) void __stdcall __E__1__() { IExports::_p = (unsigned long)IExports::p[1]; __asm jmp IExports::_p }
extern "C" __declspec(naked) void __stdcall __E__2__() { IExports::_p = (unsigned long)IExports::p[2]; __asm jmp IExports::_p }
extern "C" __declspec(naked) void __stdcall __E__3__() { IExports::_p = (unsigned long)IExports::p[3]; __asm jmp IExports::_p }
extern "C" __declspec(naked) void __stdcall __E__4__() { IExports::_p = (unsigned long)IExports::p[4]; __asm jmp IExports::_p }
extern "C" __declspec(naked) void __stdcall __E__5__() { IExports::_p = (unsigned long)IExports::p[5]; __asm jmp IExports::_p }
extern "C" __declspec(naked) void __stdcall __E__6__() { IExports::_p = (unsigned long)IExports::p[6]; __asm jmp IExports::_p }
extern "C" __declspec(naked) void __stdcall __E__7__() { IExports::_p = (unsigned long)IExports::p[7]; __asm jmp IExports::_p }
extern "C" __declspec(naked) void __stdcall __E__8__() { IExports::_p = (unsigned long)IExports::p[8]; __asm jmp IExports::_p }
extern "C" __declspec(naked) void __stdcall __E__9__() { IExports::_p = (unsigned long)IExports::p[9]; __asm jmp IExports::_p }
extern "C" __declspec(naked) void __stdcall __E__10__() { IExports::_p = (unsigned long)IExports::p[10]; __asm jmp IExports::_p }
extern "C" __declspec(naked) void __stdcall __E__11__() { IExports::_p = (unsigned long)IExports::p[11]; __asm jmp IExports::_p }
extern "C" __declspec(naked) void __stdcall __E__12__() { IExports::_p = (unsigned long)IExports::p[12]; __asm jmp IExports::_p }
extern "C" __declspec(naked) void __stdcall __E__13__() { IExports::_p = (unsigned long)IExports::p[13]; __asm jmp IExports::_p }
extern "C" __declspec(naked) void __stdcall __E__14__() { IExports::_p = (unsigned long)IExports::p[14]; __asm jmp IExports::_p }


