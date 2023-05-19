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
	bool isBeingAttacked(const vector<vector<Piece*>>& piecesOnBoard, Color allyColor, int targetX = -5, int targetY = -5) const;

	bool thisMoveCanMakeChecked(vector<vector<Piece*>>& piecesOnBoard, int newX, int newY);

	void setDead();


	virtual void updateTableMove(vector<vector<Piece*>>& piecesOnBoard) = 0;
	virtual bool move(vector<vector<Piece*>>& piecesOnBoard, int newX, int newY);



};

// ---------------------------------
class Rook : public Piece {
public:
	bool canCastling = true;
	Rook(int _posX, int _posY, Color _color);
	~Rook();
	void updateTableMove(vector<vector<Piece*>>& piecesOnBoard) override;
	bool move(vector<vector<Piece*>>& piecesOnBoard, int newX, int newY) override;
};

// ---------------------------------
class Knight : public Piece {
public:
	Knight(int _posX, int _posY, Color _color);
	~Knight();
	void updateTableMove(vector<vector<Piece*>>& piecesOnBoard) override;
};

// ---------------------------------
class Bishop : public Piece {
public:
	Bishop(int _posX, int _posY, Color _color);
	~Bishop();
	void updateTableMove(vector<vector<Piece*>>& piecesOnBoard) override;
	
};

// ---------------------------------
class Queen : public Piece {
public:
	Queen(int _posX, int _posY, Color _color);
	~Queen();
	void updateTableMove(vector<vector<Piece*>>& piecesOnBoard) override;
};

// ---------------------------------
class King : public Piece {
public:
	bool canCastling = true;
	King(int _posX, int _posY, Color _color);
	~King();
	void updateTableMove(vector<vector<Piece*>>& piecesOnBoard) override;
	bool move(vector<vector<Piece*>>& piecesOnBoard, int newX, int newY) override;
	void addCastlingMove(vector<vector<Piece*>>& piecesOnBoard);
};

// ---------------------------------
class Pawn : public Piece {
public:
	bool canFirstMove = true;
	bool enPassant = false;
	Pawn(int _posX, int _posY, Color _color);
	~Pawn();
	void updateTableMove(vector<vector<Piece*>>& piecesOnBoard) override;
	bool move(vector<vector<Piece*>>& piecesOnBoard, int newX, int newY) override;
};
