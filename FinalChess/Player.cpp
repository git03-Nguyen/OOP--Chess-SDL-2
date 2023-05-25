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
	if (diff == Difficulty::HARD) {
		initEvaluation();
	}
}

ComAI::~ComAI() {
	for (auto& eval : evals) {
		for (int i = 0; i < 8; i++) delete eval[i];
		delete eval;
	}
}

int ComAI::evaluate(Board* board) const {
	if (diff == Difficulty::RANDOM) return 0;
	int balance = 0;

	
	// Evaluation calculation
	int wQueen = 0, wPawn = 0, bQueen = 0, bPawn = 0;
	bool isEndGame = true;
	for (int i = 0; i < 8; i++) for (int j = 0; j < 8; j++) {
		if (board->pieces[i][j]) {
			if (board->pieces[i][j]->id != PieceID::King) {
				int eval = evals[(int)board->pieces[i][j]->id][i][j];
				balance += (board->pieces[i][j]->color == Color::White) ? eval : -eval;

				/*if (isEndGame) {
					if (board->pieces[i][j]->id == PieceID::Queen) 
						(board->pieces[i][j]->color == Color::White) ? wQueen++ : bQueen++;
					else if (board->pieces[i][j]->id == PieceID::Pawn) 
						(board->pieces[i][j]->color == Color::White) ? wPawn++ : bPawn++;
					else 
						isEndGame = false;
				}*/

			}
		}
	}
	
	isEndGame = false;
	balance += evals[5 + (int)isEndGame][board->wKing->posX][board->wKing->posY];
	balance -= evals[5 + (int)isEndGame][board->bKing->posX][board->bKing->posY];

	return balance;

}

int countMinimax = 0; // Testttttttttttt

int ComAI::minimax(Board* board, int depth, int alpha, int beta, bool maximizingPlayer) {
	if (depth == 0) return evaluate(board);

	countMinimax++; //Testttttttttttttttt

	bool isContinue = true;
	if (maximizingPlayer) {
		int maxEval = INT_MIN;
		for (int i = 0; i < 8 && isContinue; i++) for (int j = 0 && isContinue; j < 8; j++) {
			Piece* p = board->pieces[i][j];
			if (p && p->color != color && p->tableMove.size() > 0) {
				
				// Save tables of COM
				std::vector<std::vector<std::vector<int>>> oldTableMoves;
				oldTableMoves.resize(8);
				for (int m = 0; m < 8; m++) {
					oldTableMoves[m].resize(8);
					for (int n = 0; n < 8; n++) {
						if (board->pieces[m][n] && board->pieces[m][n]->color == color) {
							oldTableMoves[m][n] = board->pieces[m][n]->tableMove;
						}
					}
				}

				// Pick 1 move
				for (int index = 0; index < p->tableMove.size(); index += 2) {
					int x = p->tableMove[index];
					int y = p->tableMove[index + 1];

					// Try to move
					Piece* temp = board->pieces[x][y];
					board->pieces[x][y] = p;
					board->pieces[i][j] = nullptr;
					for (int m = 0; m < 8; m++) for (int n = 0; n < 8; n++) {
						if (board->pieces[m][n] && board->pieces[m][n]->color == color)
							board->pieces[m][n]->updateTableMove(board->pieces);
					}
					// Calculate eval
 					int eval = minimax(board, depth - 1, alpha, beta, false); // false -> ...
					maxEval = (eval > maxEval) ? eval : maxEval;
					alpha = (eval > alpha) ? eval : alpha;

					// Undo the move
					board->pieces[x][y] = temp;
					board->pieces[i][j] = p;
					for (int m = 0; m < 8; m++) for (int n = 0; n < 8; n++) {
						if (board->pieces[m][n] && board->pieces[m][n]->color == color) {
							board->pieces[m][n]->tableMove = oldTableMoves[m][n];
						}
					}
					if (beta <= alpha) {
						isContinue = false;
						break;
					}// alpha-beta prunning
				}
			}
		}
		return maxEval;
	}
	else {
		int minEval = INT_MAX;
		for (int i = 0; i < 8 && isContinue; i++) for (int j = 0; j < 8 && isContinue; j++) {
			Piece* p = board->pieces[i][j];
			if (p && p->color == color && p->tableMove.size() > 0) {
				std::vector<std::vector<std::vector<int>>> oldTableMoves;
				oldTableMoves.resize(8);
				for (int m = 0; m < 8; m++) {
					oldTableMoves[m].resize(8);
					for (int n = 0; n < 8; n++) {
						if (board->pieces[m][n] && board->pieces[m][n]->color != color) {
							oldTableMoves[m][n] = board->pieces[m][n]->tableMove;
						}
					}
				}
				for (int index = 0; index < p->tableMove.size(); index += 2) {
					int x = p->tableMove[index];
					int y = p->tableMove[index + 1];

					// Try to move
					Piece* temp = board->pieces[x][y];
					board->pieces[x][y] = p;
					board->pieces[i][j] = nullptr;
					for (int m = 0; m < 8; m++) for (int n = 0; n < 8; n++) {
						if (board->pieces[m][n] && board->pieces[m][n]->color != color)
							board->pieces[m][n]->updateTableMove(board->pieces);
					}

 					// Calculate eval
					int eval = minimax(board, depth - 1, alpha, beta, true); // false -> ...
					minEval = (eval < minEval) ? eval : minEval;
					beta = (eval < beta) ? eval : beta;
					// Undo the move
					board->pieces[x][y] = temp;
					board->pieces[i][j] = p;
					for (int m = 0; m < 8; m++) for (int n = 0; n < 8; n++) {
						if (board->pieces[m][n] && board->pieces[m][n]->color != color) {
							board->pieces[m][n]->tableMove = oldTableMoves[m][n];
						}
					}
					if (beta <= alpha) {
						isContinue = false; 
						break; // alpha-beta prunning
					}
				}
			}
		}
		return minEval;
	}
}

