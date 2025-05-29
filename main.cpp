#include "raylib.h"
#include <vector>
using namespace std;

#define WIDTH 670
#define HEIGHT 800
#define LENGTH 40
#define MAX 16

struct cell {
    int x;
    int y;
    int status; // bomb will be 9, empty cell 0.
    bool activated;
};

// shows amount of flags, start game button, time.
void drawStatusBar() {
    DrawRectangle(0, 0, WIDTH, HEIGHT - 660, RED);    
}

// 640 x 640 with 15px margins
void drawCells(vector<vector<cell>>& grid) {
    for (int row = 0; row < MAX; row++) {
        for (int column = 0; column < MAX; column++) {
            auto& refToCell = grid[row][column];
            refToCell.x = column * LENGTH;
            refToCell.y = row * LENGTH;

            int drawX = refToCell.x + 15;
            int drawY = refToCell.y + 155;

            // 15 px margin on the right and the left
            DrawRectangle(drawX, drawY, LENGTH, LENGTH, GREEN);
            DrawRectangle(drawX + 2, drawY + 2, LENGTH - 4, LENGTH - 4, BLACK);
        }
    }
}

int main() {
    InitWindow(WIDTH, HEIGHT, "testing");
    SetTargetFPS(60);

    vector<vector<cell>> grid(MAX, vector<cell>(MAX));
    while(!WindowShouldClose()) {
        ClearBackground(RAYWHITE);
        
        BeginDrawing();
            drawStatusBar();
            drawCells(grid);
        EndDrawing();
    }
    CloseWindow();
}
