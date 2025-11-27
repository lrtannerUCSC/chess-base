#include "Chess.h"
#include "Bitboard.h"
#include <limits>
#include <cmath>

const BitboardElement NotAFile(0xFEFEFEFEFEFEFEFEULL);
const BitboardElement NotHFile(0x7F7F7F7F7F7F7F7FULL);
const BitboardElement rank3(0x0000000000FF0000ULL);
const BitboardElement rank6(0x0000FF0000000000ULL);
Chess::Chess()
{
    _grid = new Grid(8, 8);
    for (int i = 0; i < 64; i++) {
        _knightBitboards[i] = generateKnightMoveBitboard(i);
        _kingBitboards[i] = generateKingMoveBitboard(i);
        
        _pawnSingleMoves[0][i] = generatePawnSingleMoves(i, true);
        _pawnSingleMoves[1][i] = generatePawnSingleMoves(i, false);
        
        _pawnDoubleMoves[0][i] = generatePawnDoubleMoves(i, true);
        _pawnDoubleMoves[1][i] = generatePawnDoubleMoves(i, false);
        
        _pawnCaptures[0][i] = generatePawnCaptures(i, true);
        _pawnCaptures[1][i] = generatePawnCaptures(i, false);

        _rookBitboards[i] = generateRookMoveBitboard(i);
        _bishopBitboards[i] = generateBishopMoveBitboard(i);
        _queenBitboards[i] = generateQueenMoveBitboard(i);
    }
    


}

Chess::~Chess()
{
    delete _grid;
}

char Chess::pieceNotation(int x, int y) const
{
    const char *wpieces = { "0PNBRQK" };
    const char *bpieces = { "0pnbrqk" };
    Bit *bit = _grid->getSquare(x, y)->bit(); 
    char notation = '0';
    if (bit) {
        notation = bit->gameTag() < 128 ? wpieces[bit->gameTag()] : bpieces[bit->gameTag()-128];
    }
    return notation;
}

Bit* Chess::PieceForPlayer(const int playerNumber, ChessPiece piece)
{
    const char* pieces[] = { "pawn.png", "knight.png", "bishop.png", "rook.png", "queen.png", "king.png" };

    Bit* bit = new Bit();
    // should possibly be cached from player class?
    const char* pieceName = pieces[piece - 1];
    std::string spritePath = std::string("") + (playerNumber == 0 ? "w_" : "b_") + pieceName;
    bit->LoadTextureFromFile(spritePath.c_str());
    bit->setOwner(getPlayerAt(playerNumber));
    bit->setSize(pieceSize, pieceSize);

    return bit;
}

void Chess::setUpBoard()
{
    setNumberOfPlayers(2);
    _gameOptions.rowX = 8;
    _gameOptions.rowY = 8;

    _grid->initializeChessSquares(pieceSize, "boardsquare.png");
    FENtoBoard("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR");

    startGame();
}

