#include "Board.h"

void Board::setStartBoard() {
    for (auto& row : piecesOnBoard) {
        for (auto& p : row) {
            p = nullptr;
        }
    }

    int index = 0;
    for (int y = 0; y < 2; y++) {
        for (int x = 0; x < 8; x++) {
            piecesOnBoard[x][y] = allPieces[index++];
        }
    }
    for (int y = 6; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            piecesOnBoard[x][y] = allPieces[index++];
        }
    }

    for (auto& p : allPieces) {
        updateTableMove(p);
    }
}

Board::Board() {
    allPieces.push_back(new Rook(0, 0, Color::Black));
    allPieces.push_back(new Knight(1, 0, Color::Black));
    allPieces.push_back(new Bishop(2, 0, Color::Black));
    allPieces.push_back(new Queen(3, 0, Color::Black));
    allPieces.push_back(new King(4, 0, Color::Black));
    allPieces.push_back(new Bishop(5, 0, Color::Black));
    allPieces.push_back(new Knight(6, 0, Color::Black));
    allPieces.push_back(new Rook(7, 0, Color::Black));
    for (int i = 0; i < 8; i++) {
        allPieces.push_back(new Pawn(i, 1, Color::Black));
    }

    for (int i = 0; i < 8; i++) {
        allPieces.push_back(new Pawn(i, 6, Color::White));
    }
    allPieces.push_back(new Rook(0, 7, Color::White));
    allPieces.push_back(new Knight(1, 7, Color::White));
    allPieces.push_back(new Bishop(2, 7, Color::White));
    allPieces.push_back(new Queen(3, 7, Color::White));
    allPieces.push_back(new King(4, 7, Color::White));
    allPieces.push_back(new Bishop(5, 7, Color::White));
    allPieces.push_back(new Knight(6, 7, Color::White));
    allPieces.push_back(new Rook(7, 7, Color::White));
    
    piecesOnBoard.resize(8);
    for (auto& row : piecesOnBoard) {
        row.resize(8);
    }
    setStartBoard();
}

Board::~Board() {
    for (auto& row : piecesOnBoard) {
        for (auto& piece : row) {
            if (piece) delete piece;
        }
    }
}

bool Board::movePiece(Piece* piece, int newX, int newY) {
    if (piece->posX == newX && piece->posY == newY) return false;
    if (Piece* desPiece = piecesOnBoard[newX][newY]) {
        desPiece->setDead();
        std::cout << desPiece->imagePath << " is dead!" << std::endl;
    }
    piecesOnBoard[piece->posX][piece->posY] = nullptr;
    piece->posX = newX;
    piece->posY = newY;
    piecesOnBoard[piece->posX][piece->posY] = piece;
    // If Rook/King => lose Castling
    // If Pawn => 
    for (auto& p : allPieces) {
        updateTableMove(p);
    }
    return true;
}

void Board::updateTableMove(Piece* piece) {
    piece->tableMove.clear();
    if (!piece->isAlive) return;
    int x, y, i, j;
    int s[] = { 1, 1, -1, -1, 1 , 0, 0};

    if (piece->id == PieceID::Pawn) {
        for (int x = 0; x < 8; x++) {
            for (int y = 0; y < 8; y++) {
                Piece* destPiece = piecesOnBoard[x][y];
                // Test
                if (!destPiece || destPiece->color != piece->color) {
                    piece->tableMove.push_back(x);
                    piece->tableMove.push_back(y);
                }
            }
        }
    }

    if (piece->id == PieceID::Bishop) { 
        i = 0; j = 1;
        for (int count = 0; count < 4; count++) {
            x = piece->posX; y = piece->posY;
            x += s[i]; y += s[i] * s[j];
            while (x < 8 && x >= 0 && y < 8 && y >= 0 && !piecesOnBoard[x][y]) {
                piece->addMove(x, y);
                x += s[i]; y += s[i] * s[j];
            }
            if (x < 8 && x >= 0 && y < 8 && y >= 0 && piecesOnBoard[x][y]->color != piece->color) {
                piece->addMove(x, y);
            }
            i++; j++;
        }
    }

    if (piece->id == PieceID::Rook) {
        i = 3; j = 6;
        for (int count = 0; count < 4; count++) {
            x = piece->posX; y = piece->posY;
            x += s[i]; y += s[j];
            while (x < 8 && x >= 0 && y < 8 && y >= 0 && !piecesOnBoard[x][y]) {
                piece->addMove(x, y);
                x += s[i]; y += s[j];
            }
            if (x < 8 && x >= 0 && y < 8 && y >= 0 && piecesOnBoard[x][y]->color != piece->color) {
                piece->addMove(x, y);
            }
            i++; j--;
        }
    }

    if (piece->id == PieceID::Queen) {
        // updateMove() of Bishop
        // updateMove() of Rook

    }

    if (piece->id == PieceID::Knight) {

    }

    // Check / Checkmate
}

