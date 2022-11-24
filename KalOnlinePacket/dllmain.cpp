#include "Header.h"
#include "Exports.h"
#include <map>
#include <processthreadsapi.h>
#include "detours.h"
#include <concrt.h>

bool endProgram = false;


extern std::map<uint32_t, Monster> Monstermap;
extern uint32_t myPlayerID;
extern myPlayer gmyPlayer;
extern void(__cdecl* SendPacketInix)(BYTE type, LPCSTR format, ...);
extern BotSettings gBotSettings;
extern unsigned long __stdcall BotThread(void* p);
extern HANDLE thrdID;
/*
Statue Forest Temp
*/
void MoveSpotThread()
{
	std::cout << "[Spot List]" << std::endl;
	std::cout << "1: COP" << std::endl;
	std::cout << "2: Temp" << std::endl;
	std::cout << "3: Devil Soldier" << std::endl;
	std::cout << "4: D1 Entrance" << std::endl;
	uint32_t spot = 0;
	std::cin >> spot;
	printf("Spot :%d\n", spot);
	uint32_t x = 0;
	uint32_t y = 0;
	switch (spot)
	{
		case 1:
		{
			x = 232802;
			y = 294675;
			std::cout << "Teleport to Cop" << std::endl;
			spot = 1;
		}
		break;
		case 2:
		{
			x = 232802;
			y = 228733;
			std::cout << "Teleport to Temp" << std::endl;
			spot = 2;
		}
		break;
		case 3:
		{
			x = 269589;
			y = 228733;
			std::cout << "Teleport to Devil Spot" << std::endl;
			spot = 3;
		}
		break;
		case 4:
		{
			x = 282575;
			y = 237180;
			std::cout << "Teleport to D1 Entrance" << std::endl;
			spot = 4;
		}
		break;
	}
	if (spot != 0)
	{
		int32_t difx = gmyPlayer.x - x;
		int32_t dify = gmyPlayer.y - y;
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
			difx = gmyPlayer.x - x;
			dify = gmyPlayer.y - y;
			range = (difx * difx + dify * dify) * 1.0;
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}
		std::cout << "Arrived at Spot" << std::endl;
	}

}
#include <Psapi.h>
HANDLE fileHandle;

void ReadString(char* output) {
	ULONG read = 0;
	int index = 0;
	do {
		ReadFile(fileHandle, output + index++, 1, &read, NULL);
	} while (read > 0 && *(output + index - 1) != 0);
}
void initup()
{
	const char* msg = "updatePlayer\r\n";
	WriteFile(fileHandle, msg, strlen(msg), nullptr, NULL);
}
void updatePlayerGui()
{

	myPlayer x;
	x.hp = 500;
	x.x = 232323;
	x.y = 244441;
	x.maxhp = 1000;
	x.mana = 600;
	x.maxmana = 1200;
	x.id = 13371337;

	WriteFile(fileHandle, &x, sizeof(x), nullptr, NULL);
}
void pipeRecv(std::string& s)
{
	std::cout << "String:" << s << std::endl;
	std::cout << "CStr" << s.c_str() << std::endl;
	WriteFile(fileHandle, s.c_str(), sizeof(s), nullptr, NULL);
}
extern bool notdead;
using namespace std;



