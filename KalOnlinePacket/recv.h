#pragma once
#include <map>
#include <set>
#include <windows.h>
#include "Header.h"

#pragma pack(push, 1)
typedef struct SItemDrop
{
	uint16_t size; // 0
	uint8_t header; // 2
	uint16_t index; // 3
	uint32_t iid; // 5
	uint32_t x; // 9
	uint32_t y; // 13
	uint32_t unknown; //17
	uint32_t amount; // 21

} SItemDrop, * PItemDrop;
typedef struct SSomeoneAttack
{
	uint16_t size;
	uint8_t header;
	uint32_t attackerID;
	uint32_t targetID;
	uint16_t damage;
	uint16_t damage2;
}SSomeoneAttack, * PSomeoneAttack;
typedef struct SMonsterMove
{
	uint16_t size;
	uint8_t header;
	uint32_t id;
	uint8_t xstep;
	uint8_t ystep;
}SMonsterMove, * PMonsterMove;
typedef struct SMonsterSpawn
{
	uint16_t size;
	uint8_t header;
	uint16_t index;
	uint32_t id;
	uint32_t x;
	uint32_t y;
	uint32_t z;
	uint16_t hp;
}SMonsterSpawn, * PMonsterSpawn;
typedef struct SMonsterDespawn
{
	uint16_t size;
	uint8_t header;
	uint32_t id;
}SMonsterDespawn, * PMonsterDespawn;
typedef struct SSomeoneDie
{
	uint16_t size;
	uint8_t header;
	uint32_t id;
	uint8_t unknown;
	uint8_t type;
}SSomeoneDie, * PSomeoneDie;
typedef struct SStatChange
{
	uint16_t size;
	uint8_t header;
	uint8_t type; 
}SStatChange, *PStatChange;
#pragma pack(pop)
enum CharacterStatChange
{
	hp = 0x07,
	mana = 0x08,
	experience = 0x19,
	minMaxPhysic = 0x1B,
	minMaxMagic = 0x1C,
	defense = 0x0F
};
struct BotConfig
{
	double pickRange =					0.00;
	double attackRange =				0.00;

	std::set<uint16_t> itemFilter =		{};
	uint16_t itemFitlerGreaterThen =	0;

};
//458752003 sob quest id     SendPacket(0x30,"bbbb",3,0,88,27);
class Player
{
public:
	uint16_t currentHP;
	uint16_t maxHP;
	uint16_t currentMana;
	uint16_t maxMana;


	POINT coordinates;

};
class CRecv
{
public:

	CRecv()
	{
		mPlayer = std::make_unique<Player>();
		mBotSettings = std::make_unique<BotSettings>();
		mBotConfig = std::make_unique<BotConfig>();
	}

	~CRecv();

	std::unique_ptr<Player> mPlayer;
	std::unique_ptr<BotSettings> mBotSettings;

	std::unique_ptr<BotConfig> mBotConfig;

	void FilterRecv(char* Packet);

private:

	void ItemSpawn(char* Packet);

	void StatChange(char* Packet);

	bool getDistance(POINT target, double range);


};
extern void(__cdecl* SendPacketInix)(BYTE type, LPCSTR format, ...);