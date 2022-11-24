#include "Header.h"
#include "recv.h"
#include <fstream>
void(__cdecl* SendPacketInix)(BYTE type, LPCSTR format, ...) = (void(__cdecl*)(BYTE, LPCSTR, ...))sendAdr;//0x007B77C0;//0x006BBFE0;


myPlayer gmyPlayer;
BotSettings gBotSettings;

extern bool endProgram;

bool getAttackedbot = false;
bool foundMob = false;
bool gettingAttacked = false;


uint32_t myPlayerID = 0;
uint32_t lastTargetID = 0;
uint64_t tickcounter = 0;
uint64_t lastmed = 0;
uint64_t lastSkill = 0;
uint32_t lastTargetBot = 0;
uint32_t failAttackCounter = 0;

/*
[RECV | 4f | 8]:
1b type
7c 00 min
9e 00 max
00

[RECV | 4f | 8]:
1c da 00 3d 01 00

[RECV | 4f | 6]:
10 00 00 08

[RECV | 4f | 8]:
0f
52 00
00 00
00

[RECV | 6e | 9]:
2c 33 98 08 3c 02 00

[RECV | 75 | 3]:
2c

[RECV | 4f | 8]:
08 f7 03 00 00 02

[RECV | 89 | 8]:
90 b9 c0 06 09 02

[RECV | c4 | 3]:
11

[RECV | 4f | 8]:
08 09 04 00 00 0c
*/
std::set<uint8_t> headerFilter({ 0x05,0x32,0x6A, 0x7A,0xA8,0x8B,0x2F,0x86,0x9B,0x8D,0x0F,0x77,0x0E,0xEB,0xC4,0x75,0x65,0xEF,0xED,0x67,0x9C,0x89,0x9D,0xB5,0x51,0x5D,0xCC,0xA0,0x00,0x37,0xF0,0xE7,0x91,0x68,0x31,0xB6,0x7B,0x78,0x71,0xE9,0x97,0x23,0x44,0x9A,0xC6,0x6E ,0x34 });

//std::set<uint16_t> blockMobsIndex({ });

