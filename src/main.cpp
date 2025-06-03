#include "raylib.h"
#include "cell.h"
#include "board.h"
#include "statusbar.h"

#include <vector>
#include <queue>
#include <iostream>
#include <algorithm>
using namespace std;

#define WIDTH 670
#define HEIGHT 800
#define LENGTH 40
#define MAX 16
#define MINECOUNT 50

const int statusBarHeight = HEIGHT - 680;

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

        if (board.isGameWon()) {
            statusBar.setExpression(win);
            statusBar.stop();
        } else if (board.isGameOver()) {
            statusBar.setExpression(lost);
        }
        statusBar.draw();


        Vector2 mouse = GetMousePosition();
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            if (statusBar.isRestartClicked(mouse.x, mouse.y)) {
                board.restart();
                statusBar.reset(MINECOUNT);
                statusBar.setExpression(ongoing);
            } else {
                bool mineExploded = board.handleLeftClick(mouse.x, mouse.y);
                if (mineExploded && board.isGameOver()) {
                    statusBar.setExpression(lost);
                    statusBar.reset(MINECOUNT);
                } else if (!statusBar.isTimerRunning() && !board.isGameOver() && !board.isGameWon()) {
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
