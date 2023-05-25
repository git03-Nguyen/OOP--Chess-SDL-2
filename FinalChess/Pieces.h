#pragma once
#include <vector>
#include <string>
#include <SDL.h>
#include <SDL_image.h>
#include <string>
#include <vector>

using namespace std;

enum class Color { White, Black, None };

enum class PieceID { Pawn, Rook, Knight, Bishop, Queen, King };

enum class MoveID { Match = -1, Move = 0, Kill = 1, Pawn1st = 2, EnPassant = 3, Promotion = 4, QCastle = 5, KCastle = 6 };

class Piece {
public:
	int posX, posY;
	Color color;
	PieceID id;
	std::string imagePath;
	SDL_Texture* texture;
	bool isAlive;
	std::vector<int> tableMove;
	Piece* myKing;


	Piece(int _posX, int _posY, Color _color);
	virtual ~Piece();

	void setTexture(SDL_Renderer* renderer);
	void setKing(Piece* myKing);

	bool isLegalMove(int newX, int newY);
	void addMove(int x, int y);
	bool isBeingAttacked(const vector<vector<Piece*>>& pieces, int posX = -1, int posY = -1) const;

	bool thisMoveCanMakeChecked(vector<vector<Piece*>>& pieces, int newX, int newY);

	virtual void updateTableMove(vector<vector<Piece*>>& pieces) = 0;
	virtual MoveID move(vector<vector<Piece*>>& pieces, int newX, int newY, bool isTrying = false);



};

// ---------------------------------
class Rook : public Piece {
public:
	bool canCastling = true;
	Rook(int _posX, int _posY, Color _color);
	~Rook();
	void updateTableMove(vector<vector<Piece*>>& pieces) override;
	MoveID move(vector<vector<Piece*>>& pieces, int newX, int newY, bool isTrying = false);

};

// ---------------------------------
class Knight : public Piece {
public:
	Knight(int _posX, int _posY, Color _color);
	~Knight();
	void updateTableMove(vector<vector<Piece*>>& pieces) override;
};

// ---------------------------------
class Bishop : public Piece {
public:
	Bishop(int _posX, int _posY, Color _color);
	~Bishop();
	void updateTableMove(vector<vector<Piece*>>& pieces) override;
	
};

// ---------------------------------
class Queen : public Piece {
public:
	Queen(int _posX, int _posY, Color _color);
	~Queen();
	void updateTableMove(vector<vector<Piece*>>& pieces) override;
};

// ---------------------------------
class King : public Piece {
public:
	bool castlingQ = true;
	bool castlingK = true;

	King(int _posX, int _posY, Color _color);
	~King();

	void updateTableMove(vector<vector<Piece*>>& pieces) override;
	MoveID move(vector<vector<Piece*>>& pieces, int newX, int newY, bool isTrying = false) override;
	void addKingCastlingMove(vector<vector<Piece*>>& pieces);
	void addQueenCastlingMove(vector<vector<Piece*>>& pieces);
};

// ---------------------------------
class Pawn : public Piece {
public:
	bool enPassant = false;
	Pawn(int _posX, int _posY, Color _color);
	~Pawn();
	void updateTableMove(vector<vector<Piece*>>& pieces) override;
	MoveID move(vector<vector<Piece*>>& pieces, int newX, int newY, bool isTrying = false) override;
};
