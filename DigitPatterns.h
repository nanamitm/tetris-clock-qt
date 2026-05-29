#pragma once
#include <vector>
#include <utility>

// Piece fall instruction (ported from toblum/TetrisAnimation TetrisNumbers.h)
struct fall_instr {
    int blocktype; // 0=Square 1=L 2=L-rev 3=I 4=S 5=S-rev 6=T 7=Corner
    int color;     // 1-7 color index
    int x_pos;     // x offset within digit (from digit's left edge)
    int y_stop;    // y where piece bottom lands (8=top row, 16=bottom row)
    int num_rot;   // clockwise rotations (0-3)
};

// X-axis start column for each of the 4 digits (matches xShiftClock in original)
static constexpr int DIGIT_X_OFFSETS[4] = {0, 7, 17, 24};

// ── Digit definitions (from toblum/TetrisAnimation TetrisNumbers.h) ──────────

static const fall_instr num_0[] = {
    {2, 5, 4, 16, 0},{4, 7, 2, 16, 1},{3, 4, 0, 16, 1},{6, 6, 1, 16, 1},
    {5, 1, 4, 14, 0},{6, 6, 0, 13, 3},{5, 1, 4, 12, 0},{5, 1, 0, 11, 0},
    {6, 6, 4, 10, 1},{6, 6, 0,  9, 1},{5, 1, 1,  8, 1},{2, 5, 3,  8, 3}};

static const fall_instr num_1[] = {
    {2, 5, 4, 16, 0},{3, 4, 4, 15, 1},{3, 4, 5, 13, 3},
    {2, 5, 4, 11, 2},{0, 0, 4,  8, 0}};

static const fall_instr num_2[] = {
    {0, 0, 4, 16, 0},{3, 4, 0, 16, 1},{1, 2, 1, 16, 3},{1, 2, 1, 15, 0},
    {3, 4, 1, 12, 2},{1, 2, 0, 12, 1},{2, 5, 3, 12, 3},{0, 0, 4, 10, 0},
    {3, 4, 1,  8, 0},{2, 5, 3,  8, 3},{1, 2, 0,  8, 1}};

static const fall_instr num_3[] = {
    {1, 2, 3, 16, 3},{2, 5, 0, 16, 1},{3, 4, 1, 15, 2},{0, 0, 4, 14, 0},
    {3, 4, 1, 12, 2},{1, 2, 0, 12, 1},{3, 4, 5, 12, 3},{2, 5, 3, 11, 0},
    {3, 4, 1,  8, 0},{1, 2, 0,  8, 1},{2, 5, 3,  8, 3}};

static const fall_instr num_4[] = {
    {0, 0, 4, 16, 0},{0, 0, 4, 14, 0},{3, 4, 1, 12, 0},{1, 2, 0, 12, 1},
    {2, 5, 0, 10, 0},{2, 5, 3, 12, 3},{3, 4, 4, 10, 3},{2, 5, 0,  9, 2},
    {3, 4, 5, 10, 1}};

static const fall_instr num_5[] = {
    {0, 0, 0, 16, 0},{2, 5, 2, 16, 1},{2, 5, 3, 15, 0},{3, 4, 5, 16, 1},
    {3, 4, 1, 12, 0},{1, 2, 0, 12, 1},{2, 5, 3, 12, 3},{0, 0, 0, 10, 0},
    {3, 4, 1,  8, 2},{1, 2, 0,  8, 1},{2, 5, 3,  8, 3}};

static const fall_instr num_6[] = {
    {2, 5, 0, 16, 1},{5, 1, 2, 16, 1},{6, 6, 0, 15, 3},{6, 6, 4, 16, 3},
    {5, 1, 4, 14, 0},{3, 4, 1, 12, 2},{2, 5, 0, 13, 2},{3, 4, 2, 11, 0},
    {0, 0, 0, 10, 0},{3, 4, 1,  8, 0},{1, 2, 0,  8, 1},{2, 5, 3,  8, 3}};

static const fall_instr num_7[] = {
    {0, 0, 4, 16, 0},{1, 2, 4, 14, 0},{3, 4, 5, 13, 1},{2, 5, 4, 11, 2},
    {3, 4, 1,  8, 2},{2, 5, 3,  8, 3},{1, 2, 0,  8, 1}};

