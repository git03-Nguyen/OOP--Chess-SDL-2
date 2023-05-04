#pragma once

#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <string>

#include "Board.h"
#include "Pieces.h"

#define BOARD_OFFSET 29
#define BOARD_BORDER 39.1

#define BUTTON_SIZE 60
#define SETTING_POS_X 600
#define SETTING_POS_Y 500
#define UNDO_POS_X 600
#define UNDO_POS_Y 280
#define REDO_POS_X 600
#define REDO_POS_Y 350

#define TURN_POS_X 570
#define TURN_POS_Y 50
#define TURN_WIDTH 110
#define TURN_HEIGHT 180

enum class Button { NONE = -1, SETTINGS = 0, UNDO = 1, REDO = 2 };

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

	// button textures
	SDL_Texture* buttonTexture[3];


	// Board - pieces
	Board* board;

	// Drawing methods
	void drawBackground();
	void drawBoard();
	void drawAllPieces();
	void drawButtons();
	void drawButton(SDL_Texture* texture, int size, int posX, int posY);
	void drawCurrentTurn(Color turn);
	void drawButtonFocused(Button focusingBtn);


public:
	GuiManager(SDL_Window* _window, Board* piecesOnBoard);
	~GuiManager();

	void render(Color turn, Button focusingBtn);
	void renderHighLight(Piece* piece);
	void renderClickBtn(Button btn);

	Button getButton(int x, int y);
	bool isOverBoard(int x, int y);

};

