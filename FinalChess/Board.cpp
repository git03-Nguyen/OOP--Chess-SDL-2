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
                pieces[x][y] = new Rook(x, y, Color::Black);
                dynamic_cast<Rook*>(pieces[x][y])->canCastling = false;
                break;
            case 'n':
                pieces[x][y] = new Knight(x, y, Color::Black); break;
            case 'b':
                pieces[x][y] = new Bishop(x, y, Color::Black); break;
            case 'q':
                pieces[x][y] = new Queen(x, y, Color::Black); break;
            case 'k':
                pieces[x][y] = new King(x, y, Color::Black);
                bKing = dynamic_cast<King*>(pieces[x][y]);
                break;
            case 'p':
                pieces[x][y] = new Pawn(x, y, Color::Black); break;
            case 'R':
                pieces[x][y] = new Rook(x, y, Color::White);
                dynamic_cast<Rook*>(pieces[x][y])->canCastling = false;
                break;
            case 'N':
                pieces[x][y] = new Knight(x, y, Color::White); break;
            case 'B':
                pieces[x][y] = new Bishop(x, y, Color::White); break;
            case 'Q':
                pieces[x][y] = new Queen(x, y, Color::White); break;
            case 'K':
                pieces[x][y] = new King(x, y, Color::White);
                wKing = dynamic_cast<King*>(pieces[x][y]);
                break;
            case 'P':
                pieces[x][y] = new Pawn(x, y, Color::White); break;
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

    // Current Turn 
    currentTurn = (fen[i] == 'w') ? Color::White : Color::Black;
    i += 2;

    // Castling white
    while (i < strlen(fen) && fen[i] != ' ') {
        switch (fen[i]) {
        case 'K':
            wKing->canCastling = dynamic_cast<Rook*>(pieces[7][7])->canCastling = true;
            break;

        case 'Q':
            wKing->canCastling = dynamic_cast<Rook*>(pieces[0][7])->canCastling = true;
            break;

        case 'k':
            bKing->canCastling = dynamic_cast<Rook*>(pieces[7][0])->canCastling = true;
            break;

        case 'q':
            bKing->canCastling = dynamic_cast<Rook*>(pieces[0][0])->canCastling = true;
            break;

        default:
            throw string("Invalid FEN string!");
        }
        i++;
    }
    i++;

    // En passant
    if (fen[i] != '-') {
        while (i < strlen(fen) && fen[i] != ' ') {
            i++;
        }
    }
    else {
        i++;
    }
    i++;

    // Halfmove-clock -> 50-move-rule
    halfMoveClock = 0;
    while (i < strlen(fen) && fen[i] >= '0' && fen[i] <= '9') {
        halfMoveClock = halfMoveClock * 10 + (fen[i] - '0');
        i++;
    }
    i++;

    // Fullmove-clock 
    fullMoveClock = 0;
    while (i < strlen(fen) && fen[i] >= '0' && fen[i] <= '9') {
        fullMoveClock = fullMoveClock * 10 + (fen[i] - '0');
        i++;
    }
    


}

std::string Board::getFEN() const {
    string fen = "";
    int x = 0, y = 0, blank = 0;
    while (y < 8) {
        x = 0;
        while (x < 8) {
            if (!pieces[x][y]) blank++;
            else {
                if (blank) {
                    fen += (blank + '0');
                    blank = 0;
                }
                switch (pieces[x][y]->id) {
                case PieceID::Rook:
                    fen += (pieces[x][y]->color == Color::White) ? 'R' : 'r';
                    break;
                case PieceID::Knight:
                    fen += (pieces[x][y]->color == Color::White) ? 'N' : 'n';
                    break;
                case PieceID::Bishop:
                    fen += (pieces[x][y]->color == Color::White) ? 'B' : 'b';
                    break;
                case PieceID::Queen:
                    fen += (pieces[x][y]->color == Color::White) ? 'Q' : 'q';
                    break;
                case PieceID::King:
                    fen += (pieces[x][y]->color == Color::White) ? 'K' : 'k';
                    break;
                case PieceID::Pawn:
                    fen += (pieces[x][y]->color == Color::White) ? 'P' : 'p';
                    break;
                }
            }
            x++;
        }
        if (blank) {
            fen += (blank + '0');
            blank = 0;
        }
        fen += '/';
        y++;
    }

    fen.pop_back();
    fen += ' ';

    fen += (currentTurn == Color::White) ? 'w' : 'b';
    fen += " ";

    for (int i = 0; i <= 7; i += 7) for (int j = 0; j <= 7; j += 7) {
        Rook* rook = dynamic_cast<Rook*>(pieces[j][i]);
        if (rook && rook->canCastling) {
            King* king = (rook->color == Color::White) ? wKing : bKing;
            if (king->canCastling) {
                if (rook->posX < king->posX) fen += (rook->color == Color::White) ? 'Q' : 'q';
                else fen += (rook->color == Color::White) ? 'K' : 'k';
            }
        }
    }
    

    // Enpassant
    fen += " - ";

    fen += to_string(halfMoveClock) + " ";

    fen += to_string(fullMoveClock);

    return fen;
}

