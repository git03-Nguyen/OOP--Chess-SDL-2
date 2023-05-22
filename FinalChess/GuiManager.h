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
#define RESUME_POS_X 485
#define RESUME_POS_Y 165
#define RESUME_SIZE 50
#define VOLUMN_POS_X 166
#define VOLUMN_POS_Y 333
#define VOLUMN_WIDTH 20
#define VOLUMN_HEIGHT 266



class GuiManager {
private:
	int SCREEN_WIDTH, SCREEN_HEIGHT, BOARD_SIZE, SIDEBAR_WIDTH, CELL_SIZE;
	
	SDL_Window* window;
	SDL_Renderer* renderer;
	SDL_Texture* buffer;

	// some textures
	SDL_Texture* bgTexture; 
	SDL_Texture* boardTexture;
	SDL_Texture* blackTurnTexture;
	SDL_Texture* whiteTurnTexture;
	SDL_Texture* nextMoveTexture;
	SDL_Texture* killingMoveTexture;

	// Main menu textures
	SDL_Texture* mainMenuTexture;
	
	// Setting_menu textures
	SDL_Texture* settingBoardTexture;
	SDL_Texture* sliderVolumnTexture;
	SDL_Texture* dotVolumnTexture;

	// Match-result textures
	SDL_Texture* whiteWinTexture;
	SDL_Texture* blackWinTexture;

	// buttons
	vector<Button*> buttons;

	// Board - pieces
	Board* board;

	// Drawing methods
	void drawBackground(State state);
	void drawBoard();
	void drawAllPieces();
	void drawCurrentTurn(Color turn);
	void drawCircleButton(Button* button, Button* focusedButton, bool drawShadow = true);
	void drawHighLight(Piece* piece);


public:
	GuiManager(SDL_Window* _window);
	~GuiManager();
	void setBoard(Board* board);
	SDL_Renderer* getRenderer() const;

	bool isOnBoard(int x, int y) const;
	Button* getButton(GameState* gameState, int x, int y) const;

	void render(GameState* gameState);
	void renderMainMenu(GameState* gameState);
	void renderChooseDifficulty(GameState* gameState);
	void renderChooseColor(GameState* gameState);
	void renderPlaying(GameState* gameState);
	void renderSettingMenu(GameState* gameState);
	void renderPromotionMenu(GameState* gameState);
	void renderMatchResult(GameState* gameState);
	void addBlendLayer() const;


};

