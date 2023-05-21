#pragma once
#include <iostream>
#include <string>
#include <SDL.h>

#include "Board.h"
#include "GuiManager.h"
#include "Player.h"
#include "GameState.h"


class GameManager {
private:
	int SCREEN_WIDTH, SCREEN_HEIGHT, BOARD_SIZE, CELL_SIZE;

	// control window
	SDL_Window* window;

	// control model
	Board* board;
	Player* player1;
	Player* player2;
	Player* player;

	// control view
	GuiManager* gui;

	// flags almost in game state
	GameState* gameState;
	
	int checkWinLose() const;

	// uitilities
	void changeTurn();

	// init new game
	void initialize();

public:

	GameManager(const char* title, int xPos, int yPos, int width, int height);
	~GameManager();

	void gameLoop(int fps);

	void handleEvent();
	
	void handleClickOnBoard(int boardX, int boardY);
	void handleChoosingMove(int newX, int newY);
	void handleClickButton(Button* clickedButton);

	// -------------
	// Count Perft Results
	int testCaseDepth(int depth) const;
	
};


