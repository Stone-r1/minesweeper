#include "raylib.h"
#include "cell.h"
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

class Board {
private:
    vector<vector<Cell>> grid;
    bool firstClick;

public:
    Board() {
        grid.assign(MAX, vector<Cell>(MAX));
        firstClick = true;
    }

    void draw() {
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

    void restart() {
        for (int i = 0; i < MAX; i++) {
            for (int j = 0; j < MAX; j++) {
                grid[i][j].reset();
            }
        }

        currentFlagAmount = MINECOUNT;
        firstClick = true;
    }

    void generateMines(int x, int y) {
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

    void generateNumbers() {
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

    void bfs(int x, int y) {
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

    bool handleLeftClick(int mouseX, int mouseY) {
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

    void handleRightClick(int mouseX, int mouseY) {
        int x = (mouseX - 15) / LENGTH;
        int y = (mouseY - 155) / LENGTH;

        if (x >= 0 && y >= 0 && x < MAX && y < MAX && !grid[y][x].activated) {
            grid[y][x].toggleFlag();
            currentFlagAmount += grid[y][x].isFlagged ? -1 : 1;
        }
    }
};

class StatusBar {
private:
    int width;
    int height;
    int flagCount;
    double startTime;
    double elapsedTime;
    bool isRunning;

public:
    StatusBar(int width_, int height_, int flagCount_) {
        width = width_;
        height = height_;
        flagCount = flagCount_;
        startTime = 0.0;
        elapsedTime = 0.0;
        isRunning = false;
    }

    void start() {
        startTime = GetTime();
        isRunning = true;
    }

    void reset(int flags) {
        flagCount = flags;
        elapsedTime = 0.0;
        isRunning = false;
    }

    void update() {
        if (isRunning) {
            elapsedTime = GetTime() - startTime;
        }
    }

    bool isRestartClicked(int mouseX, int mouseY) const {
        int margin = 10;
        int size = height - 20;
        int x1 = (width - size) / 2, x2 = x1 + size;
        int y1 = margin, y2 = y1 + size;

        return (mouseX >= x1 && mouseX <= x2 && mouseY >= y1 && mouseY <= y2);
    }

    void draw() {
        DrawRectangle(0, 0, width, height, RED);

        const int margin = 10;
        const int restartButtonSize = height - 20;
        const int sideBoxWidth = height * 2;
        const int fontSize = 80;

        int restartButtonX = (WIDTH - restartButtonSize) / 2;
        int restartButtonY = margin;

        // position side boxes with margins
        int leftBoxX = margin;
        int rightBoxX = width - sideBoxWidth - margin;
        int sideBoxY = margin;

        DrawRectangle(leftBoxX, sideBoxY, sideBoxWidth, restartButtonSize, RAYWHITE); // Flags box
        DrawRectangle(rightBoxX, sideBoxY, sideBoxWidth, restartButtonSize, RAYWHITE); // Time box
        DrawRectangle(restartButtonX, restartButtonY, restartButtonSize, restartButtonSize, RAYWHITE); // Button box

        int minutes = (int)(elapsedTime / 60);
        int seconds = (int)(elapsedTime) % 60;
        char currentTime[6];
        snprintf(currentTime, sizeof(currentTime), "%02d:%02d", minutes, seconds);
        int timeTextX = rightBoxX + (sideBoxWidth - MeasureText(currentTime, fontSize)) / 2;
        int timeTextY = margin + sideBoxY + (restartButtonSize - fontSize) / 2;
        DrawText(currentTime, timeTextX, timeTextY, fontSize, GREEN);

        char buffer[3];
        snprintf(buffer, sizeof(buffer), "%d", currentFlagAmount);
        int flagTextX = leftBoxX + (sideBoxWidth - MeasureText(buffer, fontSize)) / 2;
        int flagTextY = timeTextY;
        DrawText(buffer, flagTextX, flagTextY, fontSize, GREEN);
    }

    void incrementFlag() {
        flagCount++; 
    }

    void decrementFlag() {
        flagCount--;
    }

    int getFlagAmount() {
        return flagCount;
    }

    bool isTimerRunning() {
        return isRunning;
    }

    double getElapsedTime() {
        return elapsedTime;
    }
};

int main() {
    InitWindow(WIDTH, HEIGHT, "testing");
    SetTargetFPS(60);

    Board board;
    StatusBar statusBar(WIDTH, statusBarHeight, MINECOUNT);

    while(!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(DARKGRAY);

        statusBar.update();
        board.draw();
        statusBar.draw();
    
        Vector2 mouse = GetMousePosition();
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            if (statusBar.isRestartClicked(mouse.x, mouse.y)) {
                board.restart();
                statusBar.reset(MINECOUNT);
            } else {
                bool mineExploded = board.handleLeftClick(mouse.x, mouse.y);
                if (mineExploded) {
                    statusBar.reset(MINECOUNT);
                } else if (!statusBar.isTimerRunning()) {
                    statusBar.start();
                }
            }
        } else if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
            board.handleRightClick(mouse.x, mouse.y);
        }

        EndDrawing();
    }
    CloseWindow();
}