std::map<uint32_t, Monster> Monstermap;
void DumpNPC(uint32_t id, uint32_t x, uint32_t y)
{
	std::ofstream myfile;
	myfile.open("npc.txt", std::ios::app);
	myfile << "ID:" << id << " X:" << x << " Y:" << y << "\n";
	myfile.close();
}
bool moveToPosition(POINT position)
{
	int32_t difx = gmyPlayer.x - position.x;
	int32_t dify = gmyPlayer.y - position.y;
	double range = (difx * difx + dify * dify) * 1.0;
	while (range >= 200.00)
	{
		if (range > 1000.00)
		{
			signed char tempx = -(difx / sqrt(range)) * gBotSettings.moveFastMultiplicator;
			signed char tempy = -(dify / sqrt(range)) * gBotSettings.moveFastMultiplicator;
			SendPacketInix(0xD6, "bbb", tempx, tempy, 0);
		}
		if (range <= 1000.00 && range > 200.00)
		{
			signed char tempx = -(difx / sqrt(range)) * gBotSettings.moveSlowMultiplicator;
			signed char tempy = -(dify / sqrt(range)) * gBotSettings.moveSlowMultiplicator;
			SendPacketInix(0x99, "bbb", tempx, tempy, 0);
		}
		difx = gmyPlayer.x - position.x;
		dify = gmyPlayer.y - position.y;
		range = (difx * difx + dify * dify) * 1.0;
		std::this_thread::sleep_for(std::chrono::milliseconds(200));
	}
	std::cout << "Arrived at Position ( " << position.x << " ) (" << position.y << " )" << std::endl;

	return true;
}
bool HplowExit()
{
	if (gmyPlayer.hp <= gBotSettings.lowhpexit)
	{
		std::cout << "[LOW HP EXIT] Moving to Exit Spot!!!!" << std::endl;
		int32_t difx = gmyPlayer.x - gBotSettings.exitPositionBot.x;
		int32_t dify = gmyPlayer.y - gBotSettings.exitPositionBot.y;
		double range = (difx * difx + dify * dify) * 1.0;
		while (range >= 200.00)
		{
			if (range > 1000.00)
			{
				signed char tempx = -(difx / sqrt(range)) * gBotSettings.moveFastMultiplicator;
				signed char tempy = -(dify / sqrt(range)) * gBotSettings.moveFastMultiplicator;
				SendPacketInix(0x99, "bbb", tempx, tempy, 0);
			}
			if (range <= 1000.00 && range > 200.00)
			{
				signed char tempx = -(difx / sqrt(range)) * gBotSettings.moveSlowMultiplicator;
				signed char tempy = -(dify / sqrt(range)) * gBotSettings.moveSlowMultiplicator;
				SendPacketInix(0x99, "bbb", tempx, tempy, 0);
			}
			difx = gmyPlayer.x - gBotSettings.exitPositionBot.x;
			dify = gmyPlayer.y - gBotSettings.exitPositionBot.y;
			range = (difx * difx + dify * dify) * 1.0;
			std::this_thread::sleep_for(std::chrono::milliseconds(200));
		}
		std::cout << "Arrived at Spot" << std::endl;

		return true;
	}
	else
		return false;

}
bool moveToStart = false;
uint32_t failcheckMobmap = 0;
uint32_t failcheckMoveSpot = 0;
bool notdead = true;
HANDLE thrdID = 0;
void AddGuiLog(std::string s)
{
	s.insert(0, "Log:");
	int sendret = send(gBotSettings.guiSocket, s.c_str(), s.size() + 1, 0);

}
unsigned long __stdcall BotThread(void* p)
{
	std::cout << "Bot Thread Started " << std::endl;
	while (notdead)
	{
		if (endProgram)
			return 1;

		// 		if ((gmyPlayer.hp < gmyPlayer.maxhp && GetTickCount64() - lastmed > 700))
		// 		{
		// 					SendPacketInix(0xAD, "d", gBotSettings.medID);
		//  					//gmyPlayer.hp += 300;
		//  					lastmed = GetTickCount64();
		// 		}
		if ((gBotSettings.botrunning) || (gBotSettings.moveBot))
		{
			// 			if (gBotSettings.medID != 0)
			// 			{
			// 				if ((gmyPlayer.hp < gmyPlayer.maxhp && GetTickCount64() - lastmed > 700))
			// 				{
			// 					SendPacketInix(0xAD, "d", gBotSettings.medID);
			// 					gmyPlayer.hp += 300;
			// 					lastmed = GetTickCount64();
			// 				}
			// 			}


			if (gBotSettings.moveBot)
			{
				if (gBotSettings.debugOutput)
					printf("[DEBUG] Entered moveBot Monstermap size: %d\n", Monstermap.size());

				if ((Monstermap.size() <= 0 && gBotSettings.startPositionBot.x > 0))
				{
					moveToStart = true;
					std::cout << "Move to Start Position" << std::endl;
					if (moveToPosition(gBotSettings.startPositionBot))
						moveToStart = false;
					failcheckMobmap++;
					if (failcheckMobmap >= 5)
					{
						POINT newx;
						newx.x = gBotSettings.startPositionBot.x + 1000;
						newx.y = gBotSettings.startPositionBot.y;
						if (moveToPosition(newx))
						{
							printf("Reached spot move away now move back\n");
							if (moveToPosition(gBotSettings.startPositionBot))
							{
								failcheckMobmap = 0;
								failcheckMoveSpot++;
								if (failcheckMoveSpot >= 3)
								{
									std::this_thread::sleep_for(std::chrono::seconds(60));
									failcheckMoveSpot = 0;
								}

							}

						}
					}
				}
				if (moveToStart == false)
				{


					std::pair<uint32_t, Monster> nearestMonster;
					float lastRange = 99999999990.00f;
					for (auto& mob : Monstermap)
					{
						int32_t difx = gmyPlayer.x - mob.second.x;
						int32_t dify = gmyPlayer.y - mob.second.y;
						float range = (difx * difx + dify * dify) * 1.0;
						if (range < lastRange)
						{
							lastRange = range;
							nearestMonster = mob;
						}
						if (gBotSettings.debugOutput)
							printf("[DEBUG] for auto Monstermap\n");
					}
					if (gBotSettings.debugOutput)
						printf("[DEBUG] After Monstermap loop\n");
					//auto it = std::min_element(std::begin(Monstermap), std::end(Monstermap),[](const auto& l, const auto& r) { return l.second.distance < r.second.distance; });
					if (nearestMonster.first != 0)
					{
						foundMob = true;

						while (foundMob)
						{
							failcheckMoveSpot = 0;
							if (gBotSettings.debugOutput)
								printf("[DEBUG] While foundMob\n");
							if (gBotSettings.moveBot)
							{
								// 								if (HplowExit())
								// 								{
								// 									while (gmyPlayer.hp < gmyPlayer.maxhp)
								// 									{
								// 										SendPacketInix(0xAD, "d", gBotSettings.medID);
								// 										std::this_thread::sleep_for(std::chrono::seconds(1));
								// 									}
								// 									if (gmyPlayer.hp >= gmyPlayer.maxhp)
								// 									{
								// 										std::cout << "Move back to Spot" << std::endl;
								// 										if (moveToPosition(gBotSettings.startPositionBot))
								// 											std::cout << "Spot Reached Continue Botting..." << std::endl;
								// 									}
								// 								}
								int32_t difx = gmyPlayer.x - Monstermap[nearestMonster.first].x;
								int32_t dify = gmyPlayer.y - Monstermap[nearestMonster.first].y;
								float range = (difx * difx + dify * dify) * 1.0;
								if (range > 1000000.00f)
								{
									Monstermap.erase(nearestMonster.first);
									foundMob = false;
								}
								if (gBotSettings.debugOutput)
									printf("[DEBUG] While Found => calc Range: %f\n", range);
								if ((range > gBotSettings.movefastrange && range < 1000000.00f))
								{
									if (gBotSettings.debugOutput)
										printf("[DEBUG] Range > 200.0f\n");
									signed char tempx = -(difx / sqrt(range)) * gBotSettings.moveFastMultiplicator;
									signed char tempy = -(dify / sqrt(range)) * gBotSettings.moveFastMultiplicator;
									SendPacketInix(0xD6, "bbb", tempx, tempy, 0);
									std::this_thread::sleep_for(std::chrono::milliseconds(gBotSettings.movefastsleep));
									std::stringstream msg;
									msg << "[Move Fast] [" << range << "]";
									AddGuiLog(msg.str());
									//printf("[Move Fast] (%d,%d) [%.2f]\r", tempx, tempy, range);
								}
								if (range > gBotSettings.attackrange && range <= gBotSettings.moveslowrange)
								{
									if (gBotSettings.debugOutput)
										printf("[DEBUG] Range < 2000.0f\n");

									signed char tempx = -(difx / sqrt(range)) * gBotSettings.moveSlowMultiplicator;
									signed char tempy = -(dify / sqrt(range)) * gBotSettings.moveSlowMultiplicator;
									SendPacketInix(0x99, "bbb", tempx, tempy, 0);
									std::this_thread::sleep_for(std::chrono::milliseconds(gBotSettings.moveslowsleep));
									std::stringstream msg;
									msg << "[Move Slow] [" << range << "]";
									AddGuiLog(msg.str());
									//printf("[Move Slow] (%d,%d) [%.2f]\r", tempx, tempy, range);
								}
								if (range <= gBotSettings.attackrange)
								{
									if (gBotSettings.debugOutput)
										printf("[DEBUG] range < 40.0f\n");
									while (Monstermap.contains(nearestMonster.first) && gBotSettings.moveBot)
									{
										// 										if (HplowExit())
										// 										{
										// 											while (gmyPlayer.hp < gmyPlayer.maxhp)
										// 											{
										// 												SendPacketInix(0xAD, "d", gBotSettings.medID);
										// 												std::this_thread::sleep_for(std::chrono::seconds(1));
										// 											}
										// 											if (gmyPlayer.hp >= gmyPlayer.maxhp)
										// 											{
										// 												std::cout << "Move back to Spot" << std::endl;
										// 												if (moveToPosition(gBotSettings.startPositionBot))
										// 													std::cout << "Spot Reached Continue Botting..." << std::endl;
										// 											}
										// 										}
										if (gBotSettings.debugOutput)
											printf("[DEBUG] While Attack\n");
										if (nearestMonster.second.hp < 100)
										{
											//SendPacketInix(0xA5, "d", nearestMonster.first);
											//printf("Open Woodenbox\n");
											//std::this_thread::sleep_for(std::chrono::seconds(8));
											Monstermap.erase(nearestMonster.first);

										}
										else
										{
											/*if (gBotSettings.medID != 0)
											{
												if ((gmyPlayer.hp < gmyPlayer.maxhp) && (GetTickCount64() - lastmed > 700))
												{
													SendPacketInix(0xAD, "d", gBotSettings.medID);
													gmyPlayer.hp += 300;
													lastmed = GetTickCount64();
												}
											}*/

											if (gBotSettings.debugOutput)
												printf("[DEBUG] Before Tickcount\n");
											if (GetTickCount64() - tickcounter > gBotSettings.delayAttack)
											{
												if (failAttackCounter >= gBotSettings.failAttackCounterLimit)
												{
													Monstermap.erase(nearestMonster.first);
													failAttackCounter = 0;
												}
												std::stringstream msg;
												msg << "[Attack[" << nearestMonster.second.index << "][" << range << "]";
												AddGuiLog(msg.str());
												//printf("[Attack[%d][%.2f]\n", nearestMonster.second.index, range);
												failAttackCounter++;
												lastTargetBot = nearestMonster.first;
												//SendPacketInix(0x29, "bdd", 1, it->first, num);
												//int range = 18300 - 17300 + 1;
												//int num = rand() % range + 17300;

												//std::this_thread::sleep_for(std::chrono::milliseconds(100));
												//SendPacketInix(0xB5, "d", nearestMonster.first);


												//std::this_thread::sleep_for(std::chrono::milliseconds(800));
												//SendPacketInix(0x65, "bd", gBotSettings.skillid, nearestMonster.first);


												//std::this_thread::sleep_for(std::chrono::milliseconds(gBotSettings.sleepbeforeskillattack));
												SendPacketInix(0xC7, "bbd", gBotSettings.skillid, 1, nearestMonster.first);



												tickcounter = GetTickCount64();
												std::this_thread::sleep_for(std::chrono::milliseconds(100));
												//SendPacketInix(0x16, "b", 0);
											}
										}
										std::this_thread::sleep_for(std::chrono::milliseconds(50));
									}
									std::this_thread::sleep_for(std::chrono::milliseconds(gBotSettings.sleepafterkill));
									foundMob = false;


								}
							}
							else
								foundMob = false;

						}

					}
					else
					{
						if (gBotSettings.debugOutput)
							printf("[DEBUG] Nearestmod.first => 0\n");
						foundMob = false;
						Monstermap.erase(nearestMonster.first);
						std::this_thread::sleep_for(std::chrono::milliseconds(50));
					}
				}
			}
			/*
			if ((gettingAttacked) && (getAttackedbot))
			{
				uint32_t range = 850 - 500 + 1;
				uint32_t num = rand() % range + 500;
				if (GetTickCount64() - tickcounter > num)
				{
					if (GetTickCount64() - lastSkill > 30000)
					{
						//if (yesOrNo(0.20f))
						//{
						//	SendPacketInix(0xC7, "bd", TricksterSkills::CrossSlash, lastTargetID);
						//}
						//else
						//{
							int range = 18300 - 17300 + 1;
							int num = rand() % range + 17300;
							SendPacketInix(0x29, "bdd", 1, lastTargetID, num);
							tickcounter = GetTickCount64();
						//}
						//lastSkill = GetTickCount64();
					}
					else
					{
						int range = 18300 - 17300 + 1;
						int num = rand() % range + 17300;
						SendPacketInix(0x29, "bdd", 1, lastTargetID, num);
						tickcounter = GetTickCount64();

					}
				}
			}
			*/

			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}
		else
			std::this_thread::sleep_for(std::chrono::milliseconds(100));

	}
	std::cout << "Bot Thread Ended " << std::endl;
	return 1;
}
uint64_t lastmedTicker = 0;
uint64_t lastAttackD1 = 0;
// void printTimeElapsed(
// 	std::chrono::steady_clock::time_point t0,
// 	std::chrono::steady_clock::time_point t1)
// {
// 	int64_t hh; // hour
// 	int64_t mm; // min
// 	int64_t ss; // sec
// 	int64_t ml; // millisec
// 	int64_t mc; // microsec
// 	int64_t ns; // nanosec
// 
// 	ns = std::chrono::duration_cast<std::chrono::nanoseconds>(t1 - t0).count();
// 	std::cout << ns << std::endl;
// 	mc = ns / 1000;
// 	ns %= 1000;
// 	ml = mc / 1000;
// 	mc %= 1000;
// 	ss = ml / 1000;
// 	ml %= 1000;
// 	mm = ss / 60;
// 	ss %= 60;
// 	hh = mm / 60;
// 	mm %= 60;
// 
// 	std::cout
// 		<< std::setfill('0') << std::setw(3) << hh << ":"
// 		<< std::setfill('0') << std::setw(2) << mm << ":"
// 		<< std::setfill('0') << std::setw(2) << ss << "."
// 		<< std::setfill('0') << std::setw(3) << ml << "."
// 		<< std::setfill('0') << std::setw(3) << mc << "."
// 		<< std::setfill('0') << std::setw(3) << ns << std::endl;
// 	return;
// }
// std::chrono::steady_clock::time_point t0;
// std::chrono::steady_clock::time_point t1;
void FilterRecv(char* Packet)
{
	//t0 = std::chrono::steady_clock::now();
	if (gBotSettings.sniffRecv)
	{
		WORD size;
		memcpy((void*)&size, (void*)((DWORD)Packet), 2);
		printf("[RECV | %02x | %d]: ", (BYTE)Packet[2], size);
		printf("\n");
		//char s[SIZE];
		//snprintf(s, SIZE, "%02X", Packet[i]);
		//std::string packetString;
		//packetString += "[RECV | " + std::to_string((BYTE)Packet[2]) + " | " + std::to_string(size) + "]\n";
		{
			int i;
			for (i = 3; i <= size; i++)
			{
				printf("%02x ", (BYTE)Packet[i]);

			}
			printf("\n\n");
		}
	}
	// 	else
	// 	{
	BYTE header = (BYTE)Packet[2];
	// 		if (!headerFilter.contains(header))
	// 		{
	switch (header)
	{
	case 0x89:
	{
		uint32_t id = *(uint32_t*)&Packet[3];
		uint8_t type = *(uint8_t*)&Packet[7];
		if ((id == myPlayerID && type == 9))
		{
			gBotSettings.moveBot = false;
			notdead = false;
			CloseHandle(thrdID);
		}
	}break;
	case S2C::StatChange:
	{
		
		/*
[RECV | 9a | 10]:
2c 33 98 08 20 2d 00 00
*/
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
		}
		if (type == CharacterStatChange::mana)
		{
			int16_t newmana = *(int16_t*)&Packet[4];
			gmyPlayer.mana = newmana;
		}
		// 				switch (type)
		// 				{
		// 				case CharacterStatChange::hp:
		// 				{
		// 					int16_t newhp = *(int16_t*)&Packet[4];
		// 					gmyPlayer.hp = newhp;
		// 					if ((newhp < gmyPlayer.maxhp && GetTickCount64() - lastmedTicker > 250))
		// 					{
		// 						SendPacketInix(0xAD, "d", gBotSettings.medID);
		// 						lastmedTicker = GetTickCount64();
		// 					}

		// 					// 					if (newhp <= gBotSettings.lowhpexit)
		// 					// 					{
		// 					// 						HplowExit();
		// 					// 					}
		// 
		// 				}break;
						// 				case CharacterStatChange::exp:
						// 				{
						// 					uint32_t expAdded = *(uint32_t*)&Packet[3];
						// 				    uint32_t expAdded2 = *(uint32_t*)&Packet[7];
						// 				   uint32_t expTotal = *(uint32_t*)&Packet[11];
						// 				   uint32_t expTotal2 = *(uint32_t*)&Packet[15];
						// 				}break;
						// 				case CharacterStatChange::mana:
						// 				{
						// 
						// 				}break;
						//}
	}break;
	case S2C::ItemDropped:
	{
		//PItemDrop item = (PItemDrop)Packet;
		
		//printf("[Item Drop] => ( %d ) Amount ( %d )\n", item->index, item->amount);
		WORD Index = *(WORD*)&Packet[3];
		if ((Index == 181 || Index == 31 || Index == 48 || Index == 2102 || Index == 2365 || Index == 2412 || Index == 2530
			|| Index == 2531 || Index == 2532 || Index == 2533 || Index == 2534 || Index == 2535 || Index == 2536 || Index > 5000))
		{
			DWORD x = *(DWORD*)&Packet[9];
			DWORD y = *(DWORD*)&Packet[13];
			int32_t diffx = gmyPlayer.x - x;
			int32_t diffy = gmyPlayer.y - y;
			float range = sqrt((diffx * diffx + diffy * diffy) * 1.0f);
			if (range < gBotSettings.pickrange)
			{

				DWORD iid = *(DWORD*)&Packet[5];
				DWORD amount = *(DWORD*)&Packet[21];

				SendPacketInix(C2S::PickItem, "dddb", iid, x / 32, y / 32, 0);
				if (Index == 31)
					gBotSettings.geonCollected += amount;
			}
		}

	}
	break;
	case S2C::SomeoneAttack:
	{
		//	PSomeoneAttack someoneattack = (PSomeoneAttack)Packet;
		DWORD attackerID = *(DWORD*)&Packet[3];
		DWORD targetID = *(DWORD*)&Packet[7];
		if (attackerID == myPlayerID)
		{
			lastTargetID = targetID;
			// 					WORD damage = *(WORD*)&Packet[11];
			// 					WORD damage2 = *(WORD*)&Packet[13];
			// 					if (Monstermap.contains(targetID))
			// 					{
			// 						Monstermap[targetID].hp -= damage + damage2;
			// 					}
		}
		if ((attackerID != myPlayerID && targetID == myPlayerID))
		{
			//	gettingAttacked = true;
			//	lastTargetID = attackerID;
			if (gBotSettings.d1Bot)
			{
				if (GetTickCount64() - lastAttackD1 > 250)
				{
					SendPacketInix(0xC7, "bbd", gBotSettings.skillid, 1, attackerID);
					lastAttackD1 = GetTickCount64();
				}
			}
			/*
			WORD damage = *(WORD*)&Packet[11];
			WORD damage2 = *(WORD*)&Packet[13];
			if ((damage + damage2) > 0)
			{
				gmyPlayer.hp -= damage + damage2;
				if (gBotSettings.medID != 0)
				{
					if ((gmyPlayer.hp < gmyPlayer.maxhp))
					{
						SendPacketInix(0xAD, "d", gBotSettings.medID);
						gmyPlayer.hp += 300;
						//lastmed = GetTickCount64();
					}
				}
				if (gmyPlayer.hp <= 0)
				{
					gBotSettings.moveBot = false;
				}

				*/
				//}
// 					if (Monstermap.contains(attackerID))
// 					{
// 						Monstermap[attackerID].hp -= damage + damage2;
// 					}
		}
	}
	break;
	case S2C::TeleportCoordinates:
	{
		uint32_t x = *(uint32_t*)&Packet[4];
		uint32_t y = *(uint32_t*)&Packet[8];
		gmyPlayer.x = x;
		gmyPlayer.y = y;
	}
	break;
	// 			case S2C::Notice:
	// 			{
	// 				uint8_t packetSize = *(uint8_t*)&Packet[0];
	// 				char notice[1000] = { 0 };
	// 				memcpy((void*)&notice, (void*)((DWORD)Packet + 3), packetSize - 3);
	// 				printf("[Notice] %s\n", notice);
	// 			}
	// 			break;
	case S2C::SomeoneDie:
	{
		//PSomeoneDie die = (PSomeoneDie)Packet;
		DWORD iid = *(DWORD*)&Packet[3];
		BYTE type = *(BYTE*)&Packet[8];
		if (gBotSettings.behadeall)
		{
			SendPacketInix(0xC7, "bbd", 1, 1, iid);
			failAttackCounter = 0;
		}
		else
		{
			if (gBotSettings.moveBot)
			{
				if (iid == lastTargetBot)
				{
					SendPacketInix(0xC7, "bbd", 1, 1, iid);
					Monstermap.erase(iid);
					failAttackCounter = 0;
					gBotSettings.mobskilled++;
				}
			}

			if (gBotSettings.botrunning)
			{
				if (iid == lastTargetID)
				{
					SendPacketInix(0xC7, "bbd", 1, 1, iid);
					gettingAttacked = false;
				}
			}
		}


		if (Monstermap.contains(iid))
		{
			Monstermap.erase(iid);
		}
	}
	break;
	case S2C::MonsterMove:
	{
		//PMonsterMove move = (PMonsterMove)Packet;
		uint32_t id = *(uint32_t*)&Packet[3];
		if (Monstermap.contains(id))
		{
			int8_t xstep = *(int8_t*)&Packet[7];
			int8_t ystep = *(int8_t*)&Packet[8];
			Monstermap[id].x += xstep;
			Monstermap[id].y += ystep;
		}
	}
	break;
	case S2C::MonsterMoveStop:
	{
		//PMonsterMove move = (PMonsterMove)Packet;
		uint32_t id = *(uint32_t*)&Packet[3];
		if (Monstermap.contains(id))
		{
			int8_t xstep = *(int8_t*)&Packet[7];
			int8_t ystep = *(int8_t*)&Packet[8];
			Monstermap[id].x += xstep;
			Monstermap[id].y += ystep;
		}
	}
	break;
	case S2C::PlayerAppear:
	{
		/*
		[RECV | 6b | 183]:
63 57 25 07 
6b 61 6c 6f 62 6c 61 
00 84 
b6 24 04 00 
54 af 03 00 
ec 31 00 00 
01 00 00 00 00 00 00 00 00 00 2b 1c 00 00 88 1c 95 1c a2 1c af 1c bc 1c 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 bc 0f 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 01 01 00 00 00 00 00 00 00 00 00 00 00 00 00 00 80 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 2e 0c 00 00 00 00 00 00 00 00 00 00 00 00 00
		*/
		if (myPlayerID == 0)
		{
			char name[255] = { 0 };
			memcpy((void*)&name, (void*)((DWORD)Packet + 7), 16);
			std::string sname(name);
			if ((sname == "kalobla") || (sname == "Bloodxsonikk"))
			{
				DWORD id = *(DWORD*)&Packet[3];
				myPlayerID = id;
				gmyPlayer.id = id;
				int aftername;
				aftername = strlen(name);
				memcpy((void*)&(gmyPlayer.x), (void*)((DWORD)Packet + 7 + aftername + 2), 4);
				memcpy((void*)&(gmyPlayer.y), (void*)((DWORD)Packet + 7 + aftername + 2 + 4), 4);
				memcpy((void*)&(gmyPlayer.z), (void*)((DWORD)Packet + 7 + aftername + 2 + 4 + 4), 4);
				std::string consoleTitle = "PlayerID: ";
				consoleTitle += std::to_string(myPlayerID);
				consoleTitle += "       Position: [ " + std::to_string(gmyPlayer.x) + " / " + std::to_string(gmyPlayer.y) + " / " + std::to_string(gmyPlayer.z) + " ]";
				consoleTitle += " Geon Collected[ " + std::to_string(gBotSettings.geonCollected) + " ]";
				SetConsoleTitleA(consoleTitle.c_str());
			}
		}

	}
	break;
	case S2C::MonsterAppear:
	{
		//PMonsterSpawn spawn = (PMonsterSpawn)Packet;
		uint16_t index = *(uint16_t*)&Packet[3];
		if (gBotSettings.blockMobsIndex.contains(index))
		{

		}
		else
		{
			if (gBotSettings.justAttackMobsMode)
			{
				if (gBotSettings.justAttackMobs.contains(index))
				{
					uint32_t id = *(uint32_t*)&Packet[5];
					if (!Monstermap.contains(id))
					{
						uint32_t x = *(uint32_t*)&Packet[9];
						uint32_t y = *(uint32_t*)&Packet[13];
						uint32_t z = *(uint32_t*)&Packet[17];
						uint16_t hp = *(uint16_t*)&Packet[23];

						Monster m;
						m.hp = hp;
						m.x = x;
						m.y = y;
						m.z = z;
						m.index = index;
						Monstermap[id] = m;
						if (gBotSettings.debugOutput)
							printf("[DEBUG] Mob Appear not in list %d\n", Monstermap.size());
					}
					else
					{
						uint32_t x = *(uint32_t*)&Packet[9];
						uint32_t y = *(uint32_t*)&Packet[13];
						uint32_t z = *(uint32_t*)&Packet[17];
						uint16_t hp = *(uint16_t*)&Packet[23];

						Monster m;
						m.hp = hp;
						m.x = x;
						m.y = y;
						m.z = z;
						m.index = index;
						Monstermap[id] = m;
						if (gBotSettings.debugOutput)
							printf("[DEBUG] Mob Appear already in list %d\n", Monstermap.size());
					}
				}
			}
			else
			{
				uint32_t id = *(uint32_t*)&Packet[5];
				if (!Monstermap.contains(id))
				{
					uint32_t x = *(uint32_t*)&Packet[9];
					uint32_t y = *(uint32_t*)&Packet[13];
					uint32_t z = *(uint32_t*)&Packet[17];
					uint16_t hp = *(uint16_t*)&Packet[23];

					Monster m;
					m.hp = hp;
					m.x = x;
					m.y = y;
					m.z = z;
					m.index = index;
					Monstermap[id] = m;
					if (gBotSettings.debugOutput)
						printf("[DEBUG] Mob Appear not in list %d\n", Monstermap.size());
				}
				else
				{
					uint32_t x = *(uint32_t*)&Packet[9];
					uint32_t y = *(uint32_t*)&Packet[13];
					uint32_t z = *(uint32_t*)&Packet[17];
					uint16_t hp = *(uint16_t*)&Packet[23];

					Monster m;
					m.hp = hp;
					m.x = x;
					m.y = y;
					m.z = z;
					m.index = index;
					Monstermap[id] = m;
					if (gBotSettings.debugOutput)
						printf("[DEBUG] Mob Appear already in list %d\n", Monstermap.size());
				}
			}
		}

	}
	break;
	// 			case 0x86:
	// 			{
	// 				uint16_t npcID = *(uint16_t*)&Packet[3];
	// 				uint32_t x = *(uint32_t*)&Packet[10];
	// 				uint32_t y = *(uint32_t*)&Packet[14];
	// 				DumpNPC(npcID, x, y);
	// 				/*
	// 				
	// 				NPC Appear ?
	// [RECV | 86 | 40]:
	// 76 42 id
	// 00 00 
	// d2 04 4b
	// 5d 17 04 00 x
	// 4b b8 03 00 y
	// a4 4c 00 00 z
	// b3 bf 00 00 00 00 00 00 00 00 00 00 00 00 01 00 00 00 00
	// 
	// 				*/
	// 			}
	// 				break;
	case S2C::MonsterDisappear:
	{
		//PMonsterDespawn despawn = (PMonsterDespawn)Packet;
		uint32_t id = *(uint32_t*)&Packet[3];
		if (Monstermap.contains(id))
			Monstermap.erase(id);
	}
	break;
	// 			case S2C::ItemDisappear: // Item Dissapear
	// 			{
	// 				//[RECV | 57 | 7]: 1f 08 12 04 9b
	// 				//DWORD iid = *(DWORD*)&Packet[3];
	// 				//BYTE type = *(BYTE*)&Packet[7];
	// 
	// 			}break;
	// 			case S2C::ConnectButtonAnswer: // connect button
	// 			{
	// 				//Sleep(2000);
	// 				//SendPacketInix(0x08, "sss", "bloodx69", "ptMoQP", "00-FF-03-A4-E4-18");
	// 				//[RECV | 7d | 43]: a7 cd 4b 19 3a 8b 78 cf 62 68 de e1 62 45 96 f7 27 00 04 00 00 00 22 00 00 03 12 02 38 8b 34 01 e4 7a e6 62 00 00 00 00 00
	// 			}
	// 			break;
	// 			case S2C::LoginAnswer: // login answer
	// 			{
	// 				//[RECV | 95 | 12]: 01 29 ba 00 00 00 00 00 00 68
	// 			}
	// 			break;
	// 			case S2C::LoadCharacterList: // load char list
	// 			{
	// 				//[RECV | 72 | 162]: 01 00 00 00 00 00 03 a7 93 21 00 42 6c 6f 6f 64 78 73 6f 6e 69 6b 6b 00 04 01 01 00 00 00 00 08 00 0a 00 19 00 0e 00 08 00 01 01 00 00 00 00 00 a6 93 21 00 42 6c 6f 6f 64 78 78 78 78 00 03 0b 3c 00 00 00 00 33 00 6e 00 08 00 0a 00 32 00 01 01 00 00 00 00 08 ce 00 cf 00 3f 25 30 25 31 25 32 25 33 25 34 25 a3 93 21 00 53 6f 6e 69 6b 6b 36 39 00 05 07 3c 00 00 00 00 32 00 55 00 08 00 08 00 5c 00 04 06 00 00 00 00 08 3c 06 a9 1f c9 1f ad 1f ac 1f bc 0f aa 1f ab 1f 00 00 00 00 00
	// 			}
	// 			break;
	// 
	// 			case S2C::ChatMessage: // chat message
	// 			{
	// 
	// 			}
	// 			break;
	// 
	// 			case S2C::ItemInventory:
	// 			{
	// 				/*
	// 				DWORD iid = *(DWORD*)&Packet[3];
	// 				DWORD amount = *(DWORD*)&Packet[7];
	// 				WORD type = *(WORD*)&Packet[11];
	// 				if (type == 11)
	// 				{
	// 					printf("Item[%d] Decreased to amount: %d\n", iid, amount);
	// 				}
	// 				if (type == 12)
	// 				{
	// 					printf("Item[%d] Increased to amount: %d\n", iid, amount);
	// 				}
	// 				*/
	// 			}
	// 			break;
	// 
	// 			case S2C::LoadGuildWindow: //load guild window list 
	// 			{
	// 				/*
	// 				[RECV | 41 | 411] :
	// 				01 02 7e 0f 00 63 00 08 01 00 02 7e 0f 00 4f 42 46 00 00 00 00 00 00 00 00 00 00 00 00 00 00 44 61 72 6b 48 6f 6c 79 4b 6e 69 67 68 74 00 00 00 00 00 00 00 08 72 08 24 ba 00 02 00 e6 0d 07 00 41 72 63 61 6e 65 00 00 00 00 00 00 00 00 00 00 00 44 6f 6d 69 6e 61 74 6f 72 00 00 00 00 00 00 00 00 00 00 00 00 08 79 ef 9e a1 00 03 00 8c 94 1c 00 4c 61 43 6f 6b 61 4e 6f 73 74 72 61 00 00 00 00 00 31 5f 4c 6f 78 69 4e 00 00 00 00 00 00 00 00 00 00 00 00 00 00 08 7d 72 d3 90 00 04 00 47 55 21 00 47 6c 6f 62 61 6c 55 6e 69 74 65 44 00 00 00 00 00 46 41 4b 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 08 73 f5 6c 75 00 05 00 3b 6e 12 00 58 58 58 00 00 00 00 00 00 00 00 00 00 00 00 00 00 52 6f 73 63 68 69 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 07 67 f1 58 63 00 06 00 1d 04 02 00 53 6f 6c 61 72 46 6f 72 45 76 65 72 00 00 00 00 00 6b 6c 61 75 73 66 72 6f 67 00 00 00 00 00 00 00 00 00 00 00 00 07 6c 3c da 62 00 07 00 4c df 1c 00 53 68 61 6f 6c 69 6e 46 69 67 68 74 65 72 00 00 00 53 68 65 6c 6c 79 43 6f 61 74 00 00 00 00 00 00 00 00 00 00 00 07 63 30 86 4b 00 08 00 da 87 1d 00 54 65 72 72 6f 72 53 71 75 61 64 00 00 00 00 00 00 5a 6c 6f 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 07 69 b7 02 44 00 00
	// 			[RECV | 41 | 5]:
	// 	00 01 7e
	// 			*/
	// 			}
	// 			break;
	// 
	// 			case S2C::ItemToInventory:// new item to inventory Bamboo Keg 98 geon
	// 				//[RECV | 85 | 70]: bd 01 d2 20 c8 83 00 00 00 00 00 01 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
	// 				break;
	// 
	// 			case S2C::LoadInventory:// load inventory
	// 			{
	// 				/*
	// 				[RECV | 7c | 943]:0e 00 3c 06 93 10 09 83 00 21 00 00 00 4d 98 01 00 20 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 a9 1f aa ac bc 83 00 01 00 00 00 01 00 00 00 18 18 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 c9 1f 90 d8 bd 83 00 01 00 00 00 01 00 00 00 28 28 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ad 1f 34 77 c2 83 00 01 00 00 00 01 00 00 00 14 14 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ac 1f bc eb c6 83 00 01 00 00 00 01 00 00 00 0c 0c 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 53 1d 7c 2b c7 83 00 80 00 00 04 44 01 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 bc 0f 7d 2b c7 83 00 81 00 00 04 01 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 98 fc 0f 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 1f 00 b0 2b c7 83 00 00 00 00 00 85 52 2f 01 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 20 0d 93 70 c7 83 00 00 00 00 00 60 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 aa 1f c7 7a c7 83 00 01 00 00 00 01 00 00 00 0e 0e 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 cb 12 80 91 c7 83 00 80 00 00 04 01 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
	// 				Index ?30 00
	// 				IID ? 33 92 c7 83 00 00 00 00 00
	// 				amount meds d4 07 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 bf 01 32 93 c7 83 00 00 00 00 00 14 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ab 1f ae 23 3d 00 00 01 00 00 00 01 00 00 00 0a 0a 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 01
	// 				*/
	// 			}
	// 			default:
	// 			{
	// // 				WORD size;
	// // 				memcpy((void*)&size, (void*)((DWORD)Packet), 2);
	// // 				printf("[RECV | %02x | %d]: ", (BYTE)Packet[2], size);
	// // 				printf("\n");
	// // 				{
	// // 					int i;
	// // 					for (i = 3; i <= size; i++)
	// // 					{
	// // 						printf("%02x ", (BYTE)Packet[i]);
	// // 					}
	// // 					printf("\n\n");
	// // 				}
	// 			}
	// 			break;
	}
	// 		}
	// 	}
