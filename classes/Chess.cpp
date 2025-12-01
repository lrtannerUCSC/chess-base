#include "Chess.h"
#include "Bitboard.h"
#include "GameState.h"
#include <limits>
#include <cmath>
#include <vector>
#include <map>

static std::map<char, int> evaluateScores = {
    {'P', 100}, {'p', -100},    // Pawns
    {'N', 200}, {'n', -200},    // Knights
    {'B', 230}, {'b', -230},    // Bishops
    {'R', 400}, {'r', -400},    // Rooks
    {'Q', 900}, {'q', -900},    // Queens
    {'K', 2000}, {'k', -2000},  // Kings
    {'0', 0}                     // Empty squares
};

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
    const char *wpieces = { "0PRNBQK" };
    const char *bpieces = { "0prnbqk" };
    Bit *bit = _grid->getSquare(x, y)->bit(); 
    char notation = '0';
    if (bit) {
        notation = bit->gameTag() < 128 ? wpieces[bit->gameTag()] : bpieces[bit->gameTag()-128];
    }
    return notation;
}

Bit* Chess::PieceForPlayer(const int playerNumber, ChessPiece piece)
{
    const char* pieces[] = { "pawn.png", "rook.png", "knight.png", "bishop.png", "queen.png", "king.png" };

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

    setAIPlayer(1);
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
    
    // Convert coordinates to bitboard indices (0-63)
    int srcIndex = srcY * 8 + srcX;
    int dstIndex = dstY * 8 + dstX;
    
    // Get piece type from gameTag
    int gameTag = bit.gameTag();
    int pieceType = gameTag & 0x7F;
    
    // First, check if it's the current player's turn
    if (!canBitMoveFrom(bit, src)) {
        return false;
    }
    
    // Check if destination has a friendly piece
    if (dstSquare->bit() && dstSquare->bit()->getOwner() == bit.getOwner()) {
        return false; // Can't capture your own piece
    }
    
    // Get current board state
    std::string currentState = stateString();
    
    // Create GameState object with current position
    GameState gameState;
    char currentPlayer = (getCurrentPlayer()->playerNumber() == 0) ? WHITE : BLACK;
    gameState.init(currentState.c_str(), currentPlayer);
    
    // Generate all legal moves for current position
    auto legalMoves = gameState.generateAllMoves();
    
    // Check if the requested move is in the list of legal moves
    for (const auto& move : legalMoves) {
        if (move.from == srcIndex && move.to == dstIndex) {
            // We found a legal move from src to dst
            return true;
        }
    }
    
    return false;
}

