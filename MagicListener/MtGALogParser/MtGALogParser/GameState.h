#pragma once
class GameState {
private:
	int playerOneLife;
	int playerTowLife;

public:
	GameState(string json);
	~GameState();
};

