#include "Player.h"

Human::Human(Color _color): Player(_color) {
	this->type = PlayerType::Human;
}

void Human::makeMove(Board* board) {
	// make move from handleEvent -> don't need to code here
}


// ------------------------------------------


ComAI::ComAI(Color _color, Difficulty _diff) : Player(_color), diff(_diff) {
	this->type = PlayerType::ComAI;
}

void ComAI::makeMove(Board* board) {
	// make move from calculations
	if (diff == Difficulty::RANDOM) {
		makeRandomMove(board);
	}
	else if (diff == Difficulty::HARD) {
		makeHardMove(board);
	}
}

void ComAI::makeRandomMove(Board* board) const {
	vector<Piece*> availPiece;
	for (int i = 0; i < 8; i++) for (int j = 0; j < 8; j++) {
		Piece* p = board->piecesOnBoard[i][j];
		if (p && p->color == color && p->tableMove.size() > 0) availPiece.push_back(p);
	}

	int chosen, newX, newY;
	do {
		chosen = rand() % (availPiece.size());
		newX = rand() % availPiece[chosen]->tableMove.size();
		newY = 0;
		if (newX % 2 != 0) {
			newX = (newX + 1) % availPiece[chosen]->tableMove.size();
		}
		newY = newX + 1;
	} while (!board->movePiece(availPiece[chosen], availPiece[chosen]->tableMove[newX], availPiece[chosen]->tableMove[newY]));

	cout << "AI moved to [" << availPiece[chosen]->posX << "][" << availPiece[chosen]->posY << "]" << endl;

	// Promotion
	if (availPiece[chosen]->id == PieceID::Pawn && (availPiece[chosen]->posY == 0 || availPiece[chosen]->posY == 7)) {
		int choice = rand() % 5 + 1;
		board->promotePawn(availPiece[chosen], choice);
	}

}

void ComAI::makeHardMove(Board* board) const {
	//
}

Player::Player(Color _color): color(_color) {
}
