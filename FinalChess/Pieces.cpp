#include "Pieces.h"
#include <iostream>
using namespace std;

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
	this->myKing = myKing;
}

bool Piece::isLegalMove(int newX, int newY) {
	if (tableMove.size() == 0) return false;
	for (int i = 0; i < tableMove.size() - 1; i += 2) 
		if (tableMove[i] == newX && tableMove[i + 1] == newY) 
			return true;
	return false;
}

void Piece::addMove(int x, int y) {
	tableMove.push_back(x);
	tableMove.push_back(y);
}

bool Piece::isBeingAttacked(const vector<vector<Piece*>>& pieces, int posX, int posY) const {
	if (posX == -1 && posY == -1) {
		posX = this->posX; 
		posY = this->posY;
	}
	int x, y, i = 0, j = 7;
	// Check 8 L square
	int knight[] = { 1, 1, -1, -1, 2, -2, 2, -2 };
	for (int count = 0; count < 8; count++, i++, j--) {
		x = posX + knight[i]; y = posY + knight[j];
		if (x < 8 && x >= 0 && y < 8 && y >= 0 && pieces[x][y] && pieces[x][y]->color != color && pieces[x][y]->id == PieceID::Knight)
			return true;
	}

	// Check 4 lines
	i = 0; j = 3;
	int rook[] = { -1, 1 , 0, 0 };
	for (int count = 0; count < 4; count++, i++, j--) {
		x = posX + rook[i]; y = posY + rook[j];
		bool isBlocked = false;
		while (x < 8 && x >= 0 && y < 8 && y >= 0 && !isBlocked) {
			if (pieces[x][y]) {
				isBlocked = true;
				if (pieces[x][y]->color != color && (pieces[x][y]->id == PieceID::Rook || pieces[x][y]->id == PieceID::Queen))
					return true;
			}
			x += rook[i]; y += rook[j];
		}
	}
	
	// Check 4 diagonal
	i = 0; j = 1;
	int bishop[] = { 1, 1, -1, -1, 1 };
	for (int count = 0; count < 4; count++, i++, j++) {
		x = posX + bishop[i]; y = posY + bishop[i] * bishop[j];
		bool isBlocked = false;
		while (x < 8 && x >= 0 && y < 8 && y >= 0 && !isBlocked) {
			if (pieces[x][y]) {
				isBlocked = true;
				if (pieces[x][y]->color != color) {
					if (pieces[x][y]->id == PieceID::Bishop || pieces[x][y]->id == PieceID::Queen)
						return true;
					if (pieces[x][y]->id == PieceID::Pawn &&
						((color == Color::White && y == posY - 1) || (color == Color::Black && y == posY + 1)))
						return true;
				}
			}
			x += bishop[i]; y += bishop[i] * bishop[j];
		}
	}

	return false;
}

bool Piece::thisMoveCanMakeChecked(vector<vector<Piece*>>& pieces, int newX, int newY) {
	// Save the old things
	Piece* dest = pieces[newX][newY];
	vector<int> oldTableMoves = vector<int>();
	int oldX = posX, oldY = posY;
	
	// Try the move and check King is checked
	if (dest) dest->isAlive = false;
	pieces[oldX][oldY] = nullptr; // move away from old position
	pieces[newX][newY] = this; // move to new position
	posX = newX; posY = newY;

	King* king = dynamic_cast<King*>(myKing);
	bool res = king->isBeingAttacked(pieces);

	// UNDO things
	pieces[newX][newY] = dest;
	if (dest) dest->isAlive = true;
	pieces[oldX][oldY] = this;
	posX = oldX; posY = oldY;

	return res;
}

MoveID Piece::move(vector<vector<Piece*>>& pieces, int newX, int newY, bool isTry) {
	MoveID type = MoveID::Move;
	if (pieces[newX][newY]) {
		pieces[newX][newY]->isAlive = false;
		if (!isTry) delete pieces[newX][newY];
		type = MoveID::Kill;
	}
	pieces[posX][posY] = nullptr;
	pieces[posX = newX][posY = newY] = this;
	return type;
}

// ----------------------------------

Rook::Rook(int _posX, int _posY, Color _color): Piece(_posX, _posY, _color) {
	id = PieceID::Rook;
	imagePath = _color == Color::White ? "../Assets/Pieces/RookWhite.png" : "../Assets/Pieces/RookBlack.png";
}

Rook::~Rook() {
}

