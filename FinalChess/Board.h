#pragma once
#include <vector>
#include <string>
#include <SDL.h>
#include <iostream>

#include "Pieces.h"

class Board {
private:
	King* bKing;
	King* wKing;
	void parseFEN(const char* fen);

public:
	SDL_Renderer* renderer;
	std::vector<std::vector<Piece*>> pieces;
	std::string getFEN() const;
	Color currentTurn;
	int halfMoveClock;
	int fullMoveClock;

	Board(const char* fen);
	~Board();
	
	bool movePiece(Piece* piece, int newX, int newY);
	void promotePawn(Piece* pawn, int choice);
	int checkWinLose() const;

	void undo();
	void redo();

};
