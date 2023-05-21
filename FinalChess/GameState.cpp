#include "GameState.h"

GameState::GameState() {
	state = State::MAIN_MENU; // 
	isRunning = true;

	choice = 0; // Human is default
	volumn = 50; // Default
	promotion = 0; // No promotion

	// For MAIN_MENU, CHOOSE_COLOR, CHOOSE_OPPONENT, PLAYING, SETTING_MENU, PROMOTION, MATCH_RESULT -> choose buttons
	focusedButton = nullptr;
	clickedButton = nullptr;

	// For PLAYING -> choose pieces
	currentColor = Color::White; // white always goes first
	clickedPiece = nullptr;

	// For MATCH_RESULT
	matchResult = 0; // 0: going on ; 1: white win ; -1: black win

	// flag if any changes in GUI
	guiHasChanged = true; // first draw
}
