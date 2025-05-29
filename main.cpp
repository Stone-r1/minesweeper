#include "raylib.h"
#include <vector>
#include <cstdlib>
#include <iostream>
#include <queue>
using namespace std;

#define WIDTH 670
#define HEIGHT 800
#define LENGTH 40
#define MAX 16
#define MINECOUNT 40

struct Cell {
    int x;
    int y;
    int status; // bomb will be 9, empty cell 0.
    bool activated;
    bool isFlagged;
};

// shows amount of flags, start game button, time.
void drawStatusBar() {
    DrawRectangle(0, 0, WIDTH, HEIGHT - 660, RED);    
}

// 640 x 640 with 15px margins
void drawCells(vector<vector<Cell>>& grid) {
    for (int row = 0; row < MAX; row++) {
        for (int column = 0; column < MAX; column++) {
            auto& refToCell = grid[row][column];
            refToCell.x = column * LENGTH;
            refToCell.y = row * LENGTH;

            int drawX = refToCell.x + 15;
            int drawY = refToCell.y + 155;

            // 15 px margin on the right and the left
            DrawRectangle(drawX, drawY, LENGTH, LENGTH, GRAY);
            DrawRectangle(drawX + 2, drawY + 2, LENGTH - 4, LENGTH - 4, BLACK);
        }
    }
}

void generateMines(vector<vector<Cell>>& grid, int x, int y) {
    int currentMineCount = 0;
    while (currentMineCount < MINECOUNT) {
        int row = rand() % MAX;
        int column = rand() % MAX;

        // skip the safe cell and its neighbors
        if (abs(row - x) <= 1 && abs(column - y) <= 1) {
            continue;
        }

        if (grid[row][column].status == 9) {
            continue;
        }

        grid[row][column].status = 9;
        currentMineCount++;
    }
}

void generateNumbers(vector<vector<Cell>>& grid) {
    int colDir[8] = {-1, -1, 0, 1, 1, 1, 0, -1};
    int rowDir[8] = {0, 1, 1, 1, 0, -1, -1, -1};

    for (int row = 0; row < MAX; row++) {
        for (int column = 0; column < MAX; column++) {
            int mines = 0;

            for (int d = 0; d < 8; d++) {
                int newR = row + rowDir[d];
                int newC = column + colDir[d];

                if (newR < 0 || newC < 0 || newR >= MAX || newC >= MAX) {
                    continue;
                }

                if (grid[newR][newC] == 9) {
                    mines++;
                }
            }

            grid[row][column].status = mines;
        }
    }
}

void bfs(vector<vector<Cell>>& grid, int x, int y) {
    int rowDir[4] = {1, -1, 0, 0};
    int colDir[4] = {0, 0, -1, 1};

    queue<pair<int, int>> q;
    q.push({x, y});
    
    while (!q.empty()) {
        auto [r, c] = q.front();
        q.pop();

        // unlock this cell
        grid[r][c].activated = true;

        for (int d = 0; d < 4; d++) {
            int newR = r + rowDir[d];
            int newC = c + colDir[d];

            if (grid[newR][newC].status == 0) {
                q.push({newR, newC});
            }
        }
    }
}

int main() {
    InitWindow(WIDTH, HEIGHT, "testing");
    SetTargetFPS(60);

    vector<vector<Cell>> grid(MAX, vector<Cell>(MAX));
    bool firstClick = true;

    while(!WindowShouldClose()) {
        ClearBackground(RAYWHITE);
        
        BeginDrawing();
            drawStatusBar();
            drawCells(grid);

            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                Vector2 mouse = GetMousePosition();
                int x = (mouse.x - 15) / LENGTH;
                int y = (mouse.y - 155) / LENGTH;

                if (x >= 0 && y >= 0 && x < MAX && y < MAX) {
                    if (firstClick) {
                        generateMines(grid, x, y);
                        generateNumbers(grid);
                        firstClick = false;
                    } else {
                        // normal gameplay
                    }
                    
                    if (grid[x][y].status == 0) {
                        bfs(grid, x, y);
                    }

                    grid[x][y].activated = true;
                }
            } else if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
                Vector2 mouse = GetMousePosition();
                int x = (mouse.x - 15) / LENGTH;
                int y = (mouse.y - 155) / LENGTH;

                grid[x][y].flagged = !grid[x][y].flagged;
            }

        EndDrawing();
    }
    CloseWindow();
}
