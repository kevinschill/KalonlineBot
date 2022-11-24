#include "recv.h"


void CRecv::ItemSpawn(char* Packet)
{
	PItemDrop item = (PItemDrop)Packet;
	uint16_t Index = item->index;
	if((mBotConfig->itemFilter.contains(Index) || Index >= mBotConfig->itemFitlerGreaterThen))
// 	if ((Index == 181 || Index == 31 || Index == 48 || Index == 2102 || Index == 2365 || Index == 2412 || Index == 2530
// 		|| Index == 2531 || Index == 2532 || Index == 2533 || Index == 2534 || Index == 2535 || Index == 2536 || Index > 5000))
	{
		if (getDistance(POINT(item->x,item->y), mBotSettings->pickrange))
		{
			SendPacketInix(C2S::PickItem, "dddb", item->iid, item->x / 32, item->y / 32, 0);
		}
	}
}