void ComAI::getBestMove(Piece*& bestPiece, int& bestX, int& bestY, Board* board, int depth) {
	int minEval = INT_MAX;
	int alpha = INT_MIN;
	int beta = INT_MAX;

	countMinimax = 0;

	for (int i = 0; i < 8; i++) for (int j = 0; j < 8; j++) {
		Piece* p = board->pieces[i][j];
		if (p && p->color == color && p->tableMove.size() > 0) {
			
			// Save tables for HUMAN pieces
			std::vector<std::vector<std::vector<int>>> oldTableMoves;
			oldTableMoves.resize(8);
			for (int m = 0; m < 8; m++) {
				oldTableMoves[m].resize(8);
				for (int n = 0; n < 8; n++) {
					if (board->pieces[m][n] && board->pieces[m][n]->color != color) {
						oldTableMoves[m][n] = board->pieces[m][n]->tableMove;
					}
				}
			}
			// Pick 1 move and try moving
			for (int index = 0; index < p->tableMove.size(); index += 2) {
				int x = p->tableMove[index];
				int y = p->tableMove[index + 1];

				// Try to move
				Piece* temp = board->pieces[x][y];
				board->pieces[x][y] = p;
				board->pieces[i][j] = nullptr;
				for (int m = 0; m < 8; m++) for (int n = 0; n < 8; n++) {
					if (board->pieces[m][n] && board->pieces[m][n]->color != color)
						board->pieces[m][n]->updateTableMove(board->pieces);
				}

				// Calculate eval
				int eval = minimax(board, depth, alpha, beta, true); // true -> maximizing -> human
				if (eval < minEval) {
					minEval = eval;
					bestPiece = p;
					bestX = x;
					bestY = y;
				}

				// Undo the move
				board->pieces[x][y] = temp;
				board->pieces[i][j] = p;
				for (int m = 0; m < 8; m++) for (int n = 0; n < 8; n++) {
					if (board->pieces[m][n] && board->pieces[m][n]->color != color) {
						board->pieces[m][n]->tableMove = oldTableMoves[m][n];
					}
				}

				beta = (eval < beta) ? eval : beta;

			}
			oldTableMoves.clear();
		}
	}
	std::cout << "COUNT: " << countMinimax << endl;// Testttttttttttttttttt
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
		Piece* p = board->pieces[i][j];
		if (p && p->color == color && p->tableMove.size() > 0) availPiece.push_back(p);
	}

	int chosen, newX, newY;
	chosen = rand() % (availPiece.size());
	newX = rand() % availPiece[chosen]->tableMove.size();
	newY = 0;
	if (newX % 2 != 0) {
		newX = (newX + 1) % availPiece[chosen]->tableMove.size();
	}
	newY = newX + 1;

	MoveID type = board->movePiece(availPiece[chosen], availPiece[chosen]->tableMove[newX], availPiece[chosen]->tableMove[newY]);
	if (type == MoveID::Promotion) {
		// Random for promotion
		int choice = rand() % 5 + 1;
		board->promotePawn(availPiece[chosen], availPiece[chosen]->tableMove[newX], availPiece[chosen]->tableMove[newY], choice);
		cout << "Random.AI have just promoted a pawn!" << endl;
	}
	else if (type == MoveID::KCastle || type == MoveID::QCastle) {
		cout << "Random.AI have just casling!" << endl;
	}
	else if (type == MoveID::EnPassant) {
		cout << "Random.AI have just en passant!" << endl;
	}

	std::cout << "Random.AI moved to [" << availPiece[chosen]->posX << "][" << availPiece[chosen]->posY << "]" << endl;

}

