#include "Header.h"


BYTE Byte;
WORD Word;
char* some;
DWORD ss = 0;
DWORD sb = 0;
DWORD Dword = 0;
DWORD Dword2 = 0;

/*
Send 69 -> bbdd (4) :
[B-1: 20] [B-2: 2] [D-3: 106] [D-4: -2084005877]
*/

uint32_t sendasmHOOKjmpAdr = sendAdr;//0x007B77C0;//0x006BBFE0;
uint32_t sendasmHOOKjmpAdrBack = sendasmHOOKjmpAdr + 6;//0x07;//0x09;
extern myPlayer gmyPlayer;
extern BotSettings gBotSettings;
int __declspec (naked) SendSniffer(BYTE type, LPCSTR format, ...)
{
	__asm
	{

		push    ebp
		mov     ebp, esp
		sub     esp, 116
	}
	if (gBotSettings.sniffSend)
	{
		va_list args;

		printf("Send %.2x -> %s (%d) : ", type, format, strlen(format));
		printf("\n");



		va_start(args, format);
		for (ss = 0; ss < strlen(format); ss++)
		{

			switch (format[ss])
			{
			case 'b': //BYTE
				Byte = va_arg(args, BYTE);
				printf("[B-%d: %d] ", ss + 1, Byte);
				break;
			case 'd': //DWORD
				Dword = (DWORD)va_arg(args, DWORD);
				printf("[D-%d: %d] ", ss + 1, Dword);

				break;
			case 'w': //WORD
				printf("[W-%d: %d] ", ss + 1, (WORD)va_arg(args, DWORD));
				break;
			case 's': //STRING
				some = va_arg(args, char*);
				printf("[S-%d: %s] ", ss + 1, some);
				//printf(" %d: %s\n",i+1,something);// wird ja garnet ausgegebn
				break;
			case 'm':
				printf(" [M-%d: %d] ", ss + 1, (DWORD)va_arg(args, DWORD));
				break;

				break;
			}
		}
		printf("\n\n");
		va_end(args);
	}
// 	else
// 	{
		switch (type)
		{
		case 0xD6:
		{
			va_list args;
			va_start(args, format);
			for (ss = 0; ss < strlen(format); ss++)
			{
				switch (format[ss])
				{
				case 'b': //BYTE
					//Byte = (signed char)va_arg(args, BYTE);
					if (ss + 1 == 1)
						gmyPlayer.x += (signed char)va_arg(args, signed char);
					if (ss + 1 == 2)
						gmyPlayer.y += (signed char)va_arg(args, signed char);
					if (ss + 1 == 3)
						gmyPlayer.z += (signed char)va_arg(args, signed char);
					break;
				case 'd': //DWORD
					Dword = (DWORD)va_arg(args, DWORD);
					break;
				case 'w': //WORD
					break;
				case 's': //STRING
					some = va_arg(args, char*);
					break;
				case 'm':
					break;

					break;
				}
			}
			va_end(args);
		}break;
		case 0x99:
		{
			va_list args;
			va_start(args, format);
			for (ss = 0; ss < strlen(format); ss++)
			{
				switch (format[ss])
				{
				case 'b': //BYTE
					if (ss + 1 == 1)
						gmyPlayer.x += (signed char)va_arg(args, signed char);
					if (ss + 1 == 2)
						gmyPlayer.y += (signed char)va_arg(args, signed char);
					if (ss + 1 == 3)
						gmyPlayer.z += (signed char)va_arg(args, signed char);
					break;
				case 'd': //DWORD
					Dword = (DWORD)va_arg(args, DWORD);
					break;
				case 'w': //WORD
					break;
				case 's': //STRING
					some = va_arg(args, char*);
					break;
				case 'm':
					break;

					break;
				}
			}
			va_end(args);
		}break;
			/*
		case 0x72: // drop Item d = id d = amount
		{
			if (gBotSettings.setMed)
			{
				va_list args;


				va_start(args, format);
				for (ss = 0; ss < strlen(format); ss++)
				{
					switch (format[ss])
					{
					case 'd': //DWORD
						if (ss + 1 == 1)
						{
							gBotSettings.medID = (DWORD)va_arg(args, DWORD);
							gBotSettings.setMed = false;
						}

						break;
					}
				}
				va_end(args);
			}
		}
		break;
		case 0xC7: // use Skill bd id - mobid
		{
			if (gBotSettings.scanSkills)
			{
				va_list args;

				printf("Send %.2x -> %s (%d) : ", type, format, strlen(format));
				printf("\n");



				va_start(args, format);
				for (ss = 0; ss < strlen(format); ss++)
				{

					switch (format[ss])
					{
					case 'b': //BYTE
						Byte = va_arg(args, BYTE);
						printf("[B-%d: %d] ", ss + 1, Byte);
						break;
					case 'd': //DWORD
						Dword = (DWORD)va_arg(args, DWORD);
						printf("[D-%d: %d] ", ss + 1, Dword);

						break;
					case 'w': //WORD
						printf("[W-%d: %d] ", ss + 1, (WORD)va_arg(args, DWORD));
						break;
					case 's': //STRING
						some = va_arg(args, char*);
						printf("[S-%d: %s] ", ss + 1, some);
						//printf(" %d: %s\n",i+1,something);// wird ja garnet ausgegebn
						break;
					case 'm':
						printf(" [M-%d: %d] ", ss + 1, (DWORD)va_arg(args, DWORD));
						break;

						break;
					}
				}
				printf("\n\n");
				va_end(args);
			}

		}
		break;
		case 0x3A://open mail b 2
			break;
		case 0x59: // Syncro packet ?
			break;
		case 0x19: // dbd npc id 0 48 ? open Armor Shop 
			//48 = Thief 17 = Knight 18 = Archer
			break;
		case 0x7F:// buy item m / item id ?
			break;
		case 0xAf: // talk to npc shop ? d npc id
			break;
		case 0x34: // Enter Shop d npc id
			break;
		case 0x7D: // open player shop bd 0 id
			break;
		case 0xB5:// focus target d = id
			break;
		case 0x2B:// chat message s
			break;
		case 0x09:// connect button click
			break;
		case 0x04: // unk ?
		// Send 04 -> bd (2) :[B - 1:1] [D - 2:12094]
			break;
		case 0x08://login sss
			break;
		case 0x0A:// secondpw bsd
			break;
		case 0x00: // unkn ?
			break;
		case 0x29: // attack
		{

		}break;
		case 0xba: // pick
		{

		}break;
		case 0x02: // ping ?
		{

		}
		break;
		case 0x0d: // ping ?
		{

		}
		break;
		case 0x05:// dd 22000483 / 0
			break;
		case 0x32: // action? setzen
		//{

		//}
			break;
		case 0xAC:// b 2 & 3
		// Send ac -> b (1) :[B - 1:2]
			break;
		case 0xad: // use item 
		{
			if (gBotSettings.medID == 0)
			{
				va_list args;

				printf("Send %.2x -> %s (%d) : ", type, format, strlen(format));
				printf("\n");



				va_start(args, format);
				for (ss = 0; ss < strlen(format); ss++)
				{

					switch (format[ss])
					{
					case 'b': //BYTE
						Byte = va_arg(args, BYTE);
						printf("[B-%d: %d] ", ss + 1, Byte);
						break;
					case 'd': //DWORD
						Dword = (DWORD)va_arg(args, DWORD);
						printf("[D-%d: %d] ", ss + 1, Dword);

						break;
					case 'w': //WORD
						printf("[W-%d: %d] ", ss + 1, (WORD)va_arg(args, DWORD));
						break;
					case 's': //STRING
						some = va_arg(args, char*);
						printf("[S-%d: %s] ", ss + 1, some);
						//printf(" %d: %s\n",i+1,something);// wird ja garnet ausgegebn
						break;
					case 'm':
						printf(" [M-%d: %d] ", ss + 1, (DWORD)va_arg(args, DWORD));
						break;

						break;
					}
				}
				printf("\n\n");
				va_end(args);
			}
		}
		break;
		case 0x3F: // byte - new skill learn
			break;
			//case 0xA7:Send a7 -> bbbbb (5) :[B - 1:0] [B - 2:30] [B - 3:0] [B - 4:0] [B - 5:0] update stats
			//break;
		case 0x06: // create char
			/*
			Send 06 -> sbwwwwwbb (9) :
	[S-1: Bloppx] [B-2: 4] [W-3: 0] [W-4: 5] [W-5: 0] [W-6: 0] [W-7: 0] [B-8: 1] [B-9: 1]
			/
			break;
		case 0x48:// open guild window ? b 13 ## 2 => create guild window
			break;
		case 0x53:// send quest packet ? dw quest id - npc id ? 
			break;
		case 0x80:// Temp Port ? bd - 1 19630
			break;
		case 0x39:// logout ?
			break;
		case 0xC3:// bb 20 / 0
		// Send c3 -> bb (2) :[B - 1:20] [B - 2:0]
			break;
		case 0x8E:// b 0
			break;
		case 0x0E:// bd 1 / z ?
		// Send 0e -> bd (2) :[B - 1:1][D - 2:18110]
			break;
		case 0x65: // passive skill ?
		//{

		//}
			break;
		case 0xA5:// open wooden box etc ? d id
			break;
		case 0xD6:
		{
			va_list args;
			va_start(args, format);
			for (ss = 0; ss < strlen(format); ss++)
			{
				switch (format[ss])
				{
				case 'b': //BYTE
					//Byte = (signed char)va_arg(args, BYTE);
					if (ss + 1 == 1)
						gmyPlayer.x += (signed char)va_arg(args, signed char);
					if (ss + 1 == 2)
						gmyPlayer.y += (signed char)va_arg(args, signed char);
					if (ss + 1 == 3)
						gmyPlayer.z += (signed char)va_arg(args, signed char);
					break;
				case 'd': //DWORD
					Dword = (DWORD)va_arg(args, DWORD);
					break;
				case 'w': //WORD
					break;
				case 's': //STRING
					some = va_arg(args, char*);
					break;
				case 'm':
					break;

					break;
				}
			}
			va_end(args);
		}break;
		case 0x99:
		{
			va_list args;
			va_start(args, format);
			for (ss = 0; ss < strlen(format); ss++)
			{
				switch (format[ss])
				{
				case 'b': //BYTE
					if (ss + 1 == 1)
						gmyPlayer.x += (signed char)va_arg(args, signed char);
					if (ss + 1 == 2)
						gmyPlayer.y += (signed char)va_arg(args, signed char);
					if (ss + 1 == 3)
						gmyPlayer.z += (signed char)va_arg(args, signed char);
					break;
				case 'd': //DWORD
					Dword = (DWORD)va_arg(args, DWORD);
					break;
				case 'w': //WORD
					break;
				case 's': //STRING
					some = va_arg(args, char*);
					break;
				case 'm':
					break;

					break;
				}
			}
			va_end(args);
		}break;
		default:
		{
			va_list args;

			printf("Send %.2x -> %s (%d) : ", type, format, strlen(format));
			printf("\n");



			va_start(args, format);
			for (ss = 0; ss < strlen(format); ss++)
			{

				switch (format[ss])
				{
				case 'b': //BYTE
					Byte = va_arg(args, BYTE);
					printf("[B-%d: %d] ", ss + 1, Byte);
					break;
				case 'd': //DWORD
					Dword = (DWORD)va_arg(args, DWORD);
					printf("[D-%d: %d] ", ss + 1, Dword);

					break;
				case 'w': //WORD
					printf("[W-%d: %d] ", ss + 1, (WORD)va_arg(args, DWORD));
					break;
				case 's': //STRING
					some = va_arg(args, char*);
					printf("[S-%d: %s] ", ss + 1, some);
					//printf(" %d: %s\n",i+1,something);// wird ja garnet ausgegebn
					break;
				case 'm':
					printf(" [M-%d: %d] ", ss + 1, (DWORD)va_arg(args, DWORD));
					break;

					break;
				}
			}
			printf("\n\n");
			va_end(args);
		}
		break;*/
	//	}
	}


	__asm
	{
		jmp sendasmHOOKjmpAdrBack
	}



}

void InixSoft::InitSend()
{
// 	sendasmHOOKjmpAdrBack = (DWORD)(sendasmHOOKjmpAdr + 0x09);
// 	while (true)
// 	{
 		Intercept(INST_JMP, (DWORD)sendasmHOOKjmpAdr, (DWORD)SendSniffer, 5);
// 		std::this_thread::sleep_for(std::chrono::milliseconds(10));
// 	}
	
	
}