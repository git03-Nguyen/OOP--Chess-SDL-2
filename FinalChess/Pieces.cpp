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

void Rook::updateTableMove() {
	tableMove.clear();
	if (!isAlive) return;
}


// ----------------------------------
Knight::Knight(int _posX, int _posY, Color _color) : Piece(_posX, _posY, _color) {
	id = PieceID::Knight;
	imagePath = _color == Color::White ? "../Assets/Pieces/KnightWhite.png" : "../Assets/Pieces/KnightBlack.png";
}

Knight::~Knight() {
}

void Knight::updateTableMove() {
	tableMove.clear();
	if (!isAlive) return;
}


// ------------------------------------
Bishop::Bishop(int _posX, int _posY, Color _color) : Piece(_posX, _posY, _color) {
	id = PieceID::Bishop;
	imagePath = _color == Color::White ? "../Assets/Pieces/BishopWhite.png" : "../Assets/Pieces/BishopBlack.png";
}

Bishop::~Bishop() {
}

void Bishop::updateTableMove() {
	tableMove.clear();
	if (!isAlive) return;
}


// -------------------------------------
Queen::Queen(int _posX, int _posY, Color _color) : Piece(_posX, _posY, _color) {
	id = PieceID::Queen;
	imagePath = _color == Color::White ? "../Assets/Pieces/QueenWhite.png" : "../Assets/Pieces/QueenBlack.png";
}

Queen::~Queen() {
}

void Queen::updateTableMove() {
	tableMove.clear();
	if (!isAlive) return;
}


// -------------------------------------
King::King(int _posX, int _posY, Color _color) : Piece(_posX, _posY, _color) {
	id = PieceID::King;
	imagePath = _color == Color::White ? "../Assets/Pieces/KingWhite.png" : "../Assets/Pieces/KingBlack.png";
}

King::~King() {
}

void King::updateTableMove() {
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

void Pawn::updateTableMove() {
	tableMove.clear();
	if (!isAlive) return;

}