void Chess::FENtoBoard(const std::string& fen) {
    // convert a FEN string to a board
    // FEN is a space delimited string with 6 fields
    // 1: piece placement (from white's perspective)
    // NOT PART OF THIS ASSIGNMENT BUT OTHER THINGS THAT CAN BE IN A FEN STRING
    // ARE BELOW
    // 2: active color (W or B)
    // 3: castling availability (KQkq or -)
    // 4: en passant target square (in algebraic notation, or -)
    // 5: halfmove clock (number of halfmoves since the last capture or pawn advance)
    int i = 0;
    for (char c : fen) {
        if (c == 'p') {
            Bit* piece = PieceForPlayer(1, Pawn);
            piece->setGameTag(1 | 128);
            piece->setPosition(_grid->getSquare(i % 8, i / 8)->getPosition());
            _grid->getSquare(i % 8, i / 8)->setBit(piece);
            i++;
        }
        if (c == 'P') {
            Bit* piece = PieceForPlayer(0, Pawn);
            piece->setGameTag(1);
            piece->setPosition(_grid->getSquare(i % 8, i / 8)->getPosition());
            _grid->getSquare(i % 8, i / 8)->setBit(piece);
            i++;
        }
        if (c == 'r') {
            Bit* piece = PieceForPlayer(1, Rook);
            piece->setGameTag(2 | 128);
            piece->setPosition(_grid->getSquare(i % 8, i / 8)->getPosition());
            _grid->getSquare(i % 8, i / 8)->setBit(piece);
            i++;
        }
        if (c == 'R') {
            Bit* piece = PieceForPlayer(0, Rook);
            piece->setGameTag(2);
            piece->setPosition(_grid->getSquare(i % 8, i / 8)->getPosition());
            _grid->getSquare(i % 8, i / 8)->setBit(piece);
            i++;
        }
        if (c == 'n') {
            Bit* piece = PieceForPlayer(1, Knight);
            piece->setGameTag(3 | 128);
            piece->setPosition(_grid->getSquare(i % 8, i / 8)->getPosition());
            _grid->getSquare(i % 8, i / 8)->setBit(piece);
            i++;
        }
        if (c == 'N') {
            Bit* piece = PieceForPlayer(0, Knight);
            piece->setGameTag(3);
            piece->setPosition(_grid->getSquare(i % 8, i / 8)->getPosition());
            _grid->getSquare(i % 8, i / 8)->setBit(piece);
            i++;
        }
        if (c == 'b') {
            Bit* piece = PieceForPlayer(1, Bishop);
            piece->setGameTag(4 | 128);
            piece->setPosition(_grid->getSquare(i % 8, i / 8)->getPosition());
            _grid->getSquare(i % 8, i / 8)->setBit(piece);
            i++;
        }
        if (c == 'B') {
            Bit* piece = PieceForPlayer(0, Bishop);
            piece->setGameTag(4);
            piece->setPosition(_grid->getSquare(i % 8, i / 8)->getPosition());
            _grid->getSquare(i % 8, i / 8)->setBit(piece);
            i++;
        }
        if (c == 'q') {
            Bit* piece = PieceForPlayer(1, Queen);
            piece->setGameTag(5 | 128);
            piece->setPosition(_grid->getSquare(i % 8, i / 8)->getPosition());
            _grid->getSquare(i % 8, i / 8)->setBit(piece);
            i++;
        }
        if (c == 'Q') {
            Bit* piece = PieceForPlayer(0, Queen);
            piece->setGameTag(5);
            piece->setPosition(_grid->getSquare(i % 8, i / 8)->getPosition());
            _grid->getSquare(i % 8, i / 8)->setBit(piece);
            i++;
        }
        if (c == 'k') {
            Bit* piece = PieceForPlayer(1, King);
            piece->setGameTag(6 | 128);
            piece->setPosition(_grid->getSquare(i % 8, i / 8)->getPosition());
            _grid->getSquare(i % 8, i / 8)->setBit(piece);
            i++;
        }
        if (c == 'K') {
            Bit* piece = PieceForPlayer(0, King);
            piece->setGameTag(6);
            piece->setPosition(_grid->getSquare(i % 8, i / 8)->getPosition());
            _grid->getSquare(i % 8, i / 8)->setBit(piece);
            i++;
        }
        
        if (c == '/') {
            continue;
        }
        if (std::isdigit(c) && c != '0') {
            i += c - '0';
        }
    }
}

bool Chess::actionForEmptyHolder(BitHolder &holder)
{
    return false;
}

bool Chess::canBitMoveFrom(Bit &bit, BitHolder &src)
{
    // need to implement friendly/unfriendly in bit so for now this hack
    int currentPlayer = getCurrentPlayer()->playerNumber() * 128;
    int pieceColor = bit.gameTag() & 128;
    if (pieceColor == currentPlayer) return true;
    return false;
}

bool Chess::canBitMoveFromTo(Bit &bit, BitHolder &src, BitHolder &dst)
{
    // Get the source and destination squares
    ChessSquare* srcSquare = dynamic_cast<ChessSquare*>(&src);
    ChessSquare* dstSquare = dynamic_cast<ChessSquare*>(&dst);
    
    if (!srcSquare || !dstSquare) {
        return false;
    }
    
    // Get coordinates
    int srcX = srcSquare->getColumn();
    int srcY = srcSquare->getRow();
    int dstX = dstSquare->getColumn();
    int dstY = dstSquare->getRow();
    
    // Get piece type from gameTag
    int gameTag = bit.gameTag();
    int pieceType = gameTag & 0x7F;
    bool isWhite = (gameTag & 0x80) == 0;
    int colorIndex = isWhite ? 0 : 1;
    
    // Convert coordinates to bitboard indices (0-63)
    int srcIndex = srcY * 8 + srcX;
    int dstIndex = dstY * 8 + dstX;
    
    ChessSquare* intermediateSquare;
    int intermediateRank;
    bool isCapture;
    bool singleMove;
    bool doubleMove;
    
    // Check movement based on piece type
    switch (pieceType) {
        case 1: // Pawn
            dstSquare = _grid->getSquare(dstX, dstY);
            isCapture = (dstSquare->bit() != nullptr) && 
                           (dstSquare->bit()->getOwner() != bit.getOwner());
            
            if (isCapture) {
                return (_pawnCaptures[colorIndex][srcIndex] & (1ULL << dstIndex)) != 0;
            } else {
                singleMove = (_pawnSingleMoves[colorIndex][srcIndex] & (1ULL << dstIndex)) != 0;
                doubleMove = (_pawnDoubleMoves[colorIndex][srcIndex] & (1ULL << dstIndex)) != 0;
                
                if (doubleMove) {
                    intermediateRank = isWhite ? srcY - 1 : srcY + 1;
                    intermediateSquare = _grid->getSquare(srcX, intermediateRank);
                    if (intermediateSquare->bit() != nullptr) {
                        return false; // Path is blocked
                    }
                }
                
                if (dstSquare->bit() != nullptr) {
                    return false;
                }
                
                return singleMove || doubleMove;
            }
            
        case 2: // Rook
            return isValidRookMove(srcX, srcY, dstX, dstY);
            
        case 3: // Knight
            return (_knightBitboards[srcIndex] & (1ULL << dstIndex)) != 0;
            
        case 4: // Bishop
            return isValidBishopMove(srcX, srcY, dstX, dstY);
            
        case 5: // Queen
            return isValidQueenMove(srcX, srcY, dstX, dstY);
            
        case 6: // King
            return (_kingBitboards[srcIndex] & (1ULL << dstIndex)) != 0;
            
        default:
            return false;
    }
}

