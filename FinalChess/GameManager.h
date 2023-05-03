#pragma once
#include <iostream>
#include <string>
#include <SDL.h>

#include "Board.h"
#include "GuiManager.h"

class GameManager {
private:
	const int BOARD_OFFSET_X = 30, BOARD_OFFSET_Y = 30;
	int SCREEN_WIDTH, SCREEN_HEIGHT, CELL_SIZE;

	// control window
	SDL_Window* window;

	// control model
	Board* board;

	// control view
	GuiManager* gui;

	// flags
	bool isRunning;
	Color currentTurn;
	bool boardStateChange;

	// uitilities
	void changeTurn();

public:

	GameManager(const char* title, int xPos, int yPos, int width, int height);
	~GameManager();

	void gameLoop(int fps);

	void handleEvent();
	void handleMouseClick(SDL_Event& e);
	void handleClickOnBoard(int boardX, int boardY);
	void handleClickButton(int x, int y);

	
};


