#include "raylib.h"
#include "board.h"
#include <algorithm>
#include <cstdio>
#include <iostream>

volatile int currentFlagAmount = MINECOUNT;
const int rowDir[8] = {0, 1, 1, 1, 0, -1, -1, -1};
const int colDir[8] = {-1, -1, 0, 1, 1, 1, 0, -1};
const int MINE = 9, EMPTY = 0;

Board::Board() {
    grid.assign(MAX, vector<Cell>(MAX));
    firstClick = true;
    gameOver = false;
    gameWin = false;
}

void Board::draw() {
    for (int row = 0; row < MAX; row++) {
        for (int column = 0; column < MAX; column++) {
            Cell& refToCell = grid[row][column];
            refToCell.x = column * LENGTH;
            refToCell.y = row * LENGTH;

            int drawX = refToCell.x + 15;
            int drawY = refToCell.y + 145;
            const int fontSize = 35;

            DrawRectangle(drawX + 4, drawY + 4, LENGTH - 2, LENGTH - 2, CELL_SH);
            DrawRectangle(drawX, drawY, LENGTH - 2, LENGTH - 2, CELL_HL);
            DrawRectangle(drawX + 4, drawY + 4, LENGTH - 6, LENGTH - 6, CELL);

            if (refToCell.activated) { 
                DrawRectangle(drawX + 2, drawY + 2, LENGTH - 4, LENGTH - 4, LIGHTGRAY);
                if (refToCell.isEmpty()) {
                    continue;
                }

                char buffer[1];
                snprintf(buffer, sizeof(buffer), "%d", refToCell.status); 
                DrawText(TextFormat("%d", refToCell.status), refToCell.x + (LENGTH + 6 + MeasureText(buffer, fontSize)) / 2, refToCell.y + 152, fontSize, DARKGREEN);
            } else if (refToCell.isFlagged) {
                DrawText(TextFormat("%s", "⚑"), refToCell.x + (LENGTH - 10 + MeasureText("⚑", fontSize)) / 2, refToCell.y + 152, fontSize, RED);
            }
        }
    }

    if (gameOver) {
        for (int i = 0; i < MAX; i++) {
            for (int j = 0; j < MAX; j++) {
                if (grid[i][j].isMine() && !grid[i][j].isFlagged) {
                    grid[i][j].activated = false;
                    DrawText(TextFormat("%s", "*"), grid[i][j].x + (LENGTH - 10 + MeasureText("*", 35)) / 2, grid[i][j].y + 152, 35, RED);
                }
            }
        }
        return;
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
    gameOver = false;
    gameWin = false;
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

bool Board::bfs(int x, int y) {
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
                    gameOver = true;
                    return false;
                } else {
                    grid[newR][newC].deactivate();
                }
            }
        }
    }
    return true;
}

bool Board::isGameWon() {
    for (int i = 0; i < MAX; i++) {
        for (int j = 0; j < MAX; j++) {
            if (!grid[i][j].isMine() && !grid[i][j].activated) {
                return false;
            }
        } 
    }

    return true;
}

bool Board::isGameOver() {
    return gameOver;
}

bool Board::handleLeftClick(int mouseX, int mouseY) {
    if (gameOver || gameWin) return false;

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
        bool explode = bfs(y, x);
        if (!explode) {
            // restart();
            return true;
        }
    } else if (grid[y][x].isMine()) {
        gameOver = true;
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
            int explode = bfs(y, x);
            if (!explode) {
                return true;
            }
        }
    }

    if (!isGameOver() && isGameWon()) {
        gameWin = true;
        return true;
    }

    return false;
}

void Board::handleRightClick(int mouseX, int mouseY) {
    if (gameOver || gameWin) return;

    int x = (mouseX - 15) / LENGTH;
    int y = (mouseY - 145) / LENGTH;

    if (x >= 0 && y >= 0 && x < MAX && y < MAX && !grid[y][x].activated) {
        grid[y][x].toggleFlag();
        currentFlagAmount += grid[y][x].isFlagged ? -1 : 1;
    }
}
