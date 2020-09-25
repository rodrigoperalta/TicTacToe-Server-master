#include "Player.h"

Player::Player()
{
	//alias[255] = NULL;
	turn = false;
}

Player::~Player()
{
	//delete[] this;
}

bool Player::GetTurn()
{
	return turn;
}
void Player::SetTurn(bool _turn)
{
	turn = _turn;
}

void Player::SwitchTurns()
{
	if (turn)
		turn = false;
	if (!turn)
		turn = true;
}

void Player::SetWantsToPlayAgain(bool _wantsToPlayAgain)
{
	wantsToPlayAgain = _wantsToPlayAgain;
}
bool Player::GetWantsToPlayAgain()
{
	return wantsToPlayAgain;
}

char Player::GetToken()
{
	return token;
}
void Player::SetToken(int _token)
{
	if (_token == 0)
		token = (char)88;
	else
		token = (char)79;	
}

void Player::SetStatus(Status _status)
{
	status = _status;
}

Status Player::GetStatus()
{
	return status;
}

char* Player::GetAliasPointer()
{
	return &alias[255];
}

string Player::GetAlias()
{
	return (string)alias;
}

void Player::SetAlias(char _alias[255])
{
	memcpy(alias, _alias, sizeof(alias));	
}
sockaddr_in* Player::GetAddressPointer()
{
	return &address;
}

sockaddr_in Player::GetAddress()
{
	return address;
}
void Player::SetAddress(sockaddr_in _address)
{
	address = _address;
}
