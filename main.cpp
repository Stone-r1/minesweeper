#include "raylib.h"
#include <vector>
#include <queue>
#include <iostream>
#include <algorithm>
using namespace std;

#define WIDTH 670
#define HEIGHT 800
#define LENGTH 40
#define MAX 16
#define MINECOUNT 40

const int statusBarHeight = HEIGHT - 660;

struct Cell {
    int x;
    int y;
    int status; // bomb will be 9, empty cell 0.
    bool activated;
    bool isFlagged;
};

// shows amount of flags, start game button, time.
void drawStatusBar() {
    DrawRectangle(0, 0, WIDTH, statusBarHeight, RED); 

    // CHANGE ALIGNMENT LATER
    DrawRectangle(10, 10, statusBarHeight - 20, statusBarHeight - 20, RAYWHITE);
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

            if (refToCell.activated) {
                DrawText(TextFormat("%d", refToCell.status), refToCell.x + 30, refToCell.y + 160, 25, GREEN);
            } else if (refToCell.isFlagged) {
                DrawText(TextFormat("%s", "#"), refToCell.x + 30, refToCell.y + 160, 25, RED);
            }
        }
    }
}

void generateMines(vector<vector<Cell>>& grid, int x, int y) {
    int currentMineCount = 0;
    while (currentMineCount < MINECOUNT) {
        int row = GetRandomValue(0, MAX - 1);
        int column = GetRandomValue(0, MAX - 1);

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

                if (grid[newR][newC].status == 9) {
                    mines++;
                }
            }
            
            if (grid[row][column].status != 9) {
                grid[row][column].status = mines;
            }
        }
    }
}

void bfs(vector<vector<Cell>>& grid, int x, int y) {
    int colDir[8] = {-1, -1, 0, 1, 1, 1, 0, -1};
    int rowDir[8] = {0, 1, 1, 1, 0, -1, -1, -1};

    queue<pair<int, int>> q;
    q.push({x, y});
    
    while (!q.empty()) {
        auto [r, c] = q.front();
        q.pop();

        // unlock this cell
        grid[r][c].activated = true;

        for (int d = 0; d < 8; d++) {
            int newR = r + rowDir[d];
            int newC = c + colDir[d];
            
            if (newR < 0 || newC < 0 || newR >= MAX || newC >= MAX || grid[newR][newC].activated) {
                continue;
            }

            grid[newR][newC].activated = true;
            if (grid[newR][newC].status == 0) {
                q.push({newR, newC});
            }
        }
    }
}

void restartTheGame(vector<vector<Cell>>& grid) {
    for (int i = 0; i < MAX; i++) {
        for (int j = 0; j < MAX; j++) {
            grid[i][j].status = 0;
            grid[i][j].activated = false;
            grid[i][j].isFlagged = false;
        }
    }
}

void manageStatusBarActivities(int r, int c, vector<vector<Cell>>& grid, bool& firstClick) {
    int x1 = 10, x2 = statusBarHeight - 20;
    int y1 = x1, y2 = x2;

    if (r < x2 && r > x1 && c < y2 && c > y1) {
        restartTheGame(grid);
        firstClick = true;
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

                int statusBarX = mouse.x;
                int statusBarY = mouse.y;

                if (statusBarX >= 0 && statusBarX < statusBarHeight && statusBarY >= 0 && statusBarY < WIDTH) {
                    manageStatusBarActivities(statusBarY, statusBarX, grid, firstClick);
                }

                int x = (mouse.x - 15) / LENGTH;
                int y = (mouse.y - 155) / LENGTH;
                
                if (x >= 0 && y >= 0 && x < MAX && y < MAX) {
                    grid[y][x].activated = true;
                    if (firstClick) {
                        generateMines(grid, y, x);
                        generateNumbers(grid);
                        firstClick = false;
                    } else {
                        cout << "clicked!\n";
                    }
                    
                    if (grid[y][x].status == 0) {
                        bfs(grid, y, x);
                    }
                }
            } else if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
                Vector2 mouse = GetMousePosition();
                int x = (mouse.x - 15) / LENGTH;
                int y = (mouse.y - 155) / LENGTH;

                if (x >= 0 || y >= 0 || x < MAX || y < MAX) {
                    grid[y][x].isFlagged = !grid[y][x].isFlagged;
                }
            }

        EndDrawing();
    }
    CloseWindow();
}
