#pragma once
#include "Board.h"

enum class PlayerType { Human, ComAI };

class Player {
public:
    PlayerType type;

    virtual void makeMove(Board& board) = 0;

};

// ----------------------------------------

class Human : public Player {
public:
    Human();
    void makeMove(Board& board) override;
};

// ----------------------------------------

enum class Difficulty { RANDOM, HARD };

class ComAI : public Player {
public:
    Difficulty difficulty;
    ComAI(Difficulty difficulty = Difficulty::RANDOM);
    void makeMove(Board& board) override;
    void makeRandomMove(Board& board) const;
    void makeHardMove(Board& board)const;


};