bool Chess::isValidRookMove(int srcX, int srcY, int dstX, int dstY) {
    // Check if it's a valid rook direction (straight line)
    if (srcX != dstX && srcY != dstY) {
        return false;
    }
    
    // Determine direction
    int dx = (dstX > srcX) ? 1 : (dstX < srcX) ? -1 : 0;
    int dy = (dstY > srcY) ? 1 : (dstY < srcY) ? -1 : 0;
    
    // Check each square along the path
    int x = srcX + dx;
    int y = srcY + dy;
    
    while (x != dstX || y != dstY) {
        ChessSquare* square = _grid->getSquare(x, y);
        if (square->bit() != nullptr) {
            return false; // Path is blocked
        }
        x += dx;
        y += dy;
    }
    
    return true;
}

bool Chess::isValidBishopMove(int srcX, int srcY, int dstX, int dstY) {
    // Check if it's a valid bishop direction (diagonal)
    int deltaX = std::abs(dstX - srcX);
    int deltaY = std::abs(dstY - srcY);
    if (deltaX != deltaY) {
        return false;
    }
    
    // Determine direction
    int dx = (dstX > srcX) ? 1 : -1;
    int dy = (dstY > srcY) ? 1 : -1;
    
    // Check each square along the path
    int x = srcX + dx;
    int y = srcY + dy;
    
    while (x != dstX && y != dstY) {
        ChessSquare* square = _grid->getSquare(x, y);
        if (square->bit() != nullptr) {
            return false; // Path is blocked
        }
        x += dx;
        y += dy;
    }
    
    return true;
}

bool Chess::isValidQueenMove(int srcX, int srcY, int dstX, int dstY) {
    // Queen moves are either rook moves or bishop moves
    return isValidRookMove(srcX, srcY, dstX, dstY) || 
           isValidBishopMove(srcX, srcY, dstX, dstY);
}

void Chess::stopGame()
{
    _grid->forEachSquare([](ChessSquare* square, int x, int y) {
        square->destroyBit();
    });
}

Player* Chess::ownerAt(int x, int y) const
{
    if (x < 0 || x >= 8 || y < 0 || y >= 8) {
        return nullptr;
    }

    auto square = _grid->getSquare(x, y);
    if (!square || !square->bit()) {
        return nullptr;
    }
    return square->bit()->getOwner();
}

Player* Chess::checkForWinner()
{
    return nullptr;
}

bool Chess::checkForDraw()
{
    return false;
}

std::string Chess::initialStateString()
{
    return stateString();
}

std::string Chess::stateString()
{
    std::string s;
    s.reserve(64);
    _grid->forEachSquare([&](ChessSquare* square, int x, int y) {
            s += pieceNotation( x, y );
        }
    );
    return s;}

void Chess::setStateString(const std::string &s)
{
    _grid->forEachSquare([&](ChessSquare* square, int x, int y) {
        int index = y * 8 + x;
        char playerNumber = s[index] - '0';
        if (playerNumber) {
            square->setBit(PieceForPlayer(playerNumber - 1, Pawn));
        } else {
            square->setBit(nullptr);
        }
    });
}

