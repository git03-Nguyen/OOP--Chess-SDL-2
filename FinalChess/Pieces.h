#pragma once
#include <vector>
#include <string>
#include <SDL.h>
#include <SDL_image.h>
#include <string>
#include <vector>

using namespace std;

enum class Color { White, Black };

enum class PieceID { Pawn, Rook, Knight, Bishop, Queen, King };

class Piece {
public:
	int posX, posY;
	Color color;
	PieceID id;
	std::string imagePath;
	SDL_Texture* texture;
	bool isAlive;
	std::vector<int> tableMove;

//public:
	Piece(int _posX, int _posY, Color _color);
	virtual ~Piece();

	void setTexture(SDL_Renderer* renderer);

	bool isLegalMove(int newX, int newY);
	void addMove(int x, int y);

	void setDead();

	virtual void updateTableMove(const vector<vector<Piece*>>& piecesOnBoard) = 0;


};

// ---------------------------------
class Rook : public Piece {
public:
	Rook(int _posX, int _posY, Color _color);
	~Rook();
	void updateTableMove(const vector<vector<Piece*>>& piecesOnBoard);
};

// ---------------------------------
class Knight : public Piece {
public:
	Knight(int _posX, int _posY, Color _color);
	~Knight();
	void updateTableMove(const vector<vector<Piece*>>& piecesOnBoard);
};

// ---------------------------------
class Bishop : public Piece {
public:
	Bishop(int _posX, int _posY, Color _color);
	~Bishop();
	void updateTableMove(const vector<vector<Piece*>>& piecesOnBoard);

	
};

// ---------------------------------
class Queen : public Piece {
public:
	Queen(int _posX, int _posY, Color _color);
	~Queen();
	void updateTableMove(const vector<vector<Piece*>>& piecesOnBoard);

};

// ---------------------------------
class King : public Piece {
public:
	King(int _posX, int _posY, Color _color);
	~King();
	void updateTableMove(const vector<vector<Piece*>>& piecesOnBoard);

};

// ---------------------------------
class Pawn : public Piece {
public:
	Pawn(int _posX, int _posY, Color _color);
	~Pawn();
	void updateTableMove(const vector<vector<Piece*>>& piecesOnBoard);

};
