#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include "Player.h"

using namespace std;

/*struct PlayerIn
{
	char alias[255];
	sockaddr_in address;	
};*/

class TicTacToe
{
public:
	char mat[3][3], p1, p2;
	vector<Player*> playersInGame;
	bool full;
	bool ready;
	bool completed;
	int afterGameAnswers;
	Player* ReturnAPlayerInGame(int _i);
	vector<Player*> ReturnPlayersInGame();	
	void ClearPLayerInGame();
	void init();
	void display();
	int input(char, int, int);
	char check();
	char GetMat(int, int);
	void AddPlayerToGame(Player* _p);
	bool GetReady();
	void SetReady(bool _ready);
	bool GetCompleted();
	void SetCompleted(bool _completed);
	int GetAfterGameAnswers();
	void IncreaseAfterGameAnswers();
	void ResetAfterGameAnswers();
	TicTacToe();
	~TicTacToe();

private:
};


