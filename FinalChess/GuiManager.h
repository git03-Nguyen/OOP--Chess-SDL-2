#pragma once

#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <string>

#include "Board.h"
#include "Pieces.h"

enum class Button { SETTINGS, UNDO, REDO };

class GuiManager {
protected:

	int BOARD_OFFSET_X = 30, BOARD_OFFSET_Y = 30;
	int SCREEN_WIDTH, SCREEN_HEIGHT, CELL_SIZE;

	int SIDEBAR_WIDTH, BUTTON_SIZE = 60;

	
	SDL_Window* window;
	SDL_Renderer* renderer;

	// all GUI objects
	SDL_Texture* background; // including background, buttons and board
	SDL_Texture* settingsButton;
	SDL_Texture* undoButton;
	SDL_Texture* redoButton;

	// Pieces
	Board* board;

	void drawBackground();
	void drawBoard();
	void drawPieces();
	void drawButtonMenu();
	void drawButton(SDL_Texture* texture, int size, int posX, int posY);
	void drawCurrentTurn(Color turn);


public:
	GuiManager(SDL_Window* _window, Board* piecesOnBoard);
	~GuiManager();

	void render(Color turn);
	void renderHighLight(Piece* piece);
	void renderClickBtn(Button btn);

	bool isOverButton(int x, int y);
	bool isOverBoard(int x, int y);

	Button getButton(int x, int y);

};

