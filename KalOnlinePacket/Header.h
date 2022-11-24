#pragma once
#include <WS2tcpip.h>
#include <sstream>

#pragma comment (lib, "ws2_32.lib")
#include <windows.h>
#include <iostream>
#include <thread>
#include <set>
#include <fcntl.h>
#include <io.h>

#define INST_NOP 0x90
#define INST_CALL 0xe8
#define INST_JMP 0xe9
#define INST_BYTE 0x00
#define SHORT_JZ 0x74


LPVOID MemcpyEx(DWORD lpDest, DWORD lpSource, int len);
DWORD Intercept(int instruction, DWORD lpSource, DWORD lpDest, int len);
void FillMemoryEx(unsigned long Destination, unsigned char Fill, size_t Size);
void CreateConsole();
class Point3
{

public:
	uint32_t x;
	uint32_t y;
	uint32_t z;
	Point3(uint32_t x, uint32_t y, uint32_t z)
	{
		this->x = x;
		this->y = y;
		this->z = z;
	}

};
namespace InixSoft
{
	void RecvInit();
	void InitSend();
}
struct myPlayer
{
	uint32_t id = 0;
	int32_t x = 0;
	int32_t y = 0;
	int32_t z = 0;
	int16_t hp = 0;
	int16_t maxhp = 0;
	int16_t mana = 0;
	int16_t maxmana = 0;

};
struct BotSettings
{
	bool d1Bot = false;
	bool sniffRecv = false;
	bool sniffSend = false;
	bool pickup = true;
	bool setMed = false;
	uint32_t medID = 0;
	uint32_t geonCollected = 0;
	bool botrunning = false;
	bool moveBot = false;
	bool scanSkills = false;
	uint8_t skillid = 0;
	bool debugOutput = false;
	uint32_t moveFastMultiplicator = 0;
	uint32_t moveSlowMultiplicator = 0;
	double pickrange = 0.00;
	double attackrange = 0.00;
	uint32_t movefastsleep = 0;
	uint32_t moveslowsleep = 0;
	std::set<int> justAttackMobs{};
	bool justAttackMobsMode = false;
	uint32_t sleepafterkill = 0;
	uint32_t sleepbeforeskillattack = 0;
	std::set<uint16_t> blockMobsIndex{};
	uint32_t delayAttack = 0;
	uint32_t failAttackCounterLimit = 0;
	POINT startPositionBot{};
	POINT exitPositionBot{};
	uint32_t mobskilled = 0;
	int16_t lowhpexit = 600;
	bool behadeall = false;
	double movefastrange = 2000.00;
	double moveslowrange = 1000.00;
	SOCKET guiSocket;
};
enum S2C
{
	ItemInventory = 0x19,
	LoadGuildWindow = 0x41,
	StatChange = 0x4F,
	TeleportCoordinates = 0x45,
	ItemDisappear = 0x57,
	PlayerAppear = 0x6B,
	ChatMessage = 0x6D,
	LoadCharacterList = 0x72,
	LoadInventory = 0x7C,
	ConnectButtonAnswer = 0x7D,
	SomeoneDie = 0x8F,
	ItemToInventory = 0x85,
	MonsterDisappear = 0x87,
	SomeoneAttack = 0x90,
	MonsterAppear = 0x93,
	LoginAnswer = 0x95,
	MonsterMove = 0x98,
	ItemDropped = 0x99,
	Notice = 0xA9,
	MonsterMoveStop = 0xAD,
	AddBuffIcon = 0x34

};
enum TricksterSkills
{
	TrickSlashAdd = 14,
	SwordBarrier = 15,
	CrossSlash = 18
};
/*

*/
enum C2S
{
	PreSkillPacket = 0x65, // Format [ BYTE DWORD - SkillID - TargetID ]
	DestroyItem = 0x67, // Format [ DWORD - ItemIID ]
	LearnSkill = 0xB8, //  Format [ DWORD - SkillID ]
	PickItem = 0xBA,// Format [ DWORD DWORD DWORD BYTE - IID X/32 Y32 0 ]
	MoveStop = 0x99,// Format [ BYTE BYTE BYTE - x y z Steps ]
	Move = 0xD6, // Format [ BYTE BYTE BYTE - x y z Steps ]
	DropItem = 0x72, // Format [ DWORD - iid ]
	UseItem = 0xAD, // Format [ DWORD - iid ]
	UseSkill = 0xC7 // Format [ BYTE BYTE DWORD - SkillID 1 TargetID ]
};
enum ShamanSkills
{
	Behade = 1,
	GhostBlade = 13,
	GhostFlash = 14,
	SoulShield = 15, // Send c7 -> bbd (3) : [B - 1:15][B - 2:0][D - 3:109093719]
	Wave = 18,
	PainOfSoul = 22
};
struct Monster {
	uint16_t index;
	uint32_t hp;
	uint32_t x;
	uint32_t y;
	uint32_t z;
	float distance;
};
struct JobChangeNPC
{
	POINT sae_ryue = { 0,0 };
	POINT in_pill = { 0,0 };

};
bool HplowExit();
bool moveToPosition(POINT position);
void D1BotThread();

void ConfigStatus();
void ReadConfig();

PBYTE FindSignature(LPVOID BaseAddress, DWORD ImageSize, PCHAR Signature, PCHAR Mask);
uintptr_t GetModuleBaseAddress(DWORD procId, const wchar_t* modName);
Point3 getCoordinates();
#define recvAdr 0x00BB9759
#define sendAdr 0x00BD77C0
#define CharacterMasterPointer 0x00D183F8