Board::Board(vector<string> history) {

    pieces.resize(8);
    for (auto& row : pieces) {
        row.resize(8);
        for (auto& p : row) {
            p = nullptr;
        }
    }
    
    if (!history.size()) history.push_back("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    this->history = history;
    
    parseFEN(history.back().c_str());

    for (auto& row : pieces) for (auto& p : row) {
        if (p) {
            (p->color == Color::White) ? p->setKing(wKing) : p->setKing(bKing);
            p->updateTableMove(pieces);
        }
    }
    
    redoHistory = vector<string>();
}

Board::~Board() {
    for (auto& row : pieces) {
        for (auto& piece : row) {
            if (piece) delete piece;
        }
    }
    history.clear();
}

bool Board::movePiece(Piece* piece, int newX, int newY) {
    if (pieces[newX][newY]) halfMoveClock = 0; else halfMoveClock++;
    
    if (!piece->move(pieces, newX, newY)) return false;

    for (auto& row : pieces) {
        for (auto& piece : row) {
            if (piece) piece->updateTableMove(pieces);
        }
    }

    if (bKing->isBeingAttacked(pieces, bKing->color)) {
            cout << "BLACK in CHECK!" << endl;
    }
    if (wKing->isBeingAttacked(pieces, wKing->color)) {
        cout << "WHITE in CHECK!" << endl;
    }

    if (piece->color == Color::Black) fullMoveClock++;
    
    currentTurn = (Color)!bool(currentTurn);
    if (piece->id != PieceID::Pawn || (piece->posY != 0 && piece->posY != 7)) {
        redoHistory.clear();
        history.push_back(getFEN());
        cout << history.back() << endl;
    }
    currentTurn = (Color)!bool(currentTurn);
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
    
    pieces[pawn->posX][pawn->posY] = newPiece;
    newPiece->setKing((newPiece->color == Color::White) ? wKing : bKing);
    newPiece->setTexture(renderer);
    delete pawn;

    for (auto& row : pieces) {
        for (auto& piece : row) {
            if (piece) piece->updateTableMove(pieces);
        }
    }

    if (bKing->isBeingAttacked(pieces, bKing->color)) {
        cout << "BLACK in CHECK!" << endl;
    }
    if (wKing->isBeingAttacked(pieces, wKing->color)) {
        cout << "WHITE in CHECK!" << endl;
    }

    redoHistory.clear();
    history.push_back(getFEN());
    cout << history.back() << endl;
    cout << "Promotion successful!" << endl;
}

int Board::checkWinLose() const {
    // Check half move clock
    // if (halfMoveClock >= 50) => Drawn match
    
    // Check win/lose
    bool runOutOfMove = true;
    for (int i = 0; i < 8 && runOutOfMove; i++) {
        for (int j = 0; j < 8 && runOutOfMove; j++) {
            runOutOfMove = !((pieces[i][j] && pieces[i][j]->color != currentTurn
                && pieces[i][j]->tableMove.size() != 0));
        }
    }
    if (!runOutOfMove) return 0;
    return (currentTurn == Color::White) ? 1 : -1;
}

bool Board::canUndo() const {
    return history.size() > 1;
}

bool Board::canRedo() const {
    return redoHistory.size() > 0;
}

void Board::undo() {
    if (history.size() > 1) {
        for (auto& row : pieces) for (auto& p : row) {
            if (p) {
                delete p;
                p = nullptr;
            }
        }

        redoHistory.push_back(history.back().c_str());
        history.pop_back();
        parseFEN(history.back().c_str());

        for (auto& row : pieces) for (auto& p : row) {
            if (p) {
                (p->color == Color::White) ? p->setKing(wKing) : p->setKing(bKing);
                p->updateTableMove(pieces);
            }
        }

    }
}

void Board::redo() {
    if (redoHistory.size() > 0) {
        for (auto& row : pieces) for (auto& p : row) {
            if (p) {
                delete p;
                p = nullptr;
            }
        }

        history.push_back(redoHistory.back().c_str());
        parseFEN(redoHistory.back().c_str());
        redoHistory.pop_back();

        for (auto& row : pieces) for (auto& p : row) {
            if (p) {
                (p->color == Color::White) ? p->setKing(wKing) : p->setKing(bKing);
                p->updateTableMove(pieces);
            }
        }

    }
}


