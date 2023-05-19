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
	SDL_Renderer* renderer;
	std::vector<std::vector<Piece*>> piecesOnBoard;
	Board(SDL_Renderer* renderer = nullptr);
	~Board();
	
	bool movePiece(Piece* piece, int newX, int newY);
	void promotePawn(Piece* pawn, int choice);

	void undo();
	void redo();

};
