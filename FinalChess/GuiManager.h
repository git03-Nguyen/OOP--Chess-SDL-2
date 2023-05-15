#pragma once

#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <string>

#include "Board.h"
#include "GameState.h"

#define BOARD_OFFSET 29
#define BOARD_BORDER 39.1

#define TURN_POS_X 570
#define TURN_POS_Y 50
#define TURN_WIDTH 110
#define TURN_HEIGHT 180

#define BUTTON_SIZE 60
#define SETTING_POS_X 600
#define SETTING_POS_Y 500
#define UNDO_POS_X 600
#define UNDO_POS_Y 280
#define REDO_POS_X 600
#define REDO_POS_Y 350

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
	vector<Button*> buttons;


	// Board - pieces
	Board* board;

	// Drawing methods
	void drawBackground();
	void drawBoard();
	void drawAllPieces();
	void drawButtons(State gameState, Button* focusingBtn);
	void drawCircleButton(Button* button);
	void drawCurrentTurn(Color turn);


public:
	GuiManager(SDL_Window* _window, Board* piecesOnBoard);
	~GuiManager();

	void render(GameState* gameState);
	void renderHighLight(Piece* piece);
	void renderClickBtn(Button* button);

	Button* getButton(GameState* gameState, int x, int y) const;
	bool isOnBoard(int x, int y);

	Button* getButtonMainMenu(int x, int y) const;
	Button* getButtonChooseMode(int x, int y) const;
	Button* getButtonChooseColor(int x, int y) const;
	Button* getButtonPlaying(int x, int y) const;
	Button* getButtonSetting(int x, int y) const;
	Button* getButtonPromotion(int x, int y) const;
	Button* getButtonMatchResult(int x, int y) const;

	
	void addBlendLayer() const;
	void renderMatchResult(int matchResult) const;
	void renderSettingMenu() const;


};

