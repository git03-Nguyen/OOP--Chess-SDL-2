#include "Board.h"

void Board::parseFEN(const char* fen) {    
    if (!fen) return;

    int i = 0, x = 0, y = 0;

    while (y < 8) {
        x = 0;
        while (x < 8) {
            if (fen[i] >= '0' && fen[i] <= '9') {
                x += fen[i++] - '0'; continue;
            } 
            
            switch (fen[i++]) {
            case 'r':
                piecesOnBoard[x][y] = new Rook(x, y, Color::Black); break;
            case 'n':
                piecesOnBoard[x][y] = new Knight(x, y, Color::Black); break;
            case 'b':
                piecesOnBoard[x][y] = new Bishop(x, y, Color::Black); break;
            case 'q':
                piecesOnBoard[x][y] = new Queen(x, y, Color::Black); break;
            case 'k':
                piecesOnBoard[x][y] = new King(x, y, Color::Black);
                if (!bKing) bKing = piecesOnBoard[x][y]; else throw string("Invalid number of Black King!");
                break;
            case 'p':
                piecesOnBoard[x][y] = new Pawn(x, y, Color::Black); break;
            case 'R':
                piecesOnBoard[x][y] = new Rook(x, y, Color::White); break;
            case 'N':
                piecesOnBoard[x][y] = new Knight(x, y, Color::White); break;
            case 'B':
                piecesOnBoard[x][y] = new Bishop(x, y, Color::White); break;
            case 'Q':
                piecesOnBoard[x][y] = new Queen(x, y, Color::White); break;
            case 'K':
                piecesOnBoard[x][y] = new King(x, y, Color::White);
                if (!wKing) wKing = piecesOnBoard[x][y]; else throw string("Invalid number of White King!");
                break;
            case 'P':
                piecesOnBoard[x][y] = new Pawn(x, y, Color::White); break;
            default:
                throw string("Invalid FEN string!");
                break;
            }
            x++;
        }

        if (fen[i] != '/' && fen[i] != ' ')
            throw string("Invalid FEN string!");
        i++; y++;
    }
    

}

Board::Board(const char* fen) {

    piecesOnBoard.resize(8);
    for (auto& row : piecesOnBoard) {
        row.resize(8);
        for (auto& p : row) {
            p = nullptr;
        }
    }

    parseFEN(fen);

    for (auto& row : piecesOnBoard) for (auto& p : row) {
        if (p) {
            (p->color == Color::White) ? p->setKing(wKing) : p->setKing(bKing);
            p->updateTableMove(piecesOnBoard);
        }
    }
    
}

Board::~Board() {
    for (auto& row : piecesOnBoard) {
        for (auto& piece : row) {
            if (piece) delete piece;
        }
    }
}

bool Board::movePiece(Piece* piece, int newX, int newY) {
    
    if (!piece->move(piecesOnBoard, newX, newY)) return false;

    for (auto& row : piecesOnBoard) {
        for (auto& piece : row) {
            if (piece) piece->updateTableMove(piecesOnBoard);
        }
    }

    if (bKing->isBeingAttacked(piecesOnBoard, bKing->color)) {
            cout << "BLACK in CHECK!" << endl;
    }
    if (wKing->isBeingAttacked(piecesOnBoard, wKing->color)) {
        cout << "WHITE in CHECK!" << endl;
    }

    return true;
}

void Board::promotePawn(Piece* pawn, int choice) {
    Piece* newPiece = nullptr;

    switch (choice) {
    case 1:
        newPiece = new Queen(pawn->posX, pawn->posY, pawn->color);
        break;
    case 2:
        newPiece = new Bishop(pawn->posX, pawn->posY, pawn->color);
        break;
    case 3:
        newPiece = new Knight(pawn->posX, pawn->posY, pawn->color);
        break;
    case 4:
        newPiece = new Rook(pawn->posX, pawn->posY, pawn->color);
        break;
    default:
        break;
    }

    if (!newPiece) throw string("Error: Unsuccessful promotion!");
    
    piecesOnBoard[pawn->posX][pawn->posY] = newPiece;
    newPiece->setKing((newPiece->color == Color::White) ? wKing : bKing);
    newPiece->setTexture(renderer);
    delete pawn;

    for (auto& row : piecesOnBoard) {
        for (auto& piece : row) {
            if (piece) piece->updateTableMove(piecesOnBoard);
        }
    }

    if (bKing->isBeingAttacked(piecesOnBoard, bKing->color)) {
        cout << "BLACK in CHECK!" << endl;
    }
    if (wKing->isBeingAttacked(piecesOnBoard, wKing->color)) {
        cout << "WHITE in CHECK!" << endl;
    }

    cout << "Promotion successful!" << endl;
}

void Board::undo() {
}

void Board::redo() {
}


