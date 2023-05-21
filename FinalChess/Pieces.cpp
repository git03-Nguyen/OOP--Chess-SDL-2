#include "Pieces.h"
#include <iostream>
using namespace std;

bool isCheckingChecked = false;

Piece::Piece(int _posX, int _posY, Color _color):
posX(_posX), posY(_posY), color(_color), imagePath(""), texture(nullptr),
id(PieceID::Pawn), isAlive(true), tableMove(vector<int>()), myKing(nullptr) {}

Piece::~Piece() {
	SDL_DestroyTexture(texture);
	tableMove.clear();
}

void Piece::setTexture(SDL_Renderer* renderer) {
	texture = IMG_LoadTexture(renderer, imagePath.c_str());
}

void Piece::setKing(Piece* myKing) {
	if (myKing && myKing->color == color) {
		this->myKing = myKing;
	}
}

bool Piece::isLegalMove(int newX, int newY) {
	if (tableMove.size() == 0 || (posX == newX && posY == newY)) return false;

	for (int i = 0; i < tableMove.size() - 1; i += 2) {
		if (tableMove[i] == newX && tableMove[i + 1] == newY) {
			return true;
		}
	}
	return false;
}

void Piece::addMove(int x, int y) {
	tableMove.push_back(x);
	tableMove.push_back(y);
}

bool Piece::isBeingAttacked(const vector<vector<Piece*>>& piecesOnBoard, Color allyColor, int targetX, int targetY) const {
	if (targetX == -5 || targetY == -5) {
		targetX = posX;
		targetY = posY;
	}
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			if (piecesOnBoard[i][j] && piecesOnBoard[i][j]->color != color && piecesOnBoard[i][j]->isLegalMove(targetX, targetY)) {
				return true; // This square is being attacked by enemies
			}
		}
	}
	return false;
}

bool Piece::thisMoveCanMakeChecked(vector<vector<Piece*>>& piecesOnBoard, int newX, int newY) {
	isCheckingChecked = true;
	bool res = false;
	int i = 0, j = 0;
	
	bool found = false;

	// Save the old things
	Piece* dest = piecesOnBoard[newX][newY];
	vector<int> oldTableMoves = vector<int>();
	int oldX = posX, oldY = posY;
	
	// Try the move and check King is checked
	if (dest) dest->isAlive = false;
	piecesOnBoard[oldX][oldY] = nullptr; // move away from old position
	piecesOnBoard[newX][newY] = this; // move to new position
	posX = newX; posY = newY;

	found = false;
	for (i = 0; i < 8 && !found; i++) {
		for (j = 0; j < 8 && !found; j++) {
			if (piecesOnBoard[i][j] && piecesOnBoard[i][j]->color != color) {
				oldTableMoves.clear();
				oldTableMoves = piecesOnBoard[i][j]->tableMove;
				piecesOnBoard[i][j]->updateTableMove(piecesOnBoard);
				if (piecesOnBoard[i][j]->isLegalMove(myKing->posX, myKing->posY)) {
					res = true;
					found = true;
				}
				// Restore the enemy that cannot kill the King
				piecesOnBoard[i][j]->tableMove.clear();
				piecesOnBoard[i][j]->tableMove = oldTableMoves;
			}
		}
	}

	// UNDO things
	piecesOnBoard[newX][newY] = dest;
	if (dest) dest->isAlive = true;
	piecesOnBoard[oldX][oldY] = this;
	posX = oldX; posY = oldY;

	// Restore table moves
	if (found && i - 1 < 8 && j - 1 < 8) {
		piecesOnBoard[i - 1][j - 1]->tableMove.clear();
		piecesOnBoard[i - 1][j - 1]->tableMove = oldTableMoves;
	}

	isCheckingChecked = false;
	return res;
}

void Piece::setDead() {
	isAlive = false;
	tableMove.clear();
}

