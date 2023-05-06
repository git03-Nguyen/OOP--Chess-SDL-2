#pragma once

#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <string>

#include "Board.h"
#include "Pieces.h"
#include "Button.h"

#define BOARD_OFFSET 29
#define BOARD_BORDER 39.1

#define TURN_POS_X 570
#define TURN_POS_Y 50
#define TURN_WIDTH 110
#define TURN_HEIGHT 180

class GuiManager {
protected:
	int SCREEN_WIDTH, SCREEN_HEIGHT, BOARD_SIZE, SIDEBAR_WIDTH, CELL_SIZE;
	
	SDL_Window* window;
	SDL_Renderer* renderer;

	// some textures
	SDL_Texture* bgTexture; 
	SDL_Texture* boardTexture;
	SDL_Texture* blackTurnTexture;
	SDL_Texture* whiteTurnTexture;
	SDL_Texture* nextMoveTexture;
	SDL_Texture* killingMoveTexture;

	// buttons
	Button* buttons[3];


	// Board - pieces
	Board* board;

	// Drawing methods
	void drawBackground();
	void drawBoard();
	void drawAllPieces();
	void drawButtons(Button* focusingBtn);
	void drawButton(Button* button);
	void drawCurrentTurn(Color turn);


public:
	GuiManager(SDL_Window* _window, Board* piecesOnBoard);
	~GuiManager();

	void render(Color turn, Piece* clickedPiece, Button* focusingBtn);
	void renderHighLight(Piece* piece);
	void renderClickBtn(Button* button);

	Button* getButton(int x, int y);
	bool isOnBoard(int x, int y);

};