void Rook::updateTableMove(vector<vector<Piece*>>& pieces) {
	this->tableMove.clear();
	if (!this->isAlive) return;

	int x, y, i = 0, j = 3;
	int s[] = { -1, 1 , 0, 0 };

	// 4 line directions
	for (int count = 0; count < 4; count++, i++, j--) {
		x = this->posX + s[i]; y = this->posY + s[j];
		bool isBlocked = false;

		// Loop by this direction until out of board or meet an ally or attack an enemy
		while (x < 8 && x >= 0 && y < 8 && y >= 0 && (!pieces[x][y] || pieces[x][y]->color != this->color) && !isBlocked) {
			if (pieces[x][y]) {
				isBlocked = true; // This direction is blocked by an an enemy
			}
			if (thisMoveCanMakeChecked(pieces, x, y)) {
				// If moving to this move leading in checked => cancel this move
				x += s[i]; y += s[j];
				continue;
			}
			this->addMove(x, y);
			x += s[i]; y += s[j];
		}
	}
}

MoveID Rook::move(vector<vector<Piece*>>& pieces, int newX, int newY, bool isTrying) {
	canCastling = false;
	return Piece::move(pieces, newX, newY, isTrying);
}

// ----------------------------------
Knight::Knight(int _posX, int _posY, Color _color) : Piece(_posX, _posY, _color) {
	id = PieceID::Knight;
	imagePath = _color == Color::White ? "../Assets/Pieces/KnightWhite.png" : "../Assets/Pieces/KnightBlack.png";
}

Knight::~Knight() {
}