// 	t1 = std::chrono::steady_clock::now();
// 	std::cout << "Filter:";
// 	printTimeElapsed(t0, t1);
}

uint32_t recvasmHOOKjmpAdr = recvAdr;
uint32_t recvasmHOOKjmpAdrBack = recvasmHOOKjmpAdr + 7;
int __declspec(naked) recvASM()
{
	__asm
	{
		push edx
		push ecx
		push ebx
		push eax

		mov eax, [ebp + 0x08]
		push eax
		call FilterRecv

		pop eax
		pop ebx
		pop ecx
		pop edx

		mov edx, [ebp + 0x08]
		movzx eax, byte ptr[edx + 2]
		mov ecx, recvasmHOOKjmpAdrBack

		jmp ecx
	}
}


bool Detour32(void* src, void* dst, int len)
{
	if (len < 5) return false;

	DWORD curProtection;
	VirtualProtect(src, len, PAGE_EXECUTE_READWRITE, &curProtection);

	memset(src, 0x90, len);

	uintptr_t relativeAddress = ((uintptr_t)dst - (uintptr_t)src) - 5;

	*(BYTE*)src = 0xE9;
	*(uintptr_t*)((uintptr_t)src + 1) = relativeAddress;

	DWORD temp;
	VirtualProtect(src, len, curProtection, &temp);

	return true; 
}


void InixSoft::RecvInit()
{
	Intercept(INST_JMP, (DWORD)recvasmHOOKjmpAdr, (DWORD)&recvASM, 5);
	thrdID = CreateThread(0, 0, LPTHREAD_START_ROUTINE(BotThread), 0, 0, 0);

}

/*
RECV Analyse

NPC Appear ?
[RECV | 86 | 40]:
76 42 00 00 id
d2 04 4b
5d 17 04 00 x
4b b8 03 00 y
a4 4c 00 00 z
b3 bf 00 00 00 00 00 00 00 00 00 00 00 00 01 00 00 00 00

NPC Dissappear
[RECV | 9b | 7]:
76 42 00 00 70


Add Buff Icon
RECV | 34 | 19]:
38 7f 8f 08 playerid
01 00 00 00
03 00 00 00
20 bf 02 00 remain time in milliseconds
4c ?



[RECV | 4f | 20]:
19
4c 71 10 00 exp1
00 00 00 00 exp2
9c 00 00 00 exp1added
00 00 00 00 00 exp2added
*/