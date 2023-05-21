#pragma once
#include <vector>
#include <SDL.h>
#include <iostream>

#include "Pieces.h"

class Board {
private:
	Piece* bKing;
	Piece* wKing;
	void parseFEN(const char* fen);

public:
	SDL_Renderer* renderer;
	std::vector<std::vector<Piece*>> piecesOnBoard;

	Board(const char* fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
	~Board();
	
	bool movePiece(Piece* piece, int newX, int newY);
	void promotePawn(Piece* pawn, int choice);

	void undo();
	void redo();

};
