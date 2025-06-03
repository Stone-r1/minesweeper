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

const int statusBarHeight = HEIGHT - 680;
const int rowDir[8] = {0, 1, 1, 1, 0, -1, -1, -1};
const int colDir[8] = {-1, -1, 0, 1, 1, 1, 0, -1};
const int MINE = 9, EMPTY = 0;

volatile static int currentFlagAmount = MINECOUNT;

class Cell {
public:
    int x;
    int y;
    int status;
    bool activated;
    bool isFlagged;

    bool isMine() const {
        return status == MINE;
    }

    bool isEmpty() const {
        return status == EMPTY;
    }

    void activate() {
        activated = true;
    }

    void deactivate() {
        activated = false;
    }

    void toggleFlag() {
        isFlagged = !isFlagged;
    }

    void reset() {
        status = EMPTY;
        activated = false;
        isFlagged = false;
    }
};

void drawStatusBar() {
    DrawRectangle(0, 0, WIDTH, statusBarHeight, RED);

    const int margin = 10;
    const int restartButtonSize = statusBarHeight - 20;
    const int sideBoxWidth = statusBarHeight * 2;

    int restartButtonX = (WIDTH - restartButtonSize) / 2;
    int restartButtonY = margin;

    // position side boxes with margins
    int leftBoxX = margin;
    int rightBoxX = WIDTH - sideBoxWidth - margin;
    int sideBoxY = margin;

    DrawRectangle(leftBoxX, sideBoxY, sideBoxWidth, restartButtonSize, RAYWHITE); // Flags box
    DrawRectangle(rightBoxX, sideBoxY, sideBoxWidth, restartButtonSize, RAYWHITE); // Time box
    DrawRectangle(restartButtonX, restartButtonY, restartButtonSize, restartButtonSize, RAYWHITE); // Button box

    const char* currentTime = "00:00";
    const int fontSize = 80;
    int timeTextX = rightBoxX + (sideBoxWidth - MeasureText(currentTime, fontSize)) / 2;
    int timeTextY = margin + sideBoxY + (restartButtonSize - fontSize) / 2;
    DrawText(currentTime, timeTextX, timeTextY, fontSize, GREEN);

    char buffer[3];
    snprintf(buffer, sizeof(buffer), "%d", currentFlagAmount);
    int flagTextX = leftBoxX + (sideBoxWidth - MeasureText(buffer, fontSize)) / 2;
    int flagTextY = timeTextY;
    DrawText(buffer, flagTextX, flagTextY, fontSize, GREEN);
}


// 640 x 640 with 15px margins
void drawCells(vector<vector<Cell>>& grid) {
    for (int row = 0; row < MAX; row++) {
        for (int column = 0; column < MAX; column++) {
            auto& refToCell = grid[row][column];
            refToCell.x = column * LENGTH;
            refToCell.y = row * LENGTH;

            int drawX = refToCell.x + 15;
            int drawY = refToCell.y + 145;

            // 15 px margin on the right and the left
            DrawRectangle(drawX, drawY, LENGTH, LENGTH, GRAY);
            DrawRectangle(drawX + 2, drawY + 2, LENGTH - 4, LENGTH - 4, BLACK);

            if (refToCell.activated) {
                if (refToCell.isEmpty()) {
                    DrawRectangle(drawX + 2, drawY + 2, LENGTH - 4, LENGTH - 4, LIGHTGRAY); // TEMPORARY
                    continue;
                }
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

        if (grid[row][column].isMine()) {
            continue;
        }

        grid[row][column].status = MINE;
        currentMineCount++;
    }
}

void generateNumbers(vector<vector<Cell>>& grid) {
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

void restartTheGame(vector<vector<Cell>>& grid) {
    for (int i = 0; i < MAX; i++) {
        for (int j = 0; j < MAX; j++) {
            grid[i][j].reset();
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

void bfs(vector<vector<Cell>>& grid, int x, int y, bool& firstClick) {
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
                    restartTheGame(grid);
                    firstClick = true;
                    return;
                } else {
                    grid[newR][newC].deactivate();
                }
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

                int statusBarX = mouse.x;
                int statusBarY = mouse.y;

                if (statusBarX >= 0 && statusBarX < statusBarHeight && statusBarY >= 0 && statusBarY < WIDTH) {
                    manageStatusBarActivities(statusBarY, statusBarX, grid, firstClick);
                }

                int x = (mouse.x - 15) / LENGTH;
                int y = (mouse.y - 155) / LENGTH;
                
                if (x >= 0 && y >= 0 && x < MAX && y < MAX && !grid[y][x].isFlagged) {
                    grid[y][x].activate();
                    if (firstClick) {
                        generateMines(grid, y, x);
                        generateNumbers(grid);
                        firstClick = false;
                    }
                    
                    if (grid[y][x].isEmpty()) {
                        bfs(grid, y, x, firstClick);
                    } else if (grid[y][x].isMine()) {
                        // ADD GAMEOVER SCREEN LATER
                        restartTheGame(grid);
                        firstClick = true;
                    } else {
                        int minesNum = grid[y][x].status;
                        int flagsFound = 0;

                        for (int d = 0; d < 8; d++) {
                            int newR = y + rowDir[d];
                            int newC = x + colDir[d];

                            if (newR < 0 || newC < 0 || newR >= MAX || newC >= MAX) {
                                continue;
                            } 
                            if (grid[newR][newC].isFlagged) {
                                flagsFound++;
                            }
                        }

                        if (flagsFound >= minesNum) {
                            bfs(grid, y, x, firstClick);
                        }
                    }
                }
            } else if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
                Vector2 mouse = GetMousePosition();
                int x = (mouse.x - 15) / LENGTH;
                int y = (mouse.y - 155) / LENGTH;

                if (x >= 0 && y >= 0 && x < MAX && y < MAX) {
                    grid[y][x].toggleFlag();
                }
            }

        EndDrawing();
    }
    CloseWindow();
}
