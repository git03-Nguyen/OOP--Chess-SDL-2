#include "Player.h"

Human::Human(Color _color): Player(_color) {
	this->type = PlayerType::Human;
}

void Human::makeMove(Board& board) {
	// make move from handleEvent -> don't need to code here
}


// ------------------------------------------


ComAI::ComAI(Color _color, Difficulty _diff) : Player(_color), diff(_diff) {
	this->type = PlayerType::ComAI;
}

void ComAI::makeMove(Board& board) {
	// make move from calculations
	if (diff == Difficulty::RANDOM) {
		makeRandomMove(board);
	}
	else if (diff == Difficulty::HARD) {
		makeHardMove(board);
	}
}

void ComAI::makeRandomMove(Board& board) const {
	// 
}

void ComAI::makeHardMove(Board& board) const {
	//
}

Player::Player(Color _color): color(_color) {

}