bool Piece::move(vector<vector<Piece*>>& piecesOnBoard, int newX, int newY) {
	if (posX == newX && posY == newY) return false;
	if (Piece* desPiece = piecesOnBoard[newX][newY]) {
		desPiece->setDead();
		std::cout << desPiece->imagePath << " is dead!" << std::endl;
	}
	piecesOnBoard[posX][posY] = nullptr;
	posX = newX;
	posY = newY;
	piecesOnBoard[posX][posY] = this;
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			if (piecesOnBoard[i][j] && piecesOnBoard[i][j]->id == PieceID::Pawn && piecesOnBoard[i][j]->color != color) {
				Pawn* enemyPawn = dynamic_cast<Pawn*>(piecesOnBoard[i][j]);
				enemyPawn->enPassant = false;
			}
		}
	}
	return true;
}

// ----------------------------------

Rook::Rook(int _posX, int _posY, Color _color): Piece(_posX, _posY, _color) {
	id = PieceID::Rook;
	imagePath = _color == Color::White ? "../Assets/Pieces/RookWhite.png" : "../Assets/Pieces/RookBlack.png";
}

Rook::~Rook() {
}

void Rook::updateTableMove(vector<vector<Piece*>>& piecesOnBoard) {
	this->tableMove.clear();
	if (!this->isAlive) return;
	int x, y, i = 0, j = 3;
	int s[] = { -1, 1 , 0, 0 };
	for (int count = 0; count < 4; count++, i++, j--) {
		x = this->posX + s[i]; y = this->posY + s[j];
		bool isBlocked = false;

		// Loop by this direction until out of board or meet an ally or attack an enemy
		while (x < 8 && x >= 0 && y < 8 && y >= 0 && (!piecesOnBoard[x][y] || piecesOnBoard[x][y]->color != this->color) && !isBlocked) {
			if (piecesOnBoard[x][y]) {
				isBlocked = true; // This direction is blocked by an an enemy
			}
			if (!isCheckingChecked && thisMoveCanMakeChecked(piecesOnBoard, x, y)) {
				// If moving to this move leading in checked => cancel this move
				x += s[i]; y += s[j];
				continue;
			}
			this->addMove(x, y);
			x += s[i]; y += s[j];
		}
	}
}

bool Rook::move(vector<vector<Piece*>>& piecesOnBoard, int newX, int newY) {
	if (!Piece::move(piecesOnBoard, newX, newY)) return false;
	canCastling = false;
	return true;
}

// ----------------------------------
Knight::Knight(int _posX, int _posY, Color _color) : Piece(_posX, _posY, _color) {
	id = PieceID::Knight;
	imagePath = _color == Color::White ? "../Assets/Pieces/KnightWhite.png" : "../Assets/Pieces/KnightBlack.png";
}

Knight::~Knight() {
}

void Knight::updateTableMove(vector<vector<Piece*>>& piecesOnBoard) {
	this->tableMove.clear();
	if (!this->isAlive) return;
	int x, y, i = 0, j = 7;
	int s[] = { 1, 1, -1, -1, 2, -2, 2, -2 };
	for (int count = 0; count < 8; count++, i++, j--) {
		x = this->posX + s[i]; y = this->posY + s[j];

		if (x < 8 && x >= 0 && y < 8 && y >= 0 && (!piecesOnBoard[x][y] || piecesOnBoard[x][y]->color != this->color)) {
			if (!isCheckingChecked && thisMoveCanMakeChecked(piecesOnBoard, x, y)) {
				// If moving to this move leading in checked => cancel this move
				continue;
			}
			this->addMove(x, y);
		}
	}
}

// ------------------------------------
Bishop::Bishop(int _posX, int _posY, Color _color) : Piece(_posX, _posY, _color) {
	id = PieceID::Bishop;
	imagePath = _color == Color::White ? "../Assets/Pieces/BishopWhite.png" : "../Assets/Pieces/BishopBlack.png";
}

Bishop::~Bishop() {
}

