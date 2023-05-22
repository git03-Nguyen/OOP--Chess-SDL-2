#pragma once
#include <vector>
#include <string>
#include <SDL.h>
#include <iostream>

#include "Pieces.h"

class Board {
private:
	void parseFEN(const char* fen);

public:
	SDL_Renderer* renderer;
	std::vector<std::vector<Piece*>> pieces;

	King* bKing;
	King* wKing;

	std::string getFEN() const;
	vector<string> history;
	vector<string> redoHistory;
	Color currentTurn;
	int halfMoveClock;
	int fullMoveClock;

	Board(vector<string> history);
	~Board();
	
	bool movePiece(Piece* piece, int newX, int newY);
	void updateTableMoves();
	void promotePawn(Piece* pawn, int choice);
	int checkWinLose() const;

	bool canUndo() const;
	bool canRedo() const;

	void undo();
	void redo();

};
