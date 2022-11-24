#include "Header.h"

/*
X: 52037
Y: 34455
Z: 19993

*/
/*
51535
34082
19993
*/
extern BotSettings gBotSettings;
std::set<uint32_t> d1Monsters;
extern myPlayer gmyPlayer;
uint64_t lastmedxx = 0;
extern void(__cdecl* SendPacketInix)(BYTE type, LPCSTR format, ...);
void D1BotThread()
{
	gBotSettings.startPositionBot.x = 52037;
	gBotSettings.startPositionBot.y = 34455;
	gBotSettings.exitPositionBot.x = 51535;
	gBotSettings.exitPositionBot.y = 34082;

	while (gBotSettings.d1Bot)
	{
		if (HplowExit())
		{
			while (gmyPlayer.hp < gmyPlayer.maxhp)
			{
				SendPacketInix(0xAD, "d", gBotSettings.medID);
				std::this_thread::sleep_for(std::chrono::seconds(1));
			}
			if (gmyPlayer.hp >= gmyPlayer.maxhp)
			{
				std::cout << "Move back to Spot" << std::endl;
				if (moveToPosition(gBotSettings.startPositionBot))
					std::cout << "Spot Reached Continue Botting..." << std::endl;
			}
		}
		if ((gmyPlayer.hp < gmyPlayer.maxhp && GetTickCount64() - lastmedxx > 300))
		{
			SendPacketInix(0xAD, "d", gBotSettings.medID);
			lastmedxx = GetTickCount64();
		}
			
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
	}
}