void Bishop::updateTableMove(vector<vector<Piece*>>& piecesOnBoard) {
	this->tableMove.clear();
	if (!this->isAlive) return;
	int x, y, i = 0, j = 1;
	int s[] = { 1, 1, -1, -1, 1 };
	for (int count = 0; count < 4; count++) {
		x = this->posX + s[i]; y = this->posY + s[i] * s[j];
		bool isBlocked = false;

		while (x < 8 && x >= 0 && y < 8 && y >= 0 && (!piecesOnBoard[x][y] || piecesOnBoard[x][y]->color != color) && !isBlocked) {
			if (piecesOnBoard[x][y]) {
				isBlocked = true; // This direction is blocked by an enemy
			}
			if (!isCheckingChecked && thisMoveCanMakeChecked(piecesOnBoard, x, y)) {
				// If moving to this move leading in checked => cancel this 
				x += s[i]; y += s[i] * s[j];
				continue;
			}
			
			this->addMove(x, y);
			x += s[i]; y += s[i] * s[j];
		}
		i++; j++;
	}
}


// -------------------------------------
Queen::Queen(int _posX, int _posY, Color _color) : Piece(_posX, _posY, _color) {
	id = PieceID::Queen;
	imagePath = _color == Color::White ? "../Assets/Pieces/QueenWhite.png" : "../Assets/Pieces/QueenBlack.png";
}

Queen::~Queen() {
}

void Queen::updateTableMove(vector<vector<Piece*>>& piecesOnBoard) {
	this->tableMove.clear();
	if (!this->isAlive) return;
	// updateTableMove of Bishop & Rook
	int x, y, i = 0, j = 1;
	int s[] = { 1, 1, -1, -1, 1 , 0, 0 };

	// Copy Bishop
	for (int count = 0; count < 4; count++) {
		x = this->posX + s[i]; y = this->posY + s[i] * s[j];
		bool isBlocked = false;

		while (x < 8 && x >= 0 && y < 8 && y >= 0 && (!piecesOnBoard[x][y] || piecesOnBoard[x][y]->color != color) && !isBlocked) {
			if (piecesOnBoard[x][y]) {
				isBlocked = true; // This direction is blocked by an enemy
			}
			if (!isCheckingChecked && thisMoveCanMakeChecked(piecesOnBoard, x, y)) {
				// If moving to this move leading in checked => cancel this 
				x += s[i]; y += s[i] * s[j];
				continue;
			}

			this->addMove(x, y);
			x += s[i]; y += s[i] * s[j];
		}
		i++; j++;
	}

	// Copy Rook
	i = 3; j = 6;
	for (int count = 0; count < 4; count++, i++, j--) {
		x = this->posX + s[i]; y = this->posY + s[j];
		bool isBlocked = false;

		// Loop by this direction until out of board or meet an ally or attack an enemy
		while (x < 8 && x >= 0 && y < 8 && y >= 0 && (!piecesOnBoard[x][y] || piecesOnBoard[x][y]->color != this->color) && !isBlocked) {
			if (piecesOnBoard[x][y]) {
				isBlocked = true; // This direction is blocked by an an enemy
			}
			if (!isCheckingChecked && thisMoveCanMakeChecked(piecesOnBoard, x, y)) {
				// If moving to this move leading in checked => cancel this move
				x += s[i]; y += s[j];
				continue;
			}
			this->addMove(x, y);
			x += s[i]; y += s[j];
		}
	}
	

}


// -------------------------------------
King::King(int _posX, int _posY, Color _color) : Piece(_posX, _posY, _color) {
	id = PieceID::King;
	imagePath = _color == Color::White ? "../Assets/Pieces/KingWhite.png" : "../Assets/Pieces/KingBlack.png";
}

King::~King() {
}