void Knight::updateTableMove(vector<vector<Piece*>>& pieces) {
	this->tableMove.clear();
	if (!this->isAlive) return;

	int x, y, i = 0, j = 7;
	int s[] = { 1, 1, -1, -1, 2, -2, 2, -2 };

	// 8-L directions
	for (int count = 0; count < 8; count++, i++, j--) {
		x = this->posX + s[i]; y = this->posY + s[j];

		if (x < 8 && x >= 0 && y < 8 && y >= 0 && (!pieces[x][y] || pieces[x][y]->color != this->color)) {
			if (thisMoveCanMakeChecked(pieces, x, y)) {
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

void Bishop::updateTableMove(vector<vector<Piece*>>& pieces) {
	this->tableMove.clear();
	if (!this->isAlive) return;

	int x, y, i = 0, j = 1;
	int s[] = { 1, 1, -1, -1, 1 };

	// 4 diagonal directions
	for (int count = 0; count < 4; count++) {
		x = this->posX + s[i]; y = this->posY + s[i] * s[j];
		bool isBlocked = false;

		while (x < 8 && x >= 0 && y < 8 && y >= 0 && (!pieces[x][y] || pieces[x][y]->color != color) && !isBlocked) {
			if (pieces[x][y]) {
				isBlocked = true; // This direction is blocked by an enemy
			}
			if (thisMoveCanMakeChecked(pieces, x, y)) {
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

void Queen::updateTableMove(vector<vector<Piece*>>& pieces) {
	this->tableMove.clear();
	if (!this->isAlive) return;

	int x, y, i = 0, j = 1;
	int s[] = { 1, 1, -1, -1, 1 , 0, 0 };

	// Copy Bishop - 4 diagonal directions
	for (int count = 0; count < 4; count++) {
		x = this->posX + s[i]; y = this->posY + s[i] * s[j];
		bool isBlocked = false;

		while (x < 8 && x >= 0 && y < 8 && y >= 0 && (!pieces[x][y] || pieces[x][y]->color != color) && !isBlocked) {
			if (pieces[x][y]) {
				isBlocked = true; // This direction is blocked by an enemy
			}
			if (thisMoveCanMakeChecked(pieces, x, y)) {
				// If moving to this move leading in checked => cancel this 
				x += s[i]; y += s[i] * s[j];
				continue;
			}

			this->addMove(x, y);
			x += s[i]; y += s[i] * s[j];
		}
		i++; j++;
	}

	// Copy Rook - 4 line directions
	i = 3; j = 6;
	for (int count = 0; count < 4; count++, i++, j--) {
		x = this->posX + s[i]; y = this->posY + s[j];
		bool isBlocked = false;

		while (x < 8 && x >= 0 && y < 8 && y >= 0 && (!pieces[x][y] || pieces[x][y]->color != this->color) && !isBlocked) {
			if (pieces[x][y]) {
				isBlocked = true; // This direction is blocked by an an enemy
			}
			if (thisMoveCanMakeChecked(pieces, x, y)) {
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

void King::updateTableMove(vector<vector<Piece*>>& pieces) {
	this->tableMove.clear();
	if (!this->isAlive) return;

	// Check for castling
	if (castlingK) {
		addKingCastlingMove(pieces);
	}
	if (castlingQ) {
		addQueenCastlingMove(pieces);
	}

	int x, y, i, j;
	int s[] = { 1, 1, -1, -1, 1 , 0, 0 };

	// Straight moves
	i = 3; j = 6;
	for (int count = 0; count < 4; count++, i++, j--) {
		x = this->posX + s[i]; y = this->posY + s[j];

		if (x >= 0 && x < 8 && y >= 0 && y < 8 && (!pieces[x][y] || pieces[x][y]->color != color)) {
			if (thisMoveCanMakeChecked(pieces, x, y)) {
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

		if (x >= 0 && x < 8 && y >= 0 && y < 8 && (!pieces[x][y] || pieces[x][y]->color != color)) {
			if (thisMoveCanMakeChecked(pieces, x, y)) {
				// If moving to this move leading in checked => cancel this mov
				continue;
			}
			this->addMove(x, y);
		}
	}


}

MoveID King::move(vector<vector<Piece*>>& pieces, int newX, int newY, bool isTrying) {
	castlingK = castlingQ = false;

	// If move is castling -> move with rook (kingside/queenside)
	if (abs(newX - posX) == 2 && newY == posY) {
		if (newX == 2) { // Queen side
			pieces[0][posY]->move(pieces, 3, posY);
			Piece::move(pieces, newX, newY, isTrying);
			return MoveID::QCastle;
		}
		else { // King side
			pieces[7][posY]->move(pieces, 5, posY);
			Piece::move(pieces, newX, newY, isTrying);
			return MoveID::KCastle;
		}
	}
	
	return Piece::move(pieces, newX, newY, isTrying);
}

void King::addKingCastlingMove(vector<vector<Piece*>>& pieces) {
	if (posX != 4 || (posY != 0 && posY != 7)) return;
	if (pieces[7][posY] && pieces[7][posY]->id == PieceID::Rook && pieces[7][posY]->color == color && !this->isBeingAttacked(pieces)) {
		for (int i = 5; i < 7; i++) {
			if (pieces[i][posY] || isBeingAttacked(pieces, i, posY)) {
				return;
			}
		}
	}
	else return;
	this->addMove(6, posY);
}

void King::addQueenCastlingMove(vector<vector<Piece*>>& pieces) {
	if (posX != 4 || (posY != 0 && posY != 7)) return;
	if (pieces[0][posY] && pieces[0][posY]->id == PieceID::Rook && pieces[0][posY]->color == color && !this->isBeingAttacked(pieces)) {
		for (int i = 2; i < 4; i++) {
			if (pieces[i][posY] || isBeingAttacked(pieces, i, posY)) {
				return;
			}
		}
	}
	else return;
	this->addMove(2, posY);
}

// --------------------------------------
Pawn::Pawn(int _posX, int _posY, Color _color) : Piece(_posX, _posY, _color) {
	id = PieceID::Pawn;
	imagePath = _color == Color::White ? "../Assets/Pieces/PawnWhite.png" : "../Assets/Pieces/PawnBlack.png";
}

Pawn::~Pawn() {

}

void Pawn::updateTableMove(vector<vector<Piece*>>& pieces) {
	this->tableMove.clear();
	if (!this->isAlive) return;

	// Go straight up - white ;;;; Go straight down - black
	int x = posX, y = posY;
	(color == Color::White) ? y -= 1 : y += 1;
	if (y >= 0 && y < 8 && !pieces[x][y]
		&& (!thisMoveCanMakeChecked(pieces, x, y))) {
			// If moving to this move leading in checked => cancel this move
		this->addMove(x, y);
	}
	
	// First move 
	if (posY == 1 + 5 * (color == Color::White) && !pieces[x][y]) {
		(color == Color::White) ? y -= 1 : y += 1;
		if (y >= 0 && y < 8 && !pieces[x][y]
			&& (!thisMoveCanMakeChecked(pieces, x, y))) {
			// If moving to this move leading in checked => cancel this move
			this->addMove(x, y);
		}
	}

	// Kill diagonal up - white ;;;; ... down - black
	x = posX; y = posY;
	(color == Color::White) ? y -= 1 : y += 1;
	for (int i = -1; i <= 2; i += 3) {
		x += i;
		if (x >= 0 && x < 8 && y >= 0 && y < 8 && pieces[x][y] && pieces[x][y]->color != color) {
			if (thisMoveCanMakeChecked(pieces, x, y)) {
				// If moving to this move leading in checked => cancel this mov
				continue;
			}
			this->addMove(x, y);
		} // else if target is an EnPassant
	}

}

MoveID Pawn::move(vector<vector<Piece*>>& pieces, int newX, int newY, bool isTrying) {
	// If first move: 
	if (abs(newY - posY) == 2) {
		Piece::move(pieces, newX, newY, isTrying);
		return MoveID::Pawn1st;
	}
	// If enpassant
	else if (abs(newX - posX) == 1 && !pieces[newX][newY]) {
		int x = newX;
		int y = (color == Color::White) ? newY + 1 : newY - 1;
		pieces[x][y]->isAlive = false;
		if (!isTrying) {
			delete pieces[x][y];
			pieces[x][y] = nullptr;
		}
		Piece::move(pieces, newX, newY, isTrying);
		return MoveID::EnPassant;
	}
	// If promotion
	else if (newY == 0 || newY == 7) {
		return MoveID::Promotion;
	}
	// Normal move
	return Piece::move(pieces, newX, newY, isTrying);
}






