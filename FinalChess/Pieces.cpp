#include "Pieces.h"
#include <iostream>
using namespace std;

Piece::Piece(int _posX, int _posY, Color _color):
posX(_posX), posY(_posY), color(_color), imagePath(""), texture(nullptr), 
id(PieceID::Pawn), isAlive(true), tableMove(vector<int>()) {}

Piece::~Piece() {
	SDL_DestroyTexture(texture);
	tableMove.clear();
}

void Piece::setTexture(SDL_Renderer* renderer) {
	texture = IMG_LoadTexture(renderer, imagePath.c_str());
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

void Piece::setDead() {
	isAlive = false;
	tableMove.clear();
}

// ----------------------------------

Rook::Rook(int _posX, int _posY, Color _color): Piece(_posX, _posY, _color) {
	id = PieceID::Rook;
	imagePath = _color == Color::White ? "../Assets/Pieces/RookWhite.png" : "../Assets/Pieces/RookBlack.png";
}

Rook::~Rook() {
}

void Rook::updateTableMove(const vector<vector<Piece*>>& piecesOnBoard) {
	int x, y, i = 0, j = 3;
	int s[] = { -1, 1 , 0, 0 };
	for (int count = 0; count < 4; count++) {
		x = this->posX; y = this->posY;
		x += s[i]; y += s[j];
		while (x < 8 && x >= 0 && y < 8 && y >= 0 && !piecesOnBoard[x][y]) {
			this->addMove(x, y);
			x += s[i]; y += s[j];
		}
		if (x < 8 && x >= 0 && y < 8 && y >= 0 && piecesOnBoard[x][y]->color != this->color) {
			this->addMove(x, y);
		}
		i++; j--;
	}
	// lost castling -> King
}


// ----------------------------------
Knight::Knight(int _posX, int _posY, Color _color) : Piece(_posX, _posY, _color) {
	id = PieceID::Knight;
	imagePath = _color == Color::White ? "../Assets/Pieces/KnightWhite.png" : "../Assets/Pieces/KnightBlack.png";
}

Knight::~Knight() {
}

void Knight::updateTableMove(const vector<vector<Piece*>>& piecesOnBoard) {
	int x, y, i = 0, j = 7;
	int s[] = { 1, 1, -1, -1, 2, -2, 2, -2 };
	for (int count = 0; count < 8; count++) {
		x = this->posX; y = this->posY;
		x += s[i]; y += s[j];
		if (x < 8 && x >= 0 && y < 8 && y >= 0 && (!piecesOnBoard[x][y] || piecesOnBoard[x][y]->color != this->color)) {
			this->addMove(x, y);
			x += s[i]; y += s[j];
		}
		i++; j--;
	}
}


// ------------------------------------
Bishop::Bishop(int _posX, int _posY, Color _color) : Piece(_posX, _posY, _color) {
	id = PieceID::Bishop;
	imagePath = _color == Color::White ? "../Assets/Pieces/BishopWhite.png" : "../Assets/Pieces/BishopBlack.png";
}

Bishop::~Bishop() {
}

void Bishop::updateTableMove(const vector<vector<Piece*>>& piecesOnBoard) {
	int x, y, i = 0, j = 1;
	int s[] = { 1, 1, -1, -1, 1 };
	for (int count = 0; count < 4; count++) {
		x = this->posX; y = this->posY;
		x += s[i]; y += s[i] * s[j];
		while (x < 8 && x >= 0 && y < 8 && y >= 0 && !piecesOnBoard[x][y]) {
			this->addMove(x, y);
			x += s[i]; y += s[i] * s[j];
		}
		if (x < 8 && x >= 0 && y < 8 && y >= 0 && piecesOnBoard[x][y]->color != this->color) {
			this->addMove(x, y);
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

void Queen::updateTableMove(const vector<vector<Piece*>>& piecesOnBoard) {

	// updateTableMove of Bishop & Rook
	int x, y, i = 0, j = 1;
	int s[] = { 1, 1, -1, -1, 1 , 0, 0 };
	for (int count = 0; count < 4; count++) {
		x = this->posX; y = this->posY;
		x += s[i]; y += s[i] * s[j];
		while (x < 8 && x >= 0 && y < 8 && y >= 0 && !piecesOnBoard[x][y]) {
			this->addMove(x, y);
			x += s[i]; y += s[i] * s[j];
		}
		if (x < 8 && x >= 0 && y < 8 && y >= 0 && piecesOnBoard[x][y]->color != this->color) {
			this->addMove(x, y);
		}
		i++; j++;
	}
	i = 3; j = 6;
	for (int count = 0; count < 4; count++) {
		x = this->posX; y = this->posY;
		x += s[i]; y += s[j];
		while (x < 8 && x >= 0 && y < 8 && y >= 0 && !piecesOnBoard[x][y]) {
			this->addMove(x, y);
			x += s[i]; y += s[j];
		}
		if (x < 8 && x >= 0 && y < 8 && y >= 0 && piecesOnBoard[x][y]->color != this->color) {
			this->addMove(x, y);
		}
		i++; j--;
	}
	

}


// -------------------------------------
King::King(int _posX, int _posY, Color _color) : Piece(_posX, _posY, _color) {
	id = PieceID::King;
	imagePath = _color == Color::White ? "../Assets/Pieces/KingWhite.png" : "../Assets/Pieces/KingBlack.png";
}

King::~King() {
}

void King::updateTableMove(const vector<vector<Piece*>>& piecesOnBoard) {
	tableMove.clear();
	if (!isAlive) return;
}


// --------------------------------------
Pawn::Pawn(int _posX, int _posY, Color _color) : Piece(_posX, _posY, _color) {
	id = PieceID::Pawn;
	imagePath = _color == Color::White ? "../Assets/Pieces/PawnWhite.png" : "../Assets/Pieces/PawnBlack.png";
}

Pawn::~Pawn() {

}

void Pawn::updateTableMove(const vector<vector<Piece*>>& piecesOnBoard) {
	tableMove.clear();
	if (!isAlive) return;
	for (int x = 0; x < 8; x++) {
		for (int y = 0; y < 8; y++) {
			Piece* destPiece = piecesOnBoard[x][y];
			// Test
			if (!destPiece || destPiece->color != this->color) {
				this->tableMove.push_back(x);
				this->tableMove.push_back(y);
			}
		}
	}
}





