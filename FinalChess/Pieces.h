#pragma once
#include <vector>
#include <string>
#include <SDL.h>
#include <SDL_image.h>
#include <string>
#include <vector>

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

	void setDead();


};

// ---------------------------------
class Rook : public Piece {
public:
	Rook(int _posX, int _posY, Color _color);
	~Rook();
};

// ---------------------------------
class Knight : public Piece {
public:
	Knight(int _posX, int _posY, Color _color);
	~Knight();
};

// ---------------------------------
class Bishop : public Piece {
public:
	Bishop(int _posX, int _posY, Color _color);
	~Bishop();

	
};

// ---------------------------------
class Queen : public Piece {
public:
	Queen(int _posX, int _posY, Color _color);
	~Queen();

};

// ---------------------------------
class King : public Piece {
public:
	King(int _posX, int _posY, Color _color);
	~King();

};

// ---------------------------------
class Pawn : public Piece {
public:
	Pawn(int _posX, int _posY, Color _color);
	~Pawn();

};
