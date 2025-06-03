#ifndef STATUSBAR_H
#define STATUSBAR_H

#include "raylib.h"
#include "cell.h"
#include "colors.h"

#define WIDTH 670
#define HEIGHT 800
#define LENGTH 40
#define MAX 16
#define MINECOUNT 40

extern volatile int currentFlagAmount;

class StatusBar {
private:
    int width;
    int height;
    int flagCount;
    double startTime;
    double elapsedTime;
    bool isRunning;

public:
    StatusBar(int width_, int height_, int flagCount_);

    void start();
    void reset(int flags);
    void update();
    bool isRestartClicked(int mouseX, int mouseY) const;
    void draw(); 
    bool isTimerRunning() const;
};

#endif
