#pragma once
#include <vector>
#include <string>
#include <SDL.h>
#include <iostream>

#include "Pieces.h"

enum class MatchResult { Drawn, WhiteWin, BlackWin, None };

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
	int enPassantX = -1, enPassantY = -1;

	Board(vector<string> history);
	~Board();
	
	MoveID movePiece(Piece* piece, int newX, int newY);
	MoveID promotePawn(Piece* pawn, int newX, int newY, int choice);
	void updateTableMoves();
	MatchResult getMatchResult() const;

	bool canUndo() const;
	bool canRedo() const;

	void undo();
	void redo();

};
