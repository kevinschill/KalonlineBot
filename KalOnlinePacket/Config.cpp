#include "Header.h"


extern BotSettings gBotSettings;
extern myPlayer gmyPlayer;
void ReadConfig()
{
	char justattackmobBuffer[255];
	GetPrivateProfileStringA("Bot", "justattackmobs", 0, justattackmobBuffer, 255, ".\\config.ini");
	std::stringstream ss(justattackmobBuffer);
	gBotSettings.justAttackMobs.clear();
	for (int i; ss >> i;) {
		gBotSettings.justAttackMobs.insert(i);
		if (ss.peek() == ',')
			ss.ignore();
	}
	gBotSettings.skillid = GetPrivateProfileIntA("Bot", "skillid", 0, ".\\config.ini");
	gmyPlayer.hp = GetPrivateProfileIntA("Bot", "playerhp", 0, ".\\config.ini");
	gmyPlayer.maxhp = gmyPlayer.hp;
	gmyPlayer.mana = GetPrivateProfileIntA("Bot", "playermana", 0, ".\\config.ini");
	gmyPlayer.maxmana = gmyPlayer.hp;
	gBotSettings.medID = GetPrivateProfileIntA("Bot", "medid", 0, ".\\config.ini");
	gBotSettings.moveFastMultiplicator = GetPrivateProfileIntA("Bot", "movefastmultiplicator", 0, ".\\config.ini");
	gBotSettings.moveSlowMultiplicator = GetPrivateProfileIntA("Bot", "moveslowmultiplicator", 0, ".\\config.ini");
	gBotSettings.pickrange = (double)GetPrivateProfileIntA("Bot", "pickrange", 0, ".\\config.ini");
	gBotSettings.attackrange = (double)GetPrivateProfileIntA("Bot", "attackrange", 0, ".\\config.ini");
	gBotSettings.movefastsleep = GetPrivateProfileIntA("Bot", "movefastsleep", 0, ".\\config.ini");
	gBotSettings.moveslowsleep = GetPrivateProfileIntA("Bot", "moveslowsleep", 0, ".\\config.ini");
	gBotSettings.delayAttack = GetPrivateProfileIntA("Bot", "delayattack", 0, ".\\config.ini");

	gBotSettings.sleepafterkill = GetPrivateProfileIntA("Bot", "sleepafterkill", 0, ".\\config.ini");
	gBotSettings.sleepbeforeskillattack = GetPrivateProfileIntA("Bot", "sleepbeforeskillattack", 0, ".\\config.ini");

	gBotSettings.failAttackCounterLimit = GetPrivateProfileIntA("Bot", "failAttackCounterLimit", 0, ".\\config.ini");

	gBotSettings.lowhpexit = GetPrivateProfileIntA("Bot", "lowhpexit", 0, ".\\config.ini");

	gBotSettings.movefastrange = (double)GetPrivateProfileIntA("Bot", "movefastrange", 0, ".\\config.ini");
	gBotSettings.moveslowrange = (double)GetPrivateProfileIntA("Bot", "moveslowrange", 0, ".\\config.ini");


	int bhd = GetPrivateProfileIntA("Bot", "behadeall", 0, ".\\config.ini");
	if (bhd == 1)
		gBotSettings.behadeall = true;
	else
		gBotSettings.behadeall = false;

	char blockMobsIndexBuffer[500];
	GetPrivateProfileStringA("Bot", "monsterblocklist", 0, blockMobsIndexBuffer, 500, ".\\config.ini");
	std::stringstream blockmbosstream(blockMobsIndexBuffer);
	gBotSettings.blockMobsIndex.clear();
	for (int i; blockmbosstream >> i;) {
		gBotSettings.blockMobsIndex.insert(i);
		if (blockmbosstream.peek() == ',')
			blockmbosstream.ignore();
	}
	for (auto& x : gBotSettings.blockMobsIndex)
		printf("%d -", x);
	printf("\n");
	printf("\n"
		"[Config Reload]\n"
		"SkillID           ( %d )\n"
		"PlayerHP          ( %d )\n"
		"MedID             ( %d )\n"
		"MoveFast          ( %d )\n"
		"MoveSlow          ( %d )\n"
		"PickRange         ( %.2f )\n"
		"AttackRange       ( %.2f )\n"
		"MoveFastSleep     ( %d )\n"
		"MoveSlowSleep     ( %d )\n"
		"SleepSkill        ( %d )\n"
		"SlepAfterKill     ( %d )\n"
		"FailAttackLimit   ( %d )\n"
		"\n"
		, gBotSettings.skillid, gmyPlayer.maxhp, gBotSettings.medID, gBotSettings.moveFastMultiplicator
		, gBotSettings.moveSlowMultiplicator, gBotSettings.pickrange, gBotSettings.attackrange
		, gBotSettings.movefastsleep, gBotSettings.moveslowsleep, gBotSettings.sleepbeforeskillattack, gBotSettings.sleepafterkill
		, gBotSettings.failAttackCounterLimit);

}
void ConfigStatus()
{
	printf("\n"
		"[Config Status]\n"
		"SkillID           ( %d )\n"
		"PlayerHP          ( %d )\n"
		"MedID             ( %d )\n"
		"MoveFast          ( %d )\n"
		"MoveSlow          ( %d )\n"
		"PickRange         ( %.2f )\n"
		"AttackRange       ( %.2f )\n"
		"MoveFastSleep     ( %d )\n"
		"MoveSlowSleep     ( %d )\n"
		"SleepSkill        ( %d )\n"
		"SlepAfterKill     ( %d )"
		"\nFailAttackLimit ( %d )"
		"\n\n"
		, gBotSettings.skillid, gmyPlayer.maxhp, gBotSettings.medID, gBotSettings.moveFastMultiplicator
		, gBotSettings.moveSlowMultiplicator, gBotSettings.pickrange, gBotSettings.attackrange
		, gBotSettings.movefastsleep, gBotSettings.moveslowsleep, gBotSettings.sleepbeforeskillattack, gBotSettings.sleepafterkill
		, gBotSettings.failAttackCounterLimit);
}