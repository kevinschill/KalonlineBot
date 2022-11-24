#include "recv.h"

void CRecv::StatChange(char* Packet)
{
	uint8_t type = *(uint8_t*)&Packet[3];
	if (type == CharacterStatChange::hp)
	{
		int16_t hp = *(int16_t*)&Packet[4];
		mPlayer->currentHP = hp;

	}
	else if (type == CharacterStatChange::mana)
	{
		int16_t mana = *(int16_t*)&Packet[4];
		mPlayer->currentMana = mana;
	}
}

/*
uint8_t type = *(uint8_t*)&Packet[3];
				if (type == CharacterStatChange::hp)
				{
					int16_t newhp = *(int16_t*)&Packet[4];
					gmyPlayer.hp = newhp;
					if (gBotSettings.medID != 0)
					{
						if ((newhp < gmyPlayer.maxhp && GetTickCount64() - lastmedTicker > 250))
						{
							SendPacketInix(0xAD, "d", gBotSettings.medID);
							lastmedTicker = GetTickCount64();
						}
					}

					if (newhp == 0)
					{
						gBotSettings.moveBot = false;

					}
				}
*/