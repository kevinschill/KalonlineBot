#include "Header.h"

/*
Level 30

Sae Ryu second time talk
Send af -> d (1) :
[D-1: 16574]
*/
extern myPlayer gmyPlayer;
extern void(__cdecl* SendPacketInix)(BYTE type, LPCSTR format, ...);
extern BotSettings gBotSettings;
bool moveToCoordinates(POINT target)
{
	int32_t difx = gmyPlayer.x - target.x;
	int32_t dify = gmyPlayer.y - target.y;
	double range = (difx * difx + dify * dify) * 1.0;
	while (range >= 50.00)
	{
		if (range > 1000.00)
		{
			signed char tempx = -(difx / sqrt(range)) * gBotSettings.moveFastMultiplicator;
			signed char tempy = -(dify / sqrt(range)) * gBotSettings.moveFastMultiplicator;
			SendPacketInix(0x99, "bbb", tempx, tempy, 0);
		}
		if (range <= 1000.00 && range > 50.00)
		{
			signed char tempx = -(difx / sqrt(range)) * gBotSettings.moveSlowMultiplicator;
			signed char tempy = -(dify / sqrt(range)) * gBotSettings.moveSlowMultiplicator;
			SendPacketInix(0x99, "bbb", tempx, tempy, 0);
		}
		difx = gmyPlayer.x - target.x;
		dify = gmyPlayer.y - target.y;
		range = (difx * difx + dify * dify) * 1.0;
		std::this_thread::sleep_for(std::chrono::milliseconds(300));
	}
	std::cout << "Arrived at Spot" << std::endl;
}
namespace Level50JobChange
{
	void Part1();
	void Part2();
	void Part3();
	void Part4();
	void Part5();
	void Part6();
}
namespace Level30JobChange
{
	void Part2();
	void Part3();
	void Part4();
	void Part5();
	void Part6();
}
void Level30JobChange::Part2()
{
	POINT sae_ryue_coordinates = { 256479, 257062 };
	/*
	* Escort Index 6 + 7
	Talk -> move Escort Item 170 30 Amount
	move sae ryu -> talk
	move in pill part 3 
	*/
}
void Level30JobChange::Part3()
{
	// ID:16575 X:265577 Y:262004
	POINT in_pill_coordinates = { 265577,262004 };
	/* Big Hand 25 Index
	Talk -> in Pill -> move Big Handed 1 Head Index 171
	Talk -> in Pill -> move Sae Ryue
	*/
}
void Level30JobChange::Part4()
{
	POINT sae_ryue_coordinates = { 256479, 257062 };
	/* Scavener Index 10
	Talk sae Ryue -> Move Scavenger Item 172 amount 1
	move in Pill -> Talk in Pill
	*/
}
void Level30JobChange::Part5()
{
	//ID:16576 X:264924 Y:285906
	POINT hyungjoo_seunggong = { 264924 ,285906 };
	uint16_t TargetIndex[] = {27, 28};
	/*
		Move to Spot -> Big Handen Warriors  -> Item 173 Amount 40
		Talk to ->  HyungJoo 
		Talk again to -> Hyung
		move to Jin_pyung
	*/
}
void Level30JobChange::Part6()
{
	/*ID:16577 X:267622 Y:283253
	Talk to Jin_pyung
	*/
}
void Level50JobChange::Part1()
{
	POINT hyun_choong = { 267699,242298 };
	/*
	* ID:16639 X:267699 Y:242298
	* move to hyun_choong
		Talk to hyun_choong
	*/
}
void Level50JobChange::Part2()
{
	//ID:16640 X:268472 Y:243389
	POINT jin_haeryang = { 268472 ,243389 };
	/*
	move to jin_haeryang
	Collect 100 Sealed Talisman
	*/
}
// Send 80 -> bd (2) : [B - 1:1] [D - 2:16184] yok su
// Send 80->bd(2) :[B - 1:1] [D - 2:22650] geumoh