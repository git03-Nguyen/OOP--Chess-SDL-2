#pragma once
#include "Board.h"
#include <vector>
using namespace std;

enum class PlayerType { Human, ComAI };

class Player {
public:
    PlayerType type;
    Color color;
    Player(Color _color);
    ~Player();
    virtual void makeMove(Board* board) = 0;

};

// ----------------------------------------

class Human : public Player {
public:
    Human(Color _color);
    void makeMove(Board* board) override;
};

// ----------------------------------------

enum class Difficulty { RANDOM, HARD };

class ComAI : public Player {
public:
    Difficulty diff;
    ComAI(Color _color, Difficulty _diff = Difficulty::RANDOM);
    ~ComAI();

    void makeMove(Board* board) override;
    void makeRandomMove(Board* board) const;
    void makeHardMove(Board* board);

    int** evals[7];
    void initEvaluation();
    int evaluate(Board* board) const;

    int minimax(Board* board, int depth, int alpha, int beta, bool maximizingPlayer);

    void getBestMove(Piece*& piece, int& bestX, int& bestY, Board* board, int depth);
    

};


