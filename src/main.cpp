#include "raylib.h"
#include "cell.h"
#include "board.h"
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
