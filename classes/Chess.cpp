#include "Chess.h"
#include "Bitboard.h"
#include <limits>
#include <cmath>

Chess::Chess()
{
    _grid = new Grid(8, 8);
    for (int i = 0; i<64; i++) {
        _knightBitboards[i] = generateKnightMoveBitboard(i);
    }
    for (int i = 0; i<64; i++) {
        _kingBitboards[i] = generateKingMoveBitboard(i);
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
            // std::cout << c << std::endl;
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
    int pieceType = gameTag & 0x7F; // Mask out color bit to get piece type (1-6)
    bool isWhite = (gameTag & 0x80) == 0; // Check if white piece
    
    // Convert coordinates to bitboard indices (0-63)
    int srcIndex = srcY * 8 + srcX;
    int dstIndex = dstY * 8 + dstX;
    
    // Check movement based on piece type
    switch (pieceType) {
        case 1: // Pawn
            return true;
            
        case 2: // Rook
            return true;
            
        case 3: // Knight
            return (_knightBitboards[srcIndex] & (1ULL << dstIndex)) != 0;
            
        case 4: // bishop
            return true;
            
        case 5: // Queen
            return true;
            
        case 6: // King
            return (_kingBitboards[srcIndex] & (1ULL << dstIndex)) != 0;
            
        default:
            return false;
    }
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