static const fall_instr num_8[] = {
    {3, 4, 1, 16, 0},{6, 6, 0, 16, 1},{3, 4, 5, 16, 1},{1, 2, 2, 15, 3},
    {4, 7, 0, 14, 0},{1, 2, 1, 12, 3},{6, 6, 4, 13, 1},{2, 5, 0, 11, 1},
    {4, 7, 0, 10, 0},{4, 7, 4, 11, 0},{5, 1, 0,  8, 1},{5, 1, 2,  8, 1},
    {1, 2, 4,  9, 2}};

static const fall_instr num_9[] = {
    {0, 0, 0, 16, 0},{3, 4, 2, 16, 0},{1, 2, 2, 15, 3},{1, 2, 4, 15, 2},
    {3, 4, 1, 12, 2},{3, 4, 5, 12, 3},{5, 1, 0, 12, 0},{1, 2, 2, 11, 3},
    {5, 1, 4,  9, 0},{6, 6, 0, 10, 1},{5, 1, 0,  8, 1},{6, 6, 2,  8, 2}};

static const fall_instr* ALL_DIGITS[10] = {
    num_0, num_1, num_2, num_3, num_4,
    num_5, num_6, num_7, num_8, num_9
};
static const int DIGIT_SIZES[10] = {12, 5, 11, 11, 9, 11, 12, 7, 13, 12};

// ── Block shape definitions ───────────────────────────────────────────────────
// Returns (dx, dy) offsets from the piece's (x_pos, y_bottom).
// dy is 0 for the bottom row, negative for rows above.
using CellOffsets = std::vector<std::pair<int,int>>;

inline CellOffsets getBlockOffsets(int blocktype, int num_rot)
{
    switch (blocktype) {
    case 0: // Square (2×2)
        return {{0,0},{1,0},{0,-1},{1,-1}};

    case 1: // L-shape
        switch (num_rot) {
        case 0: return {{0,0},{1,0},{0,-1},{0,-2}};
        case 1: return {{0,0},{0,-1},{1,-1},{2,-1}};
        case 2: return {{1,0},{1,-1},{1,-2},{0,-2}};
        case 3: return {{0,0},{1,0},{2,0},{2,-1}};
        }
        break;

    case 2: // L-reverse
        switch (num_rot) {
        case 0: return {{0,0},{1,0},{1,-1},{1,-2}};
        case 1: return {{0,0},{1,0},{2,0},{0,-1}};
        case 2: return {{0,0},{0,-1},{0,-2},{1,-2}};
        case 3: return {{0,-1},{1,-1},{2,-1},{2,0}};
        }
        break;

    case 3: // I-shape (4 long)
        if (num_rot == 0 || num_rot == 2)
            return {{0,0},{1,0},{2,0},{3,0}};    // horizontal
        else
            return {{0,0},{0,-1},{0,-2},{0,-3}};  // vertical

    case 4: // S-shape
        if (num_rot == 0 || num_rot == 2)
            return {{1,0},{0,-1},{1,-1},{0,-2}};
        else
            return {{0,0},{1,0},{1,-1},{2,-1}};

    case 5: // S-reverse
        if (num_rot == 0 || num_rot == 2)
            return {{0,0},{0,-1},{1,-1},{1,-2}};
        else
            return {{1,0},{2,0},{0,-1},{1,-1}};

    case 6: // T (half-cross)
        switch (num_rot) {
        case 0: return {{0,0},{1,0},{2,0},{1,-1}};
        case 1: return {{0,0},{0,-1},{0,-2},{1,-1}};
        case 2: return {{1,0},{0,-1},{1,-1},{2,-1}};
        case 3: return {{1,0},{1,-1},{1,-2},{0,-1}};
        }
        break;

    case 7: // Corner (3 cells)
        switch (num_rot) {
        case 0: return {{0,0},{1,0},{0,-1}};
        case 1: return {{0,0},{0,-1},{1,-1}};
        case 2: return {{1,0},{1,-1},{0,-1}};
        case 3: return {{0,0},{1,0},{1,-1}};
        }
        break;
    }
    return {};
}
