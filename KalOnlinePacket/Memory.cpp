#include "Header.h"
#include <TlHelp32.h>

Point3 getCoordinates()
{
	DWORD add = *(DWORD*)CharacterMasterPointer;
	auto PositionX = *(int*)(add + 18192);
	auto PositionZ = *(int*)(add + 18196);
	auto PositionY = *(int*)(add + 18200);
	return Point3(PositionX, PositionY, PositionZ);
}

uintptr_t GetModuleBaseAddress(DWORD procId, const wchar_t* modName)
{
	uintptr_t modBaseAddr = 0;
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, procId);
	if (hSnap != INVALID_HANDLE_VALUE)
	{
		MODULEENTRY32 modEntry;
		modEntry.dwSize = sizeof(modEntry);
		if (Module32First(hSnap, &modEntry))
		{
			do
			{
				if (!_wcsicmp(modEntry.szModule, modName))
				{
					modBaseAddr = (uintptr_t)modEntry.modBaseAddr;
					break;
				}
			} while (Module32Next(hSnap, &modEntry));
		}
	}
	CloseHandle(hSnap);
	return modBaseAddr;
}
int CompareByteArray(PBYTE ByteArray1, PCHAR ByteArray2, PCHAR Mask, DWORD Length)
{
	DWORD nextStart = 0;
	char start = ByteArray2[0];
	for (DWORD i = 0; i < Length; i++)
	{
		if (Mask[i] == '?')
		{
			continue;
		}
		if (ByteArray1[i] == start)
		{
			nextStart = i;
		}
		if (ByteArray1[i] != (BYTE)ByteArray2[i])
		{
			return nextStart;
		}
	}
	return -1;
}

PBYTE FindSignature(LPVOID BaseAddress, DWORD ImageSize, PCHAR Signature, PCHAR Mask)
{
	PBYTE Address = NULL;
	PBYTE Buffer = (PBYTE)BaseAddress;

	DWORD Length = strlen(Mask);

	for (DWORD i = 0; i < (ImageSize - Length); i++)
	{
		int result = CompareByteArray((Buffer + i), Signature, Mask, Length);
		if (result < 0)
		{
			Address = (PBYTE)BaseAddress + i;
			break;
		}
		else
		{
			i += result;
		}
	}

	return Address;
}

void FillMemoryEx(unsigned long Destination, unsigned char Fill, size_t Size)
{
	unsigned long oldDestProt;
	VirtualProtect((void*)Destination, Size, PAGE_EXECUTE_READWRITE, &oldDestProt);
	FillMemory((void*)Destination, Size, Fill);
	VirtualProtect((void*)Destination, Size, oldDestProt, &oldDestProt);
}

LPVOID MemcpyEx(DWORD lpDest, DWORD lpSource, int len)
{
	DWORD oldDestProt = 0;
	VirtualProtect((LPVOID)lpDest, len, PAGE_EXECUTE_READWRITE, &oldDestProt);
	memcpy((void*)lpDest, (void*)lpSource, len);
	VirtualProtect((LPVOID)lpDest, len, oldDestProt, &oldDestProt);
	return (LPVOID)lpDest;
}


DWORD Intercept(int instruction, DWORD lpSource, DWORD lpDest, int len)
{
	DWORD realtarget{};
	LPBYTE buffer = new BYTE[len];

	memset(buffer, 0x90, len);
	if (instruction != INST_NOP && len >= 5)
	{
		buffer[(len - 5)] = instruction;
		DWORD dwJMP = (DWORD)lpDest - (lpSource + 5 + (len - 5));
		memcpy(&realtarget, (void*)(lpSource + 1), 4);
		realtarget = realtarget + lpSource + 5;
		memcpy(buffer + 1 + (len - 5), &dwJMP, 4);
	}
	if (instruction == SHORT_JZ)
	{
		buffer[0] = instruction;
		buffer[1] = (BYTE)lpDest;
	}
	if (instruction == INST_BYTE)
	{
		buffer[0] = (BYTE)lpDest;
	}
	MemcpyEx(lpSource, (DWORD)buffer, len);
	delete[] buffer;
	return realtarget;
}
void CreateConsole()
{
	if (!AllocConsole()) {
		return;
	}
	FILE* fDummy;
	freopen_s(&fDummy, "CONOUT$", "w", stdout);
	freopen_s(&fDummy, "CONOUT$", "w", stderr);
	freopen_s(&fDummy, "CONIN$", "r", stdin);
	std::cout.clear();
	std::clog.clear();
	std::cerr.clear();
	std::cin.clear();

	HANDLE hConOut = CreateFileA(("CONOUT$"), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	HANDLE hConIn = CreateFileA(("CONIN$"), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	SetStdHandle(STD_OUTPUT_HANDLE, hConOut);
	SetStdHandle(STD_ERROR_HANDLE, hConOut);
	SetStdHandle(STD_INPUT_HANDLE, hConIn);
	std::wcout.clear();
	std::wclog.clear();
	std::wcerr.clear();
	std::wcin.clear();
}