void ComAI::makeHardMove(Board* board) {
	Piece* bestPiece = nullptr;
	int bestX = 0, bestY = 0;

	int depth = 3;
	getBestMove(bestPiece, bestX, bestY, board, depth);

	if (bestPiece) {
		MoveID type = board->movePiece(bestPiece, bestX, bestY);
		if (type == MoveID::Promotion) {
			// Choose the best for promotion
			cout << "Hard.AI have just promoted a pawn!" << endl;
		}
		else if (type == MoveID::KCastle || type == MoveID::QCastle) {
			cout << "Hard.AI have just casling!" << endl;
		}
		else if (type == MoveID::EnPassant) {
			cout << "Hard.AI have just en passant!" << endl;
		}
		cout << "Hard.AI moved to [" << bestX << "][" << bestY << "]" << endl;	
	}
	else {
		throw string("AI cannot find the best piece to move!");
	}
	std::cout << "Current evaluation: " << evaluate(board) << endl;
}

Player::Player(Color _color): color(_color) {
}

Player::~Player() {
}


void ComAI::initEvaluation() {

	for (auto& eval : evals) eval = nullptr;

	evals[(int)PieceID::Pawn] = new int*[8] {	
		new int[8] { 0, 0, 0, 0, 0, 0, 0, 0 },
		new int[8] { 50, 50, 50, 50, 50, 50, 50, 50 },
		new int[8] { 10, 10, 20, 30, 30, 20, 10, 10 },
		new int[8] { 5, 5, 10, 25, 25, 10, 5, 5 },
		new int[8] { 0, 0, 0, 20, 20, 0, 0, 0 },
		new int[8] { 5, -5, -10, 0, 0, -10, -5, 5 },
		new int[8] { 5, 10, 10, -20, -20, 10, 10, 5 },
		new int[8] { 0, 0, 0, 0, 0, 0, 0, 0 }
	};

	evals[(int)PieceID::Rook] = new int* [8] {
		new int[8] { 0, 0, 0, 0, 0, 0, 0, 0 },
		new int[8] { 5, 10, 10, 10, 10, 10, 10, 5 },
		new int[8] { -5, 0, 0, 0, 0, 0, 0, -5 },
		new int[8] { -5, 0, 0, 0, 0, 0, 0, -5 },
		new int[8] { -5, 0, 0, 0, 0, 0, 0, -5 },
		new int[8] { -5, 0, 0, 0, 0, 0, 0, -5 },
		new int[8] { -5, 0, 0, 0, 0, 0, 0, -5 },
		new int[8] { 0, 0, 0, 5, 5, 0, 0, 0 }
	};

	evals[(int)PieceID::Knight] = new int* [8] {
		new int[8] { -50, -40, -30, -30, -30, -30, -40, -50 },
		new int[8] { -40, -20, 0, 0, 0, 0, -20, -40 },
		new int[8] { -30, 0, 10, 15, 15, 10, 0, -30 },
		new int[8] { -30, 5, 15, 20, 20, 15, 5, -30 },
		new int[8] { -30, 0, 15, 20, 20, 15, 0, -30 },
		new int[8] { -30, 5, 10, 15, 15, 10, 5, -30 },
		new int[8] { -40, -20, 0, 5, 5, 0, -20, -40 },
		new int[8] { -50, -40, -30, -30, -30, -30, -40, -50 }
	};

	evals[(int)PieceID::Bishop] = new int* [8] {
		new int[8] { -20, -10, -10, -10, -10, -10, -10, -20 },
		new int[8] { -10,  0,    0,   0,   0,   0,    0,  -10 },
		new int[8] { -10,  0,    5,  10,  10,   5,    0,  -10 },
		new int[8] { -10,  5,    5,  10,  10,   5,    5,  -10 },
		new int[8] { -10,  0,   10,  10,  10,  10,    0,  -10 },
		new int[8] { -10, 10,   10,  10,  10,  10,   10,  -10 },
		new int[8] { -10,  5,    0,   0,   0,   0,    5,  -10 },
		new int[8] { -20, -10, -10, -10, -10, -10, -10, -20 }
	};

	evals[(int)PieceID::Queen] = new int* [8] {
		new int[8] {-20, -10, -10, -5, -5, -10, -10, -20},
		new int[8] { -10, 0, 0, 0, 0, 0, 0, -10 },
		new int[8] { -10, 0, 5, 5, 5, 5, 0, -10 },
		new int[8] { -5, 0, 5, 5, 5, 5, 0, -5 },
		new int[8] { 0, 0, 5, 5, 5, 5, 0, -5 },
		new int[8] { -10, 5, 5, 5, 5, 5, 0, -10 },
		new int[8] { -10, 0, 5, 0, 0, 0, 0, -10 },
		new int[8] { -20, -10, -10, -5, -5, -10, -10, -20 }
	};

	// King middle game
	evals[(int)PieceID::King] = new int* [8] {
		new int[8] {-30, -40, -40, -50, -50, -40, -40, -30},
		new int[8] { -30, -40, -40, -50, -50, -40, -40, -30 },
		new int[8] { -30, -40, -40, -50, -50, -40, -40, -30 },
		new int[8] { -30, -40, -40, -50, -50, -40, -40, -30 },
		new int[8] { -20, -30, -30, -40, -40, -30, -30, -20 },
		new int[8] { -10, -20, -20, -20, -20, -20, -20, -10 },
		new int[8] { 20, 20, 0, 0, 0, 0, 20, 20 },
		new int[8] { 20, 30, 10, 0, 0, 10, 30, 20 }
	};

	// King end game
	evals[(int)PieceID::King + 1] = new int* [8] {
		new int[8] {-50, -40, -30, -20, -20, -30, -40, -50},
		new int[8] { -30, -20, -10, 0, 0, -10, -20, -30 },
		new int[8] { -30, -10, 20, 30, 30, 20, -10, -30 },
		new int[8] { -30, -10, 30, 40, 40, 30, -10, -30 },
		new int[8] { -30, -10, 30, 40, 40, 30, -10, -30 },
		new int[8] { -30, -10, 20, 30, 30, 20, -10, -30 },
		new int[8] { -30, -30, 0, 0, 0, 0, -30, -30 },
		new int[8] { -50, -30, -30, -30, -30, -30, -30, -50 }
	};



}
