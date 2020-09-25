#pragma once
#include <iostream>
#include <WS2tcpip.h>
#pragma comment (lib, "ws2_32.lib")
using namespace std;

enum Status
{
	WAITING,
	PLAYING,	
};

class Player
{
public:
	char alias[255];
	Status status;
	sockaddr_in address;
	char token;
	bool turn;
	bool wantsToPlayAgain;
	char* GetAliasPointer();
	string GetAlias();
	void SetAlias(char _alias[255]);
	sockaddr_in* GetAddressPointer();
	sockaddr_in GetAddress(); 
	void SetAddress(sockaddr_in _address);
	void SetStatus(Status _status);
	Status GetStatus();
	char GetToken();
	void SetToken(int _token);
	bool GetTurn();
	void SetTurn(bool _turn);	
	void SetWantsToPlayAgain(bool _wantsToPlayAgain);
	bool GetWantsToPlayAgain();
	void SwitchTurns();

	
	
	Player();
	~Player();
};

