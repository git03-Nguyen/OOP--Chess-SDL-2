#include "GameState.h"

GameState::GameState() {
	state = State::MAIN_MENU; // 
	isRunning = true;

	choice = 0; // Human is default
	promotion = 0; // No promotion

	matchResult = MatchResult::None;

	// For MAIN_MENU, CHOOSE_COLOR, CHOOSE_OPPONENT, PLAYING, SETTING_MENU, PROMOTION, MATCH_RESULT -> choose buttons
	focusedButton = nullptr;
	clickedButton = nullptr;

	// For PLAYING -> choose pieces
	clickedPiece = nullptr;
	x = y = 0;

	// flag if any changes in GUI
	guiHasChanged = true; // first draw
}