int ServerMain()
{
	// Initialze winsock
	WSADATA wsData;
	WORD ver = MAKEWORD(2, 2);

	int wsOk = WSAStartup(ver, &wsData);
	if (wsOk != 0)
	{
		cerr << "Can't Initialize winsock! Quitting" << endl;
		return 99;
	}

	// Create a socket
	SOCKET listening = socket(AF_INET, SOCK_STREAM, 0);
	if (listening == INVALID_SOCKET)
	{
		cerr << "Can't create a socket! Quitting" << endl;
		return 99;
	}

	// Bind the ip address and port to a socket
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(54000);
	hint.sin_addr.S_un.S_addr = INADDR_ANY; // Could also use inet_pton .... 

	bind(listening, (sockaddr*)&hint, sizeof(hint));

	// Tell Winsock the socket is for listening 
	listen(listening, SOMAXCONN);

	// Create the master file descriptor set and zero it
	fd_set master;
	FD_ZERO(&master);

	// Add our first socket that we're interested in interacting with; the listening socket!
	// It's important that this socket is added for our server or else we won't 'hear' incoming
	// connections 
	FD_SET(listening, &master);

	// this will be changed by the \quit command (see below, bonus not in video!)
	bool running = true;

	while (running)
	{
		// Make a copy of the master file descriptor set, this is SUPER important because
		// the call to select() is _DESTRUCTIVE_. The copy only contains the sockets that
		// are accepting inbound connection requests OR messages. 

		// E.g. You have a server and it's master file descriptor set contains 5 items;
		// the listening socket and four clients. When you pass this set into select(), 
		// only the sockets that are interacting with the server are returned. Let's say
		// only one client is sending a message at that time. The contents of 'copy' will
		// be one socket. You will have LOST all the other sockets.

		// SO MAKE A COPY OF THE MASTER LIST TO PASS INTO select() !!!

		fd_set copy = master;

		// See who's talking to us
		int socketCount = select(0, &copy, nullptr, nullptr, nullptr);

		// Loop through all the current connections / potential connect
		for (int i = 0; i < socketCount; i++)
		{
			// Makes things easy for us doing this assignment
			SOCKET sock = copy.fd_array[i];

			// Is it an inbound communication?
			if (sock == listening)
			{
				// Accept a new connection
				SOCKET client = accept(listening, nullptr, nullptr);
				//printf("New Client: %d", client);
				// if (!gBotSettings.guiSocket)
				gBotSettings.guiSocket = client;
				// Add the new connection to the list of connected clients
				FD_SET(client, &master);

				// Send a welcome message to the connected client
				string welcomeMsg = "SERVER:Welcome to the Awesome Chat Server!";
				send(client, welcomeMsg.c_str(), welcomeMsg.size() + 1, 0);
			}
			else // It's an inbound message
			{
				char buf[4096];
				ZeroMemory(buf, 4096);

				// Receive message
				int bytesIn = recv(sock, buf, 4096, 0);
				if (bytesIn <= 0)
				{
					// Drop the client
					closesocket(sock);
					FD_CLR(sock, &master);
				}
				else
				{
					
					// Check to see if it's a command. \quit kills the server
					if (buf[0] == '\\')
					{
						// Is the command quit? 
						string cmd = string(buf, bytesIn);
						if (cmd == "\\quit")
						{
							running = false;
							break;
						}

						// Unknown command
						continue;
					}
					std::string bufString(buf,bytesIn);
					if (bufString.find("updatePlayer") != std::string::npos)
					{
						if(gBotSettings.guiSocket)
						for (u_int i = 0; i < master.fd_count; i++)
						{
							SOCKET outSock = master.fd_array[i];
							if (outSock == listening)
							{
								continue;
							}
							
							//printf("Send Struct\n");
							ostringstream ss;
							ss << "updatePlayer" << "\r\n";
							string strOut = ss.str();
							send(outSock, strOut.c_str(), strOut.size() + 1, 0);
							send(outSock, (const char*)&gmyPlayer, sizeof(myPlayer), 0);
							
						}

						
					}
					if (bufString == "justattackon")
					{
						gBotSettings.justAttackMobsMode = true;
					}
					if (bufString == "justattackoff")
					{
						gBotSettings.justAttackMobsMode = false;
					}
					if (bufString == "moveboton")
					{
						gBotSettings.moveBot = true;
					}
					if (bufString == "movebotoff")
					{
						gBotSettings.moveBot = false;
					}
					if (bufString.find("JustAttackMobs:") != std::string::npos)
					{
						std::stringstream ss(bufString.substr(15));
						gBotSettings.justAttackMobs.clear();
						for (int i; ss >> i;) {
							gBotSettings.justAttackMobs.insert(i);
							if (ss.peek() == ',')
								ss.ignore();
						}
						std::cout << ss.str() << std::endl;
					}
					if (bufString.find("Skill:") != std::string::npos)
					{
						uint32_t skillID = std::stoi(bufString.substr(6));
						gBotSettings.skillid = skillID;
					}
					// Send message to other clients, and definiately NOT the listening socket
/*
					for (u_int i = 0; i < master.fd_count; i++)
					{
						SOCKET outSock = master.fd_array[i];
						if (outSock == listening)
						{
							continue;
						}

						ostringstream ss;

						if (outSock != sock)
						{
							ss << "SOCKET #" << sock << ":" << buf << "\r\n";
						}
						else
						{
							ss << "ME:" << buf << "\r\n";
						}

						string strOut = ss.str();
						send(outSock, strOut.c_str(), strOut.size() + 1, 0);
					}*/
				}
			}
		}
	}

	// Remove the listening socket from the master file descriptor set and close it
	// to prevent anyone else trying to connect.
	FD_CLR(listening, &master);
	closesocket(listening);

	// Message to let users know what's happening.
	string msg = "SERVER:Server is shutting down. Goodbye\r\n";

	while (master.fd_count > 0)
	{
		// Get the socket number
		SOCKET sock = master.fd_array[0];

		// Send the goodbye message
		send(sock, msg.c_str(), msg.size() + 1, 0);

		// Remove it from the master file list and close the socket
		FD_CLR(sock, &master);
		closesocket(sock);
	}

	// Cleanup winsock
	WSACleanup();

	system("pause");
	return 0;
}
unsigned long __stdcall Control([[maybe_unused]] void* Param)
{
	while (true)
	{
		if (endProgram)
		{
			return 1;
		}
		std::string command;
		std::cin >> command;
		if (command == "update")
		{
			
			//initup();
			//updatePlayerGui();
		}
		if (command == "botthread")
		{
			notdead = true;
			thrdID = CreateThread(0, 0, LPTHREAD_START_ROUTINE(BotThread), 0, 0, 0);
		}
		if (command == "cargo")
		{
			SendPacketInix(0x80, "bd", 1, 16184);
			//Send 80->bd(2) : [B - 1:1] [D - 2:16184] yok su
		}
		if (command =="sniffrecv")
		{
			if (gBotSettings.sniffRecv)
				gBotSettings.sniffRecv = false;
			else
				gBotSettings.sniffRecv = true;
		}
		if (command =="sniffsend")
		{
			if (gBotSettings.sniffSend)
				gBotSettings.sniffSend = false;
			else
				gBotSettings.sniffSend = true;
		}
		if (command == "setspot")
		{
			gBotSettings.startPositionBot.x = gmyPlayer.x;
			gBotSettings.startPositionBot.y = gmyPlayer.y;
		}
		if (command == "setexit")
		{
			gBotSettings.exitPositionBot.x = gmyPlayer.x;
			gBotSettings.exitPositionBot.y = gmyPlayer.y;
		}
		if (command == "movexx")
		{
			MoveSpotThread();
		}
		if (command == "scroll")
		{
			uint32_t scrollid = 0;
			uint32_t amountuse = 0;
			std::cout << "ScrollID: ";
			std::cin >> scrollid;
			std::cout << "Amount: ";
			std::cin >> amountuse;
			for(uint32_t i = 0; i < amountuse; i++)
			{
				SendPacketInix(0xAD, "d", scrollid);
				std::this_thread::sleep_for(std::chrono::milliseconds(333));
				std::cout << "Left =>: " << amountuse - i;
			}
			std::cout << "WoW so much exp D=" << std::endl;
		}
		if (command == "portcop")
		{
// 			std::thread t(MoveSpotThread);
// 			t.detach();
// 			int32_t difx = gmyPlayer.x - 232802;
// 			int32_t dify = gmyPlayer.y - 294675;
// 			float range = (difx * difx + dify * dify) * 1.0;
// 			while (range >= 50.00f)
// 			{
// 				if (range > 1000.00f)
// 				{
// 					signed char tempx = -(difx / sqrt(range)) * gBotSettings.moveFastMultiplicator;
// 					signed char tempy = -(dify / sqrt(range)) * gBotSettings.moveFastMultiplicator;
// 					SendPacketInix(0x99, "bbb", tempx, tempy, 0);
// 				}
// 				if (range <= 1000.00f && range > 50.00f)
// 				{
// 					signed char tempx = -(difx / sqrt(range)) * gBotSettings.moveSlowMultiplicator;
// 					signed char tempy = -(dify / sqrt(range)) * gBotSettings.moveSlowMultiplicator;
// 					SendPacketInix(0x99, "bbb", tempx, tempy, 0);
// 				}
// 				difx = gmyPlayer.x - xZiel;
// 				dify = gmyPlayer.y - yZiel;
// 				range = (difx * difx + dify * dify) * 1.0;
// 				std::this_thread::sleep_for(std::chrono::milliseconds(200));
// 			}
// 			std::cout << "Arrived at COP" << std::endl;
		}
        if (command == "resetid")
        {
            myPlayerID = 0;
        }
        if (command == "cfg")
        {
            ReadConfig();
        }
		if (command == "cfgstatus")
		{
			ConfigStatus();
		}
		//DWORD OnMap = SearchPattern("55 8B EC 81 EC x x x x 83 3D x x x x x 0F 85 x x x x",0x00400000,0x007FFFFF);
        //devil soldier castle 269589 228733 17546
        if (command == "movetospot")
        {
            uint32_t xZiel = 0;
            std::cout << "X: ";
            std::cin >> xZiel;
			uint32_t yZiel = 0;
			std::cout << "Y: ";
            std::cin >> yZiel;

			int32_t difx = gmyPlayer.x -xZiel;
			int32_t dify = gmyPlayer.y - yZiel;
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
				difx = gmyPlayer.x - xZiel;
				dify = gmyPlayer.y - yZiel;
				range = (difx * difx + dify * dify) * 1.0;
                std::this_thread::sleep_for(std::chrono::milliseconds(300));
            }
            std::cout << "Arrived at Spot" << std::endl;
            

        }
		if (command == "coords")
		{
            uint32_t x = 0;
            uint32_t y = 0;
            uint32_t z = 0;
            std::cout << "X: ";
            std::cin >> x;
			std::cout << "Y: ";
			std::cin >> y;
			std::cout << "Z: ";
			std::cin >> z;
            gmyPlayer.x = x;
            gmyPlayer.y = y;
            gmyPlayer.z = z;
		}
        if (command == "l")
        {

        }
        if (command == "cleanbot")
        {
            Monstermap.clear();
        }
        if (command == "sethp")
        {
            uint16_t hp = 0;
            std::cout << "Player MAXHP: ";
            std::cin >> hp;
            gmyPlayer.maxhp = hp;
            gmyPlayer.hp = hp;
        }
		if (command == "debug")
		{
			if (gBotSettings.debugOutput)
				gBotSettings.debugOutput = false;
			else
				gBotSettings.debugOutput = true;
		}
		if (command == "scanSkill")
		{
            if (gBotSettings.scanSkills)
                gBotSettings.scanSkills = false;
            else
                gBotSettings.scanSkills = true;
		}
        if (command == "setskill")
        {
            uint8_t skillid = 0;
            std::cout << "SkillID: ";
            std::cin >> skillid;
            gBotSettings.skillid = skillid;
        }
        if (command == "setmed")
        {
            gBotSettings.setMed = true;
        }
		if (command == "setmedid")
		{
			uint32_t medid = 0;
			std::cout << "MedID: ";
			std::cin >> medid;
			gBotSettings.medID = medid;
		}
        if (command == "boton")
        {
            gBotSettings.botrunning = true;
        }
		if (command == "botoff")
		{
			gBotSettings.botrunning = false;
		}
        if (command == "movebot")
        {
            gBotSettings.moveBot = true;
        }
		if (command == "movebotoff")
		{
			gBotSettings.moveBot = false;
		}
        if (command == "justattack")
        {
			if (gBotSettings.justAttackMobsMode)
				gBotSettings.justAttackMobsMode = false;
			else
				gBotSettings.justAttackMobsMode = true;
        }
		if (command == "hide")
		{
			auto testx = FindWindowA(0, "KalOnline");
			ShowWindow((HWND)testx, SW_HIDE);
		}
		if (command == "show")
		{
			auto testx = FindWindowA(0, "KalOnline");
			ShowWindow((HWND)testx, SW_SHOW);
		}
		if (command == "d1bot")
		{
			if (gBotSettings.d1Bot)
				gBotSettings.d1Bot = false;
			else
			{
				gBotSettings.d1Bot = true;
				std::thread d1bott(D1BotThread);
				d1bott.detach();
			}

		}
		if (command == "og")
		{
			for(int i = 0; i < 70; i++)
				SendPacketInix(0x99, "bbb", 0, 0, 34);
		}
		if (command == "ug")
		{
			for (int i = 0; i < 70; i++)
				SendPacketInix(0x99, "bbb", 0, 0, -34);
		}
// 		if (GetAsyncKeyState(VK_F1) & 0x8000)
// 		{
// 		}
// 
// 		else if (GetAsyncKeyState(VK_F2) & 0x8000)
// 		{
// 
// 		}
//         
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
	}

	return 1;
}



