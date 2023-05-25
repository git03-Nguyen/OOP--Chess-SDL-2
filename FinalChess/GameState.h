#pragma once
#include "Button.h"
#include "Pieces.h"
#include "Board.h"

enum class State { MAIN_MENU, CHOOSE_DIFFICULTY, CHOOSE_COLOR, PLAYING, SETTING_MENU, PROMOTION, MATCH_RESULT };

class GameState {
public:
	GameState();
	State state;
	bool isRunning;

	int choice; // 0: human ; 1: easy AI ; 2: hard AI
	int promotion; // 1-Queen, 2-Bishop, 3-Knight, 4-Rook
	int x, y, pX, pY;
	int volumn = 50; // 0 -100
	MatchResult matchResult;

	// For MAIN_MENU, CHOOSE_COLOR, CHOOSE_OPPONENT, PLAYING, SETTING, PROMOTION, MATCH_RESULT -> choose buttons
	Button* focusedButton;
	Button* clickedButton;

	// For PLAYING -> choose pieces
	Piece* clickedPiece;
	
	// flag if any changes in GUI
	bool guiHasChanged;
};


