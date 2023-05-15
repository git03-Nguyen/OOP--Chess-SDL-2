#pragma once
#include "Button.h"
#include "Pieces.h"

enum class State { MAIN_MENU, CHOOSE_OPPONENT, CHOOSE_DIFFICULTY, CHOOSE_COLOR, PLAYING, SETTING_MENU, PROMOTION, MATCH_RESULT };

class GameState {
public:
	GameState();
	State state;
	bool isRunning;

	int choice; // 0: human ; 1: easy AI ; 2: hard AI

	// For MAIN_MENU, CHOOSE_COLOR, CHOOSE_OPPONENT, PLAYING, SETTING, PROMOTION, MATCH_RESULT -> choose buttons
	Button* focusedButton;
	Button* clickedButton;

	// For PLAYING -> choose pieces
	Color currentColor;
	Piece* clickedPiece;

	// For MATCH_RESULT
	int matchResult; // 0: going on ; 1: white win ; -1: black win
	
	// flag if any changes in GUI
	bool guiHasChanged;
};