// 			DWORD add = *(DWORD*)0x12B83F8;
// 			//printf("xx - %p\n", add);
// 			auto PositionX = *(int*)(add + 18192);
// 			auto PositionZ = *(int*)(add + 18196);
// 			auto PositionY = *(int*)(add + 18200);

typedef SHORT(__stdcall* pGetAsyncKeyState)(int vKey);
pGetAsyncKeyState Org_GetAsync = nullptr;
typedef HWND(WINAPI* pGetConsoleWindow)(void);
pGetConsoleWindow  Org_GetCnsoleWindow = nullptr;
SHORT __stdcall myGetAsyncKeyState(int vKey)
{
	printf("Asc Hook Key: %d\n", vKey);
	return Org_GetAsync(vKey);
}
void keyhook()
{

	*(FARPROC*)&Org_GetAsync = GetProcAddress(GetModuleHandleA("user32.dll"), "GetAsyncKeyState");
	Intercept(INST_JMP, (DWORD)Org_GetAsync, (DWORD)&myGetAsyncKeyState, 5);
}

void ReadAdress()
{
	printf("\n#########################################################################\n");
	printf("                   [ Reading Up to date Address ]\n");
	MODULEINFO modInfo;
	HANDLE hProcess = GetCurrentProcess();

	uintptr_t engine_module = (uintptr_t)GetModuleHandleA("engine.exe");
	if (engine_module != NULL)
		GetModuleInformation(hProcess, (HMODULE)engine_module, &modInfo, sizeof(MODULEINFO));

	PBYTE recvAddy = FindSignature((LPVOID)engine_module, modInfo.SizeOfImage, (PCHAR)"\x8B\x55\x08\x0F\xB6\x42\x02\x89\x85\x00\x00\x00\x00\x81\xBD\x00\x00\x00\x00\x00\x00\x00\x00\x0F\x87\x00\x00\x00\x00\x8B\x8D\x00\x00\x00\x00\x0F\xB6\x91\x00\x00\x00\x00\xFF\x24\x95\x00\x00\x00\x00", (PCHAR)"xxxxxxxxx????xx????????xx????xx????xxx????xxx????");
	printf("RECV [0x%p]\n", recvAddy);
	PBYTE sendAddy = FindSignature((LPVOID)engine_module, modInfo.SizeOfImage, (PCHAR)"\x55\x8B\xEC\x83\xEC\x74\xA1\x00\x00\x00\x00\x33\xC5\x89\x45\xFC\x83\x3D\x00\x00\x00\x00\x00\x74\x07\x33\xC0\xE9\x00\x00\x00\x00", (PCHAR)"xxxxxxx????xxxxxxx?????xxxxx????");
	printf("SEND [0x%p]\n", sendAddy);

	PBYTE CharacterMasterFunc = FindSignature((LPVOID)engine_module, modInfo.SizeOfImage, (PCHAR)"\x83\x3D\x00\x00\x00\x00\x00\x74\x1D\xA1\x00\x00\x00\x00\x8B\x88\x00\x00\x00\x00\x51\x6A\x00\x68\x00\x00\x00\x00\x6A\x7D\xE8\x00\x00\x00\x00\x83\xC4\x10", (PCHAR)"xx?????xxx????xx????xxxx????xxx????xxx");
	DWORD CharacterMasterPointer_ = 0;
	MemcpyEx((DWORD)&CharacterMasterPointer_, (DWORD)CharacterMasterFunc + 2, 4);
	printf("CharacterMasterPointer [0x%p]\n", CharacterMasterPointer_);
	printf("\n#########################################################################\n\n");
	// Target ? 128FECC
	// Target Char + 18480
}
unsigned long __stdcall MainThread([[maybe_unused]] void* Param)
{
    std::this_thread::sleep_for(std::chrono::seconds(5));


    CreateConsole();

    ReadConfig();

	ReadAdress();
	keyhook();
	InixSoft::RecvInit();
	InixSoft::InitSend();


	std::thread t(ServerMain);
	t.detach();
	CreateThread(0, 0, LPTHREAD_START_ROUTINE(Control), 0, 0, 0);
	
    while (true)
    {
		if (endProgram)
		{
			return 1;
		}
        std::this_thread::sleep_for(std::chrono::seconds(1));
        if (myPlayerID != 0)
        {
			//Point3 coordinates = getCoordinates();
			std::string consoleTitle = "PlayerID: ";
			consoleTitle += std::to_string(myPlayerID);
			consoleTitle += "   HP: [" + std::to_string(gmyPlayer.hp) + "/" + std::to_string(gmyPlayer.maxhp) + "]";
			consoleTitle += "   Position: [ " + std::to_string(gmyPlayer.x) + " / " + std::to_string(gmyPlayer.y) + " / " + std::to_string(gmyPlayer.z) + " ]";
			consoleTitle += "   Geon Collected[ " + std::to_string(gBotSettings.geonCollected) + " ]";
            consoleTitle += "   JustAttack [" + std::to_string(gBotSettings.justAttackMobsMode) + " ]";
			consoleTitle += "   Mobs Killed [" + std::to_string(gBotSettings.mobskilled) +" ]";
            SetConsoleTitleA(consoleTitle.c_str());
        }
		//printStatus();

    }
	

    return 1;
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    {
		Exports->Initialize();

        CreateThread(0, 0, LPTHREAD_START_ROUTINE(MainThread), 0, 0, 0);
    }
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
