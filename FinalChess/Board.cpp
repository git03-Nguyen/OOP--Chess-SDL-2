#include "Board.h"

void Board::parseFEN(const char* fen) {
    if (!fen) return;

    pieces.resize(8);
    for (auto& row : pieces) {
        row.resize(8);
        for (auto& p : row) {
            if (p) {
                delete p;
                p = nullptr;
            }
        }
    }

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

    // Castling
    while (i < strlen(fen) && fen[i] != ' ') {
        switch (fen[i]) {
        case 'K':
            wKing->castlingK = dynamic_cast<Rook*>(pieces[7][7])->canCastling = true;
            break;

        case 'Q':
            wKing->castlingQ = dynamic_cast<Rook*>(pieces[0][7])->canCastling = true;
            break;

        case 'k':
            bKing->castlingK = dynamic_cast<Rook*>(pieces[7][0])->canCastling = true;
            break;

        case 'q':
            bKing->castlingQ = dynamic_cast<Rook*>(pieces[0][0])->canCastling = true;
            break;

        default:
            throw string("Invalid FEN string!");
        }
        i++;
    }
    i++;

    // En passant
    if (fen[i] != '-') {
        enPassantX = fen[i++] - 'a';
        enPassantY = 7 - (fen[i++] - '1');
        Piece* allyPawn = nullptr;
        int pY = (currentTurn == Color::White) ? enPassantY + 1 : enPassantY - 1;
        if (enPassantX < 7) {
            allyPawn = pieces[enPassantX + 1][pY];
            if (allyPawn && allyPawn->id == PieceID::Pawn && allyPawn->color == currentTurn) allyPawn->addMove(enPassantX, enPassantY);
        }
        if (enPassantX > 0) {
            allyPawn = pieces[enPassantX - 1][pY];
            if (allyPawn && allyPawn->id == PieceID::Pawn && allyPawn->color == currentTurn) allyPawn->addMove(enPassantX, enPassantY);
        }
        i++;
    }
    else {
        i += 2;
    }

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

    // Set kings
    for (auto& row : pieces) for (auto& p : row) {
        if (p) {
            (p->color == Color::White) ? p->setKing(wKing) : p->setKing(bKing);
        }
    }

    // Update moves
    for (auto& row : pieces) for (auto& p : row) {
        if (p) {
            p->updateTableMove(pieces);
        }
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

    if (wKing->castlingK) fen += "K";
    if (wKing->castlingQ) fen += "Q";
    if (bKing->castlingK) fen += "k";
    if (bKing->castlingQ) fen += "q";
    fen += " ";

    // Enpassant
    if (enPassantX == -1 || enPassantY == -1) fen += "- ";
    else {
        fen += char('a' + enPassantX);
        fen += char(7 - enPassantY + '1');
        fen += " ";
    }

    fen += to_string(halfMoveClock) + " ";
    fen += to_string(fullMoveClock);

    return fen;
}

Board::Board(vector<string> history) {
    
    if (!history.size()) this->history.push_back("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    else this->history = history;
    parseFEN(this->history.back().c_str());
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

MoveID Board::movePiece(Piece* piece, int newX, int newY) {
    if (piece->color != currentTurn) throw string("Wrong color piece!");
    if (pieces[newX][newY]) halfMoveClock = 0; else halfMoveClock++;
    
    MoveID type = piece->move(pieces, newX, newY);
    enPassantX = enPassantY = -1;

    if (type == MoveID::Kill) halfMoveClock = 0;

    // Promotion, we have to wait for choosing promotion
    if (type == MoveID::Promotion) return MoveID::Promotion;

    // Update table moves for enemies and check if can move
    bool canMove = false;
    for (int i = 0; i < 8; i++) for (int j = 0; j < 8; j++)
        if (pieces[i][j] && pieces[i][j]->color != currentTurn) {
            pieces[i][j]->updateTableMove(pieces);
            if (!canMove && pieces[i][j]->tableMove.size() > 0) canMove = true;
        }

    // First move, note the enpassant square
    if (type == MoveID::Pawn1st) {
        enPassantX = newX; 
        enPassantY = (piece->color == Color::White) ? newY + 1 : newY - 1;
        Piece* enemyPawn = nullptr;
        if (newX < 7) {
            enemyPawn = pieces[newX + 1][newY];
            if (enemyPawn && enemyPawn->id == PieceID::Pawn) enemyPawn->addMove(enPassantX, enPassantY);
        }
        if (newX > 0) {
            enemyPawn = pieces[newX - 1][newY];
            if (enemyPawn && enemyPawn->id == PieceID::Pawn) enemyPawn->addMove(enPassantX, enPassantY);
        }
        halfMoveClock = 0;
    }

    // If enpassant
    if (type == MoveID::EnPassant) halfMoveClock = 0;

    // Change the turn if not a match-move = the move leading to win/lose/drawn = after that move the enemy cannot move
    if (canMove) {
        currentTurn = (currentTurn == Color::White) ? Color::Black : Color::White;
    }
    else {
        type = MoveID::Match;
    }

    // Record the FEN
    redoHistory.clear();
    history.push_back(getFEN());
    cout << history.back() << endl;

    if (piece->color == Color::White) fullMoveClock++;
    return type;
}

void Board::updateTableMoves() {
    for (auto& row : pieces) {
        for (auto& piece : row) {
            if (piece) piece->updateTableMove(pieces);
        }
    }
}

MoveID Board::promotePawn(Piece* pawn, int newX, int newY, int choice) {
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

    std::cout << "Promotion successful!" << endl;
    return movePiece(newPiece, newX, newY);
}

MatchResult Board::getMatchResult() const {
    // Check half move clock -> Drawn
    if (halfMoveClock >= 50) return MatchResult::Drawn;
    
    // Check enemy can move or not
    bool canMove = false;
    for (int i = 0; i < 8 && !canMove; i++) for (int j = 0; j < 8 && !canMove; j++) {
        if (pieces[i][j] && pieces[i][j]->color != currentTurn && pieces[i][j]->tableMove.size() > 0) {
            canMove = true;
            break;
        }
    }

    King* king = (currentTurn == Color::White) ? bKing : wKing;
    if (!canMove) {
        // If enemy cannot move but king not being checked  -> Stalemate = Drawn
        if (!king->isBeingAttacked(pieces)) return MatchResult::Drawn;
        
        // If enemy cannot move but king is in check -> Checkmate -> Win/Lose
        return (currentTurn == Color::White) ? MatchResult::WhiteWin : MatchResult::BlackWin;
    }
    
    return MatchResult::None;
}

bool Board::canUndo() const {
    return history.size() > 1;
}

bool Board::canRedo() const {
    return redoHistory.size() > 0;
}

void Board::undo() {
    if (history.size() > 1) {

        redoHistory.push_back(history.back().c_str());
        history.pop_back();
        parseFEN(history.back().c_str());

    }
}

void Board::redo() {
    if (redoHistory.size() > 0) {

        history.push_back(redoHistory.back().c_str());
        parseFEN(redoHistory.back().c_str());
        redoHistory.pop_back();

    }
}