void Chess::bitMovedFromTo(Bit& bit, BitHolder& src, BitHolder& dst) {
    // Clear any cached moves
    // _cachedMoves.clear();
    
    // Handle piece capture
    ChessSquare* dstSquare = dynamic_cast<ChessSquare*>(&dst);
    if (dstSquare && dstSquare->bit() && dstSquare->bit()->getOwner() != bit.getOwner()) {
        // Capture the piece
        std::cout << "Capture!" << std::endl;
        delete dstSquare->bit();
        dstSquare->setBit(nullptr);
    }
    
    // Update the bit's position
    bit.setPosition(dst.getPosition());
    
    // Switch to the next player (Game::bitMovedFromTo calls endTurn, but we need to handle AI)
    Player* current = getCurrentPlayer();
    
    // Call parent class to handle turn ending
    Game::bitMovedFromTo(bit, src, dst);
    
    // Check for pawn promotion
    int gameTag = bit.gameTag();
    int pieceType = gameTag & 0x7F;
    bool isWhite = (gameTag & 0x80) == 0;
    
    if (pieceType == Pawn) {
        ChessSquare* square = dynamic_cast<ChessSquare*>(&dst);
        if (square) {
            int row = square->getRow();
            // Check if pawn reached the last rank
            if ((isWhite && row == 0) || (!isWhite && row == 7)) {
                // Pawn promotion - promote to queen
                bit.setGameTag(isWhite ? Queen : (Queen | 128));
                
                // Update the sprite
                const char* pieceName = "queen.png";
                std::string spritePath = std::string("") + (isWhite ? "w_" : "b_") + pieceName;
                bit.LoadTextureFromFile(spritePath.c_str());
                std::cout << "Pawn promoted to Queen!" << std::endl;
            }
        }
    }
    
    // Debug output
    ChessSquare* srcSquare = dynamic_cast<ChessSquare*>(&src);
    if (srcSquare && dstSquare) {
        char srcFile = 'a' + srcSquare->getColumn();
        char dstFile = 'a' + dstSquare->getColumn();
        int srcRank = srcSquare->getRow() + 1;
        int dstRank = dstSquare->getRow() + 1;
        
        // std::cout << "Move made: " << srcFile << srcRank << " to " 
        //           << dstFile << dstRank << std::endl;
    }
    
    // Now trigger AI move if it's AI's turn
    // Assuming AI is Player 1 (Black)
    if (getCurrentPlayer()->playerNumber() == 1) {
        // std::cout << "\nAI's turn (Black)..." << std::endl;
        
        
        // Generate current legal moves for AI
        GameState gameState;
        std::string currentState = stateString();
        gameState.init(currentState.c_str(), BLACK); // AI is black
        
        _moves = gameState.generateAllMoves();
        
        if (_moves.empty()) {
            std::cout << "AI has no legal moves!" << std::endl;
            return;
        }
        
        // std::cout << "AI evaluating " << _moves.size() << " moves..." << std::endl;
        
        // Call updateAI to make the AI move
        updateAI();
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

int Chess::negamax(std::string& state, int depth, int alpha, int beta, int playerColor)
{
    _countMoves++;

    // Base case: at leaf nodes, evaluate the position
    if (depth == 0) {
        return evaluateBoard(state) * playerColor;
    }

    // Create a GameState object with the current state
    GameState gameState;
    char colorChar = (playerColor == WHITE) ? WHITE : BLACK;  // WHITE = 1, BLACK = -1
    gameState.init(state.c_str(), colorChar);
    
    // Generate moves using the GameState object
    auto newMoves = gameState.generateAllMoves();  // Use GameState directly

    if (newMoves.empty()) {
        // Checkmate or stalemate
        return evaluateBoard(state) * playerColor;
    }

    int bestVal = negInfinite;

    for(auto& move : newMoves) {
        // Save the board state
        char boardSave = state[move.to];
        char pieceMoving = state[move.from];

        // Make the move
        state[move.to] = pieceMoving;
        state[move.from] = '0';

        // Recursively evaluate
        bestVal = std::max(bestVal, -negamax(state, depth - 1, -beta, -alpha, -playerColor));

        // Undo the move
        state[move.from] = pieceMoving;
        state[move.to] = boardSave;

        // Alpha-beta pruning
        alpha = std::max(alpha, bestVal);
        if (alpha >= beta) {
            break;
        }
    }

    return bestVal;
}


void Chess::updateAI()
{
    int bestVal = negInfinite;
    BitMove bestMove;
    std::string state = stateString();
    _countMoves = 0;

    // Generate current legal moves for AI (should be Black's turn)
    GameState gameState;
    gameState.init(state.c_str(), BLACK);  // AI is Black
    
    // Get fresh moves
    _moves = gameState.generateAllMoves();
    
    if (_moves.empty()) {
        std::cout << "AI has no legal moves!" << std::endl;
        return;
    }

    // std::cout << "AI searching through " << _moves.size() << " moves..." << std::endl;

    // Search through current legal moves
    for(auto& move : _moves) {
        // Save the board state
        char boardSave = state[move.to];
        char pieceMoving = state[move.from];

        // Make the move on our state copy
        state[move.to] = pieceMoving;
        state[move.from] = '0';

        // Call negamax to evaluate this move
        // AI is Black, so playerColor = BLACK = -1
        // After AI moves, it's White's turn
        int moveVal = -negamax(state, 4, negInfinite, posInfinite, WHITE);

        // Undo the move
        state[move.from] = pieceMoving;
        state[move.to] = boardSave;

        // Track the best move found
        if (moveVal > bestVal) {
            bestMove = move;
            bestVal = moveVal;
            // std::cout << "New best move: value = " << bestVal << std::endl;
        }
    }

    // Execute the best move on the actual board
    if(bestVal != negInfinite) {
        // std::cout << "AI choosing move after checking " << _countMoves << " positions" << std::endl;
        int srcSquare = bestMove.from;
        int dstSquare = bestMove.to;
        
        int srcX = srcSquare % 8;
        int srcY = srcSquare / 8;
        int dstX = dstSquare % 8;
        int dstY = dstSquare / 8;
        
        // std::cout << "AI move: " << char('a' + srcX) << (srcY + 1) 
        //           << " to " << char('a' + dstX) << (dstY + 1) << std::endl;
        
        // Get the source and destination holders
        BitHolder& src = getHolderAt(srcX, srcY);
        BitHolder& dst = getHolderAt(dstX, dstY);
        
        // Get the piece to move
        Bit* bit = src.bit();
        
        if (bit) {
            // IMPORTANT: Use the game system to make the move properly
            // This ensures turn switching happens correctly
            
            // First, check if the move is legal
            if (dst.canDropBitAtPoint(bit, ImVec2(0, 0)) && 
                canBitMoveFromTo(*bit, src, dst)) {
                
                // Move the piece through the game system
                dst.dropBitAtPoint(bit, ImVec2(0, 0));
                src.setBit(nullptr);
                
                // CRITICAL: Call the parent's bitMovedFromTo to switch turns
                Game::bitMovedFromTo(*bit, src, dst);
                
                // std::cout << "AI move completed. Turn should now be White's." << std::endl;
            } else {
                std::cout << "ERROR: AI tried illegal move!" << std::endl;
            }
        }
    } else {
        std::cout << "AI couldn't find a valid move!" << std::endl;
    }
}
int Chess::evaluateBoard(const std::string& state) {
    int value = 0;
    for(char ch : state) {
        value += evaluateScores[ch];
    }
    return value;
}
