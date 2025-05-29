#include "raylib.h"

#define WIDTH 650
#define HEIGHT 800

struct cell {
    int width;
    int height;
    int status; // bomb will be 9, empty cell 0.
    bool activated;
};

int main() {
    InitWindow(WIDTH, HEIGHT, "testing");
    SetTargetFPS(60);

    while(!WindowShouldClose()) {
        ClearBackground(RAYWHITE);
        EndDrawing();
    }
    CloseWindow();
}