BitBoard Chess::generateKnightMoveBitboard(int square) {
    BitBoard bitboard = 0ULL;
    int rank = square / 8;
    int file = square % 8;

    std::pair<int, int> knightOffsets[] = {
        {2, 1}, {2, -1}, {1, 2}, {1, -2}, {-1, 2}, {-1, -2}, {-2, 1}, {-2, -1}
    };

    constexpr uint64_t oneBit = 1;
    for (auto [dr, df] : knightOffsets) {
        int r = rank + dr;
        int f = file + df;
        if (r >= 0 && r < 8 && f >= 0 && f < 8) {
            bitboard |= oneBit << (r * 8 + f);
        }
    }
    return bitboard;
}

BitBoard Chess::generateKingMoveBitboard(int square) {
    BitBoard bitboard = 0ULL;
    int rank = square / 8;
    int file = square % 8;

    std::pair<int, int> knightOffsets[] = {
        {1, 0}, {-1, 0}, {0, 1}, {0, -1}, {1, 1}, {1, -1}, {-1, 1}, {-1, -1}
    };

    constexpr uint64_t oneBit = 1;
    for (auto [dr, df] : knightOffsets) {
        int r = rank + dr;
        int f = file + df;
        if (r >= 0 && r < 8 && f >= 0 && f < 8) {
            bitboard |= oneBit << (r * 8 + f);
        }
    }
    return bitboard;
}

BitBoard Chess::generatePawnSingleMoves(int square, bool isWhite) {
    BitBoard bitboard = 0ULL;
    int rank = square / 8;
    int file = square % 8;
    
    if (isWhite) {
        if (rank < 7) {
            bitboard |= 1ULL << ((rank - 1) * 8 + file);
        }
    } else {
        if (rank > 0) {
            bitboard |= 1ULL << ((rank + 1) * 8 + file);
        }
    }
    
    return bitboard;
}

BitBoard Chess::generatePawnDoubleMoves(int square, bool isWhite) {
    BitBoard bitboard = 0ULL;
    int rank = square / 8;
    int file = square % 8;
    
    if (isWhite) {
        if (rank == 6) {
            bitboard |= 1ULL << ((rank - 2) * 8 + file);
        }
    } else {
        if (rank == 1) {
            bitboard |= 1ULL << ((rank + 2) * 8 + file);
        }
    }
    
    return bitboard;
}

BitBoard Chess::generatePawnCaptures(int square, bool isWhite) {
    BitBoard bitboard = 0ULL;
    int rank = square / 8;
    int file = square % 8;
    
    if (isWhite) {
        if (rank < 7) {
            if (file > 0) { // capture left
                bitboard |= 1ULL << ((rank - 1) * 8 + (file + 1));
            }
            if (file < 7) { // capture right
                bitboard |= 1ULL << ((rank - 1) * 8 + (file - 1));
            }
        }
    } else {
        if (rank > 0) {
            if (file > 0) { // capture left
                bitboard |= 1ULL << ((rank + 1) * 8 + (file + 1));
            }
            if (file < 7) { // capture right
                bitboard |= 1ULL << ((rank + 1) * 8 + (file - 1));
            }
        }
    }
    
    return bitboard;
}

BitBoard Chess::generateRookMoveBitboard(int square) {
    BitBoard bitboard = 0ULL;
    int rank = square / 8;
    int file = square % 8;
    
    // Directions: up, down, left, right
    std::pair<int, int> directions[] = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};
    
    for (auto [dr, df] : directions) {
        int r = rank + dr;
        int f = file + df;
        
        // Move in this direction until we hit the board edge
        while (r >= 0 && r < 8 && f >= 0 && f < 8) {
            bitboard |= 1ULL << (r * 8 + f);
            r += dr;
            f += df;
        }
    }
    
    return bitboard;
}

BitBoard Chess::generateBishopMoveBitboard(int square) {
    BitBoard bitboard = 0ULL;
    int rank = square / 8;
    int file = square % 8;
    
    // Directions: up-right, up-left, down-right, down-left
    std::pair<int, int> directions[] = {{1, 1}, {1, -1}, {-1, 1}, {-1, -1}};
    
    for (auto [dr, df] : directions) {
        int r = rank + dr;
        int f = file + df;
        
        // Move in this direction until we hit the board edge
        while (r >= 0 && r < 8 && f >= 0 && f < 8) {
            bitboard |= 1ULL << (r * 8 + f);
            r += dr;
            f += df;
        }
    }
    
    return bitboard;
}

BitBoard Chess::generateQueenMoveBitboard(int square) {
    // Queen moves are just rook moves OR bishop moves
    return generateRookMoveBitboard(square) | generateBishopMoveBitboard(square);
}