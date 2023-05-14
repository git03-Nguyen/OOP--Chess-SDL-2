#include "Player.h"

Human::Human() {
	this->type = PlayerType::Human;
}

void Human::makeMove(Board& board) {
	// make move from handleEvent -> don't need to code here
}


// ------------------------------------------


ComAI::ComAI(Difficulty difficulty) {
	this->type = PlayerType::ComAI;
	this->difficulty = difficulty;
}

void ComAI::makeMove(Board& board) {
	// make move from calculations
	if (difficulty == Difficulty::RANDOM) {
		makeRandomMove(board);
	}
	else if (difficulty == Difficulty::HARD) {
		makeHardMove(board);
	}
}

void ComAI::makeRandomMove(Board& board) const {
	// 
}

void ComAI::makeHardMove(Board& board) const {
	//
}