void King::updateTableMove(vector<vector<Piece*>>& piecesOnBoard) {
	this->tableMove.clear();
	if (!this->isAlive) return;

	// Check for castling
	if (canCastling) {
		addCastlingMove(piecesOnBoard);
	}

	int x, y, i, j;
	int s[] = { 1, 1, -1, -1, 1 , 0, 0 };

	// Straight moves
	i = 3; j = 6;
	for (int count = 0; count < 4; count++, i++, j--) {
		x = this->posX + s[i]; y = this->posY + s[j];

		if (x >= 0 && x < 8 && y >= 0 && y < 8 && (!piecesOnBoard[x][y] || piecesOnBoard[x][y]->color != color)) {
			if (!isCheckingChecked && thisMoveCanMakeChecked(piecesOnBoard, x, y)) {
				// If moving to this move leading in checked => cancel this move
				continue;
			}
			this->addMove(x, y);
		}
	}

	// Diagonal moves
	i = 0; j = 1;
	for (int count = 0; count < 4; count++, i++, j++) {
		x = this->posX + s[i]; y = this->posY + s[i] * s[j];

		if (x >= 0 && x < 8 && y >= 0 && y < 8 && (!piecesOnBoard[x][y] || piecesOnBoard[x][y]->color != color)) {
			if (!isCheckingChecked && thisMoveCanMakeChecked(piecesOnBoard, x, y)) {
				// If moving to this move leading in checked => cancel this mov
				continue;
			}
			this->addMove(x, y);
		}
	}


}

bool King::move(vector<vector<Piece*>>& piecesOnBoard, int newX, int newY) {
	// If move is castling -> move with rook (kingside/queenside)
	if (canCastling && abs(newX - posX) == 2 && newY == posY) {
		if (newX == 2) { // Queen side
			piecesOnBoard[0][posY]->move(piecesOnBoard, 3, posY);
		}
		else { // King side
			piecesOnBoard[7][posY]->move(piecesOnBoard, 5, posY);
		}
	}
	
	if (!Piece::move(piecesOnBoard, newX, newY)) return false;
	canCastling = false;
	return true;
}

void King::addCastlingMove(vector<vector<Piece*>>& piecesOnBoard) {
	if (!canCastling) return;
	if (posX != 4 || (posY != 0 && posY != 7)) return;
	
	bool flag = true;
	// King side
	if (piecesOnBoard[7][posY] && piecesOnBoard[7][posY]->id == PieceID::Rook && piecesOnBoard[7][posY]->color == color) {
		Rook* rook = dynamic_cast<Rook*>(piecesOnBoard[7][posY]);
		if (rook->canCastling && !this->isBeingAttacked(piecesOnBoard, color)) {
			for (int i = 5; i < 7; i++) {
				if (piecesOnBoard[i][posY] || isBeingAttacked(piecesOnBoard, color, i, posY)) {
					flag = false;
					break;
				}
			}
		}
		else {
			flag = false;
		}
	}
	else flag = false;

	if (flag) this->addMove(6, posY);
	flag = true;

	// Queen side
	if (piecesOnBoard[0][posY] && piecesOnBoard[0][posY]->id == PieceID::Rook && piecesOnBoard[0][posY]->color == color) {
		Rook* rook = dynamic_cast<Rook*>(piecesOnBoard[0][posY]);
		if (rook->canCastling && !this->isBeingAttacked(piecesOnBoard,color)) {
			for (int i = 2; i < 4; i++) {
				if (piecesOnBoard[i][posY] || isBeingAttacked(piecesOnBoard, color, i, posY)) {
					flag = false;
					break;
				}
			}
		}
		else {
			flag = false;
		}
	}
	else flag = false;


	if (flag) this->addMove(2, posY);

}

// --------------------------------------
Pawn::Pawn(int _posX, int _posY, Color _color) : Piece(_posX, _posY, _color) {
	id = PieceID::Pawn;
	imagePath = _color == Color::White ? "../Assets/Pieces/PawnWhite.png" : "../Assets/Pieces/PawnBlack.png";
}

Pawn::~Pawn() {

}

