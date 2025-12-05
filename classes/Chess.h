#pragma once

#include <sstream>
#include "Game.h"
#include "Grid.h"
#include "ChessPiece.h"
#include "Bitboard.h"
#include "GameState.h"

constexpr int pieceSize = 80;

// enum ChessPiece
// {
//     NoPiece,
//     Pawn,
//     Knight,
//     Bishop,
//     Rook,
//     Queen,
//     King
// };

class Chess : public Game
{
public:
    Chess();
    ~Chess();

    void setUpBoard() override;

    bool canBitMoveFrom(Bit &bit, BitHolder &src) override;
    bool canBitMoveFromTo(Bit &bit, BitHolder &src, BitHolder &dst) override;
    bool actionForEmptyHolder(BitHolder &holder) override;

    void stopGame() override;

    Player *checkForWinner() override;
    bool checkForDraw() override;

    std::string initialStateString() override;
    std::string stateString() override;
    void setStateString(const std::string &s) override;

    Grid* getGrid() override { return _grid; };
    // Tournament support methods
    void setBoardFromFEN(const std::string& fen);
    BitMove getLastAIMove() const { return _lastAIMove; }
    std::string getFEN() const;
    // Get current player color (WHITE=1, BLACK=-1)
    int getCurrentPlayerColor() const { return _currentPlayer; }
    // you can make this variable private, it's just grouped with the public methods for convenience
    BitMove _lastAIMove; // Stores the last move calculated by AI (for tournament)

    void bitMovedFromTo(Bit &bit, BitHolder &src, BitHolder &dst) override;
    void updateAI();

private:
    Bit* PieceForPlayer(const int playerNumber, ChessPiece piece);
    Player* ownerAt(int x, int y) const;
    void FENtoBoard(const std::string& fen);
    char pieceNotation(int x, int y) const;
    std::vector<BitMove> _moves;  // Store current legal moves
    BitBoard generateKnightMoveBitboard(int square);
    BitBoard generateKingMoveBitboard(int square);
    BitBoard generatePawnCaptures(int square, bool isWhite);
    BitBoard generatePawnSingleMoves(int square, bool isWhite);
    BitBoard generatePawnDoubleMoves(int square, bool isWhite);
    BitBoard generateQueenMoveBitboard(int square);
    BitBoard generateBishopMoveBitboard(int square);
    BitBoard generateRookMoveBitboard(int square);
    bool isValidRookMove(int srcX, int srcY, int dstX, int dstY);
    bool isValidBishopMove(int srcX, int srcY, int dstX, int dstY);
    bool isValidQueenMove(int srcX, int srcY, int dstX, int dstY);
    Grid* _grid;
    BitBoard _knightBitboards[64];
    BitBoard _kingBitboards[64];
    BitBoard _pawnSingleMoves[2][64];
    BitBoard _pawnDoubleMoves[2][64];
    BitBoard _pawnCaptures[2][64];
    BitBoard _rookBitboards[64];
    BitBoard _bishopBitboards[64];
    BitBoard _queenBitboards[64];
    int negamax(std::string& state, int depth, int alpha, int beta, int playerColor);
    int evaluateBoard(const std::string& state);
    int _countMoves;
    int posInfinite = 10000;
    int negInfinite = -10000;
    int _currentPlayer = 0;
    GameState _gamestate;
};