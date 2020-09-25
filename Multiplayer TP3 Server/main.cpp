#include <iostream>
#include <WS2tcpip.h>
#include <vector>
#include <random>
#include "TicTacToe.h"
#include "Player.h"
#define PLAYER_AMOUNT 10

#pragma comment (lib, "ws2_32.lib")

using namespace std;

struct message
{
	char cmd;
	char data[255];
	/*
	0 = ?
	1 = alias
	2 = text
	3 = play
	4 = gameText
	*/
};


void SetElement(int row, int col, int value, char array[])
{
	array[3 * row + col] = value;
}

int charToInt(char ch) {
	return ((int)ch - 48);
}

void Mem0Msg(message _msg)
{
	memset((char*)&_msg, 0, sizeof(_msg));
}

static random_device rd;
static mt19937_64 rng(rd());

int random_int(int initial, int last)
{
	uniform_int_distribution<int> distribution(initial, last);
	return distribution(rng);
}

int main()
{
	//Game	
	vector<TicTacToe*> games;
	int random;

	//Variables
	message customMessage;
	char auxArray[9];
	vector<Player*> playerV;
	sockaddr_in msgSender;
	bool validCoords;
	validCoords = false;
	char check;

	// iniciar winsock
	WSADATA data;
	WORD ver = MAKEWORD(2, 2);
	int wsOk = WSAStartup(ver, &data);
	int playerInWaiting = 0;
	if (wsOk != 0)
	{
		cerr << "No pudo inciar winsock" << endl;
		return -1;
	}

	//Server Msgs
	char welcomeMsg[255] = "Welcome to TicTacToe, you will join a game soon";
	char waitingForOponentMsg[255] = "Waiting for oponent";
	char gameStartingMsg[255] = "Game Starting";
	char coordsMsg[255] = "Please input your coords based on the grid shown above";
	char youWin[255] = "You Win! Play Again? (Y/N)";
	char youLose[255] = "You Lose! Play Again? (Y/N)";
	char thanksForPlaying[255] = "ThanksForPlaying. Have a great day!";

	// Game
	char ficha;
	int ubicacion1;
	int ubicacion2;

	// crear listening socket
	SOCKET listening = socket(AF_INET, SOCK_DGRAM, 0);
	if (listening == INVALID_SOCKET)
	{
		cerr << "Invalid socket" << endl;
		return -1;
	}

	// bind socket to adress
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(8900); // puerto en el que vamos a escuchar	
	hint.sin_addr.S_un.S_addr = ADDR_ANY;

	int bindResult = bind(listening, (sockaddr*)&hint, sizeof(hint));
	if (bindResult == SOCKET_ERROR)
	{
		cerr << "No pudo hacer el bind" << endl;
		return -1;
	}

	// recibir data del socket y procesarla (proceso bloqueante)
	char buf[1024];

	// estructura con la data del cliente que nos esta enviando mensajes
	sockaddr_in client;
	int clientSize = sizeof(client);
	char clientName[16];
	memset((char*)&customMessage, 0, sizeof(customMessage));
	memset(&client, 0, sizeof(client));
	memset(clientName, 0, sizeof(clientName));
	memset(&msgSender, 0, sizeof(msgSender));


	bool salir = true;
	Player* p;
	int sendOK;


	while (salir)
	{
		memset(&client, 0, sizeof(client));
		int bytesIn = recvfrom(listening, (char*)&customMessage, sizeof(customMessage), 0, (sockaddr*)&client, &clientSize);

		switch (customMessage.cmd)
		{
		case 1:
			p = new Player();
			p->SetAlias(customMessage.data);
			Mem0Msg(customMessage);
			memcpy(p->GetAddressPointer(), &client, sizeof(client));
			customMessage.cmd = (char)2;
			memcpy(customMessage.data, welcomeMsg, sizeof(welcomeMsg));
			playerV.push_back(p);
			cout << "Player " << p->GetAlias().c_str() << " joined the server" << endl;
			sendOK = sendto(listening, (char*)&customMessage, sizeof(customMessage), 0, (sockaddr*)p->GetAddressPointer(), sizeof(p->GetAddress()));
			Mem0Msg(customMessage);
			break;
		case 2:
			break;

		case 3:
			ubicacion1 = charToInt(customMessage.data[0]);
			ubicacion2 = charToInt(customMessage.data[1]);
			for (int i = 0; i < games.size(); i++)
			{
				for (int j = 0; j < games[i]->ReturnPlayersInGame().size(); j++) 
				{
					if (client.sin_addr.S_un.S_addr == games[i]->ReturnAPlayerInGame(j)->GetAddress().sin_addr.S_un.S_addr &&
						client.sin_port == games[i]->ReturnAPlayerInGame(j)->GetAddress().sin_port) 
					{
						if (games[i]->ReturnAPlayerInGame(j)->GetTurn()) 
						{
							if (games[i]->input(games[i]->ReturnAPlayerInGame(j)->GetToken(), ubicacion1, ubicacion2) == 0)
							{

								if (j == 0)
								{
									games[i]->ReturnAPlayerInGame(0)->SetTurn(false);
									games[i]->ReturnAPlayerInGame(1)->SetTurn(true);
								}
								if (j == 1)
								{
									games[i]->ReturnAPlayerInGame(1)->SetTurn(false);
									games[i]->ReturnAPlayerInGame(0)->SetTurn(true);
								}
							}
							else
							{
								customMessage.cmd = (char)2;
								memcpy(customMessage.data, coordsMsg, sizeof(coordsMsg));
								sendOK = sendto(listening, (char*)&customMessage, sizeof(customMessage), 0, (sockaddr*)games[i]->ReturnAPlayerInGame(j)->GetAddressPointer(),
									sizeof(games[i]->ReturnAPlayerInGame(j)->GetAddress()));
								Mem0Msg(customMessage);
							}
						}
						else
						{
							cout << "Alguien intento jugar y no es su turno" << endl;
						}
						for (int j = 0; j < games[i]->ReturnPlayersInGame().size(); j++)
						{
							customMessage.cmd = (char)3;
							int k, l;
							for (k = 0; k < 3; k++)
								for (l = 0; l < 3; l++)
									SetElement(k, l, games[i]->GetMat(k, l), auxArray);
							memcpy(customMessage.data, auxArray, sizeof(auxArray));
							sendOK = sendto(listening, (char*)&customMessage, sizeof(customMessage), 0, (sockaddr*)games[i]->ReturnAPlayerInGame(j)->GetAddressPointer(),
								sizeof(games[i]->ReturnAPlayerInGame(j)->GetAddress()));
							Mem0Msg(customMessage);
						}
						
					}
				}

				/*for (int j = 0; j < games[i]->ReturnPlayersInGame().size(); j++)
				{
					customMessage.cmd = (char)3;
					int k, l;
					for (k = 0; k < 3; k++)
						for (l = 0; l < 3; l++)
							SetElement(k, l, games[i]->GetMat(k, l), auxArray);
					memcpy(customMessage.data, auxArray, sizeof(auxArray));
					sendOK = sendto(listening, (char*)&customMessage, sizeof(customMessage), 0, (sockaddr*)games[i]->ReturnAPlayerInGame(j)->GetAddressPointer(),
						sizeof(games[i]->ReturnAPlayerInGame(j)->GetAddress()));
					Mem0Msg(customMessage);
				}		*/		
			}

			//memset(&msgSender, 0, sizeof(msgSender));
			break;

		case 4:

			break;
		case 5:
			for (int i = 0; i < games.size(); i++)
			{
				for (int j = 0; j < games[i]->ReturnPlayersInGame().size(); j++)
				{
					if (client.sin_addr.S_un.S_addr == games[i]->ReturnAPlayerInGame(j)->GetAddress().sin_addr.S_un.S_addr &&
						client.sin_port == games[i]->ReturnAPlayerInGame(j)->GetAddress().sin_port)
					{
						games[i]->IncreaseAfterGameAnswers();
						if (customMessage.data[0] == 'y')
							games[i]->ReturnAPlayerInGame(j)->SetWantsToPlayAgain(true);
						else
							games[i]->ReturnAPlayerInGame(j)->SetWantsToPlayAgain(false);
						/*if (customMessage.data[0] == 'y')
						{
							games[i]->ReturnAPlayerInGame(j)->SetWantsToPlayAgain(true);
							if (games[i]->ReturnAPlayerInGame(0)->GetWantsToPlayAgain() && games[i]->ReturnAPlayerInGame(1)->GetWantsToPlayAgain())
							{
								games[i]->SetReady(false);
								games[i]->SetCompleted(false);
							}

						}
						else
						{
							customMessage.cmd = (char)2;
							memcpy(customMessage.data, thanksForPlaying, sizeof(thanksForPlaying));
							sendOK = sendto(listening, (char*)&customMessage, sizeof(customMessage), 0, (sockaddr*)games[i]->ReturnAPlayerInGame(j)->GetAddressPointer(),
								sizeof(games[i]->ReturnAPlayerInGame(j)->GetAddress()));
							Mem0Msg(customMessage);
							if (j == 0)
							{
								playerV.push_back(games[i]->ReturnAPlayerInGame(1));
								games[i]->ClearPLayerInGame();
							}
							if (j == 1)
							{
								playerV.push_back(games[i]->ReturnAPlayerInGame(0));
								games[i]->ClearPLayerInGame();
							}
						}*/
					}
					
				}
				if (games[i]->GetAfterGameAnswers() == 2)
				{
					for (int j = 0; j < games[i]->ReturnPlayersInGame().size(); j++)
					{
						if (client.sin_addr.S_un.S_addr == games[i]->ReturnAPlayerInGame(j)->GetAddress().sin_addr.S_un.S_addr &&
							client.sin_port == games[i]->ReturnAPlayerInGame(j)->GetAddress().sin_port)
						{
							if (games[i]->ReturnAPlayerInGame(0)->GetWantsToPlayAgain() && games[i]->ReturnAPlayerInGame(1)->GetWantsToPlayAgain())
							{
								games[i]->SetReady(false);
								games[i]->SetCompleted(false);
							}
							else
							{
								for (int k = 0; k < games[i]->ReturnPlayersInGame().size(); k++)
								{
									if (games[i]->ReturnAPlayerInGame(k)->GetWantsToPlayAgain())
									{
										playerV.push_back(games[i]->ReturnAPlayerInGame(k));
									}
									else
									{
										customMessage.cmd = (char)2;
										memcpy(customMessage.data, thanksForPlaying, sizeof(thanksForPlaying));
										sendOK = sendto(listening, (char*)&customMessage, sizeof(customMessage), 0, (sockaddr*)games[i]->ReturnAPlayerInGame(k)->GetAddressPointer(),
											sizeof(games[i]->ReturnAPlayerInGame(k)->GetAddress()));
										Mem0Msg(customMessage);
									}
								}
								games[i]->ClearPLayerInGame();
							}
						}
					}
				}
				
			}

			break;

		default:
			break;
		}

		if (playerV.size() >= 2)
		{
			TicTacToe* newGame = new TicTacToe();
			for (int i = 0; i < 2; i++)
			{
				newGame->AddPlayerToGame(playerV.back());
				playerV.pop_back();
			}
			games.push_back(newGame);
		}

		for (int i = 0; i < games.size(); i++)
		{
			if (games[i]->ReturnPlayersInGame().size()==0)
			{
				games.erase(games.begin() + i);
			}
			if (games.size() > 0)
			{
				if (!games[i]->GetReady() && games[i]->ReturnPlayersInGame().size() == 2 && !games[i]->GetCompleted())
				{
					games[i]->ReturnAPlayerInGame(0)->SetStatus(Status::WAITING);
					games[i]->ReturnAPlayerInGame(1)->SetStatus(Status::WAITING);
					random = random_int(0, 1);
					games[i]->ReturnAPlayerInGame(0)->SetToken(random);
					games[i]->ReturnAPlayerInGame(1)->SetToken(random - 1);
					if (random == 0)
						games[i]->ReturnAPlayerInGame(0)->SetTurn(true);
					else
						games[i]->ReturnAPlayerInGame(1)->SetTurn(true);
					games[i]->ReturnAPlayerInGame(1)->SetWantsToPlayAgain(false);
					games[i]->ReturnAPlayerInGame(0)->SetWantsToPlayAgain(false);
					games[i]->SetCompleted(false);
					games[i]->SetReady(true);
					games[i]->init();
				}

				for (int j = 0; j < games[i]->ReturnPlayersInGame().size(); j++)
				{
					if (games[i]->ReturnAPlayerInGame(j)->GetStatus() == Status::WAITING)
					{
						games[i]->ReturnAPlayerInGame(j)->SetStatus(Status::PLAYING);
						if (j == 0)
						{
							customMessage.cmd = 1;
							memcpy(customMessage.data, games[i]->ReturnAPlayerInGame(1)->GetAlias().c_str(), sizeof(games[i]->ReturnAPlayerInGame(1)->GetAlias()));
							sendOK = sendto(listening, (char*)&customMessage, sizeof(customMessage), 0, (sockaddr*)games[i]->ReturnAPlayerInGame(j)->GetAddressPointer(),
								sizeof(games[i]->ReturnAPlayerInGame(j)->GetAddress()));
							Mem0Msg(customMessage);
						}
						if (j == 1)
						{
							customMessage.cmd = 1;
							memcpy(customMessage.data, games[i]->ReturnAPlayerInGame(0)->GetAlias().c_str(), sizeof(games[i]->ReturnAPlayerInGame(0)->GetAlias()));
							sendOK = sendto(listening, (char*)&customMessage, sizeof(customMessage), 0, (sockaddr*)games[i]->ReturnAPlayerInGame(j)->GetAddressPointer(),
								sizeof(games[i]->ReturnAPlayerInGame(j)->GetAddress()));
							Mem0Msg(customMessage);
						}
					}
				}

				check = games[i]->check();
				if (check == 'f')
					cout << "nadie gano" << endl;
				else if ((games[i]->ReturnAPlayerInGame(0)->GetToken() == check))
				{
					customMessage.cmd = (char)5;
					memcpy(customMessage.data, youWin, sizeof(youWin));
					sendOK = sendto(listening, (char*)&customMessage, sizeof(customMessage), 0, (sockaddr*)games[i]->ReturnAPlayerInGame(0)->GetAddressPointer(),
						sizeof(games[i]->ReturnAPlayerInGame(0)->GetAddress()));
					Mem0Msg(customMessage);
					customMessage.cmd = (char)5;
					memcpy(customMessage.data, youLose, sizeof(youLose));
					sendOK = sendto(listening, (char*)&customMessage, sizeof(customMessage), 0, (sockaddr*)games[i]->ReturnAPlayerInGame(1)->GetAddressPointer(),
						sizeof(games[i]->ReturnAPlayerInGame(1)->GetAddress()));
					Mem0Msg(customMessage);
					games[i]->SetCompleted(true);
					games[i]->SetReady(false);
					games[i]->ReturnAPlayerInGame(0)->SetTurn(false);
					games[i]->ReturnAPlayerInGame(1)->SetTurn(false);
					games[i]->init();
				}
				else
				{
					customMessage.cmd = (char)5;
					memcpy(customMessage.data, youWin, sizeof(youWin));
					sendOK = sendto(listening, (char*)&customMessage, sizeof(customMessage), 0, (sockaddr*)games[i]->ReturnAPlayerInGame(1)->GetAddressPointer(),
						sizeof(games[i]->ReturnAPlayerInGame(1)->GetAddress()));
					Mem0Msg(customMessage);
					customMessage.cmd = (char)5;
					memcpy(customMessage.data, youLose, sizeof(youLose));
					sendOK = sendto(listening, (char*)&customMessage, sizeof(customMessage), 0, (sockaddr*)games[i]->ReturnAPlayerInGame(0)->GetAddressPointer(),
						sizeof(games[i]->ReturnAPlayerInGame(0)->GetAddress()));
					Mem0Msg(customMessage);
					games[i]->SetCompleted(true);
					games[i]->SetReady(false);
					games[i]->ReturnAPlayerInGame(0)->SetTurn(false);
					games[i]->ReturnAPlayerInGame(1)->SetTurn(false);
					games[i]->init();
				}

				if (!games[i]->GetCompleted() && games[i]->GetReady())
				{
					for (int l = 0; l < games[i]->ReturnPlayersInGame().size(); l++)
					{
						if (client.sin_addr.S_un.S_addr == games[i]->ReturnAPlayerInGame(l)->GetAddress().sin_addr.S_un.S_addr &&
							client.sin_port == games[i]->ReturnAPlayerInGame(l)->GetAddress().sin_port)
						{
							if (games[i]->GetReady())
							{
								for (int k = 0; k < games[i]->ReturnPlayersInGame().size(); k++)
								{
									if (games[i]->ReturnAPlayerInGame(k)->GetTurn())
									{
										customMessage.cmd = (char)4;
										memcpy(customMessage.data, coordsMsg, sizeof(coordsMsg));
										sendOK = sendto(listening, (char*)&customMessage, sizeof(customMessage), 0, (sockaddr*)games[i]->ReturnAPlayerInGame(k)->GetAddressPointer(),
											sizeof(games[i]->ReturnAPlayerInGame(k)->GetAddress()));
										Mem0Msg(customMessage);
									}
								}
							}
						}
					}
				}
			}
			
		}
	}


	// destruir socket
	closesocket(listening);

	// cleanup winsock
	WSACleanup();

	return 0;
}


