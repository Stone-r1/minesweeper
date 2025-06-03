#include "raylib.h"
#include "board.h"
#include <algorithm>

volatile int currentFlagAmount = MINECOUNT;
const int rowDir[8] = {0, 1, 1, 1, 0, -1, -1, -1};
const int colDir[8] = {-1, -1, 0, 1, 1, 1, 0, -1};
const int MINE = 9, EMPTY = 0;

Board::Board() {
    grid.assign(MAX, vector<Cell>(MAX));
    firstClick = true;
}

void Board::draw() {
    for (int row = 0; row < MAX; row++) {
        for (int column = 0; column < MAX; column++) {
            Cell& refToCell = grid[row][column];
            refToCell.x = column * LENGTH;
            refToCell.y = row * LENGTH;

            int drawX = refToCell.x + 15;
            int drawY = refToCell.y + 145;

            // 15 px margin on the right and the left
            DrawRectangle(drawX, drawY, LENGTH, LENGTH, GRAY);
            DrawRectangle(drawX + 2, drawY + 2, LENGTH - 4, LENGTH - 4, BLACK);

            if (refToCell.activated) {
                if (refToCell.isEmpty()) {
                    DrawRectangle(drawX + 2, drawY + 2, LENGTH - 4, LENGTH - 4, LIGHTGRAY);
                    continue;
                }
                DrawText(TextFormat("%d", refToCell.status), refToCell.x + 30, refToCell.y + 153, 25, GREEN);
            } else if (refToCell.isFlagged) {
                DrawText(TextFormat("%s", "#"), refToCell.x + 30, refToCell.y + 153, 25, RED);
            }
        }
    }
}

void Board::restart() {
    for (int i = 0; i < MAX; i++) {
        for (int j = 0; j < MAX; j++) {
            grid[i][j].reset();
        }
    }

    currentFlagAmount = MINECOUNT;
    firstClick = true;
}

void Board::generateMines(int x, int y) {
    int currentMineCount = 0;
    while (currentMineCount < MINECOUNT) {
        int row = GetRandomValue(0, MAX - 1);
        int column = GetRandomValue(0, MAX - 1);

        // skip the safe cell and its neighbors
        if (abs(row - x) <= 1 && abs(column - y) <= 1) {
            continue;
        }

        if (grid[row][column].isMine()) {
            continue;
        }

        grid[row][column].status = MINE;
        currentMineCount++;
    }    
}

void Board::generateNumbers() {
    for (int row = 0; row < MAX; row++) {
        for (int column = 0; column < MAX; column++) {
            int mines = 0;

            for (int d = 0; d < 8; d++) {
                int newR = row + rowDir[d];
                int newC = column + colDir[d];

                if (newR < 0 || newC < 0 || newR >= MAX || newC >= MAX) {
                    continue;
                }

                if (grid[newR][newC].isMine()) {
                    mines++;
                }
            }
            
            if (!grid[row][column].isMine()) {
                grid[row][column].status = mines;
            }
        }
    }
}

void Board::bfs(int x, int y) {
    queue<pair<int, int>> q;
    q.push({x, y});
    
    while (!q.empty()) {
        auto [r, c] = q.front();
        q.pop();

        grid[r][c].activate();

        for (int d = 0; d < 8; d++) {
            int newR = r + rowDir[d];
            int newC = c + colDir[d];
            
            if (newR < 0 || newC < 0 || newR >= MAX || newC >= MAX || grid[newR][newC].activated) {
                continue;
            }

            grid[newR][newC].activate();
            if (grid[newR][newC].isEmpty()) {
                q.push({newR, newC});
            } else if (grid[newR][newC].isMine()) {
                if (!grid[newR][newC].isFlagged) {
                    restart(); 
                    return;
                } else {
                    grid[newR][newC].deactivate();
                }
            }
        }
    }
}

bool Board::handleLeftClick(int mouseX, int mouseY) {
    int x = (mouseX - 15) / LENGTH;
    int y = (mouseY - 145) / LENGTH;

    if (x < 0 || y < 0 || x >= MAX || y >= MAX || grid[y][x].isFlagged) {
        return false;
    }

    grid[y][x].activate();
    if (firstClick) {
        generateMines(y, x);
        generateNumbers();
        firstClick = false;
    }

    if (grid[y][x].isEmpty()) {
        bfs(y, x);
    } else if (grid[y][x].isMine()) {
        restart();
        return true;
    } else {
        int minesNum = grid[y][x].status;
        int flagsFound = 0;
        for (int d = 0; d < 8; d++) {
            int newR = y + rowDir[d];
            int newC = x + colDir[d];
            if (newR < 0 || newC < 0 || newR >= MAX || newC >= MAX) continue;
            if (grid[newR][newC].isFlagged) flagsFound++;
        }
        if (flagsFound >= minesNum) {
            bfs(y, x);
        }
    }

    return false;
}

void Board::handleRightClick(int mouseX, int mouseY) {
    int x = (mouseX - 15) / LENGTH;
    int y = (mouseY - 145) / LENGTH;

    if (x >= 0 && y >= 0 && x < MAX && y < MAX && !grid[y][x].activated) {
        grid[y][x].toggleFlag();
        currentFlagAmount += grid[y][x].isFlagged ? -1 : 1;
    }
}
