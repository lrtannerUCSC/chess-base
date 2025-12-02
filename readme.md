# Chess Engine

A high-performance C++ chess implementation featuring bitboard-based move generation and an AI opponent using negamax search with alpha-beta pruning.

## Features

### Core Engine
- Complete chess rules implementation
- Legal move validation and generation
- Check/checkmate detection
- Draw detection (stalemate, insufficient material, 50-move rule)
- FEN notation support for board state serialization

### AI System
- Configurable search depth
- Negamax algorithm with alpha-beta pruning
- Position evaluation with material and positional scoring
- Move ordering for search efficiency

### Performance Optimizations
- Bitboard representation for efficient piece operations
- Magic bitboards for sliding pieces (rooks, bishops, queens)
- Pre-calculated attack tables
- Efficient board state representation
- Fast bit scanning using compiler intrinsics

## Project Structure

### Core Files

| File | Description |
|------|-------------|
| `Chess.h` | Main game class inheriting from Game base class |
| `GameState.h` | Board state representation and move generation |
| `Bitboard.h` | Bitboard implementation with fast bit operations |
| `MagicBitboards.h` | Pre-calculated attack tables for sliding pieces |
| `Game.h` | Base game class with turn management and AI interface |

### Key Classes

| Class | Purpose |
|-------|---------|
| `Chess` | Main game controller and rule enforcement |
| `GameState` | Board state management and move generation |
| `BitBoard` | 64-bit board representation with efficient operations |
| `BitMove` | Compact move representation (4 bytes) |
| `GameStateData` | Persistent board state data structure |

## Building

### Dependencies
- C++17 compatible compiler
- Standard Library
- OpenGL/ImGui for GUI (in parent project)
