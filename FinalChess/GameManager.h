#pragma once
#include <iostream>
#include <string>
#include <SDL.h>

#include "Board.h"
#include "GuiManager.h"

class GameManager {
private:
	int SCREEN_WIDTH, SCREEN_HEIGHT, BOARD_SIZE, CELL_SIZE;

	// control window
	SDL_Window* window;

	// control model
	Board* board;

	// control view
	GuiManager* gui;

	// flags
	bool isRunning;
	Color currentTurn;
	Button focusingBtn;
	Piece* clickedPiece;
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
	void handleChoosingMove(int newX, int newY);
	void handleClickButton(Button clickedBtn);

	
};


