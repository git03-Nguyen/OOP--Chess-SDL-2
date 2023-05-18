#pragma once
#include <vector>
#include <SDL.h>
#include <iostream>

#include "Pieces.h"

class Board {
private:
	Piece* bKing;
	Piece* wKing;
	std::vector<Piece*> allPieces;
	void setStartBoard();

public:
	std::vector<std::vector<Piece*>> piecesOnBoard;
	Board();
	~Board();
	
	bool movePiece(Piece* piece, int newX, int newY, SDL_Renderer* renderer);
	void promotePawn(Piece* pawn, SDL_Renderer* renderer);

	void undo();
	void redo();

};
