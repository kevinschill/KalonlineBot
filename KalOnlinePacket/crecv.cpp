#include "Header.h"
#include "recv.h"
CRecv::~CRecv()
{

}
bool CRecv::getDistance(POINT target,double range)
{
	int32_t diffx = mPlayer->coordinates.x - target.x;
	int32_t diffy = mPlayer->coordinates.y - target.y;
	float difference = sqrt((diffx * diffx + diffy * diffy) * 1.0f);
	if (difference < range)
		return true;
	
	return false;
}
void CRecv::FilterRecv(char* Packet)
{
	switch ((uint8_t)Packet[2])
	{

	}
}