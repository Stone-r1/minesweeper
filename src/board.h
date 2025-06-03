#ifndef BOARD_H
#define BOARD_H

#include "raylib.h"
#include "cell.h"
#include "colors.h"
#include <vector>
#include <queue>
using namespace std;

#define WIDTH 670
#define HEIGHT 800
#define LENGTH 40
#define MAX 16
#define MINECOUNT 50

extern volatile int currentFlagAmount;

class Board {
private:
    vector<vector<Cell>> grid;
    bool firstClick;
    bool gameOver;
    bool gameWin;

public:
    Board();

    void draw();
    void restart();
    void generateMines(int x, int y);
    void generateNumbers();
    bool bfs(int x, int y);
    bool isGameWon();
    bool isGameOver();
    bool handleLeftClick(int mouseX, int mouseY);
    void handleRightClick(int mouseX, int mouseY);
};

#endif