void Pawn::updateTableMove(vector<vector<Piece*>>& piecesOnBoard) {
	this->tableMove.clear();
	if (!this->isAlive) return;

	// Enpassant
	if (posX - 1 >= 0 && piecesOnBoard[posX-1][posY] && piecesOnBoard[posX-1][posY]->id == PieceID::Pawn && piecesOnBoard[posX - 1][posY]->color != color) {
		Pawn* enemyPawn = dynamic_cast<Pawn*>(piecesOnBoard[posX - 1][posY]);
		if (enemyPawn->enPassant) {
			int y = (color == Color::White) ? posY - 1 : posY + 1;
			if (y >= 0 && y < 8 && !piecesOnBoard[posX - 1][y]) {
				this->addMove(posX - 1, y);
			}
		}
	}
	if (posX + 1 < 8 && piecesOnBoard[posX + 1][posY] && piecesOnBoard[posX + 1][posY]->id == PieceID::Pawn && piecesOnBoard[posX + 1][posY]->color != color) {
		Pawn* enemyPawn = dynamic_cast<Pawn*>(piecesOnBoard[posX + 1][posY]);
		if (enemyPawn->enPassant) {
			int y = (color == Color::White) ? posY - 1 : posY + 1;
			if (y >= 0 && y < 8 && !piecesOnBoard[posX + 1][y]) {
				this->addMove(posX + 1, y);
			}
		}
	}

	// Go straight up - white ;;;; Go straight down - black
	int x = posX, y = posY;
	(color == Color::White) ? y -= 1 : y += 1;
	if (y >= 0 && y < 8 && !piecesOnBoard[x][y]
		&& (isCheckingChecked || !thisMoveCanMakeChecked(piecesOnBoard, x, y))) {
			// If moving to this move leading in checked => cancel this move
		this->addMove(x, y);
	}
	
	// First move
	if (canFirstMove && !piecesOnBoard[x][y]) {
		(color == Color::White) ? y -= 1 : y += 1;
		if (y >= 0 && y < 8 && !piecesOnBoard[x][y]
			&& (isCheckingChecked || !thisMoveCanMakeChecked(piecesOnBoard, x, y))) {
			// If moving to this move leading in checked => cancel this move
			this->addMove(x, y);
		}
	}

	// Kill diagonal up - white ;;;; ... down - black
	x = posX; y = posY;
	(color == Color::White) ? y -= 1 : y += 1;
	for (int i = -1; i <= 2; i += 3) {
		x += i;
		if (x >= 0 && x < 8 && y >= 0 && y < 8 && piecesOnBoard[x][y] && piecesOnBoard[x][y]->color != color) {
			if (!isCheckingChecked && thisMoveCanMakeChecked(piecesOnBoard, x, y)) {
				// If moving to this move leading in checked => cancel this mov
				continue;
			}
			this->addMove(x, y);
		}
	}


}

bool Pawn::move(vector<vector<Piece*>>& piecesOnBoard, int newX, int newY) {
	// Checking be enpassant_ed
	if (abs(newY - posY) == 2) {
		if (newX - 1 >= 0 && piecesOnBoard[newX - 1][newY]
			&& piecesOnBoard[newX - 1][newY]->id == PieceID::Pawn && piecesOnBoard[newX - 1][newY]->color != color) {
			enPassant = true;
		}else if (newX + 1 < 8 && piecesOnBoard[newX + 1][newY]
			&& piecesOnBoard[newX + 1][newY]->id == PieceID::Pawn && piecesOnBoard[newX + 1][newY]->color != color) {
			enPassant = true;
		}
	}

	// Enpassant 
	if (!piecesOnBoard[newX][newY] && abs(newX - posX) == 1) {
		int y = (color == Color::White) ? newY + 1 : newY - 1;
		piecesOnBoard[newX][y]->setDead();
		cout << piecesOnBoard[newX][y]->imagePath << "is dead!" << endl;
		piecesOnBoard[newX][y] = nullptr;
	}
	
	if (!Piece::move(piecesOnBoard, newX, newY)) return false;
	canFirstMove = false;
	return true;
}






