#ifndef STATUSBAR_H
#define STATUSBAR_H

#include "raylib.h"
#include "cell.h"
#include "colors.h"

#define WIDTH 670
#define HEIGHT 800
#define LENGTH 40
#define MAX 16
#define MINECOUNT 50

extern volatile int currentFlagAmount;
static const char* ongoing = "0_0";
static const char* win = "^_^";
static const char* lost = "x_x";


class StatusBar {
private:
    int width;
    int height;
    int flagCount;
    double startTime;
    double elapsedTime;
    bool isRunning;
    const char* currentExpression;

public:
    StatusBar(int width_, int height_, int flagCount_);

    void start();
    void stop();
    void reset(int flags);
    void update();
    void setExpression(const char* expr);
    bool isRestartClicked(int mouseX, int mouseY) const;
    void draw(); 
    bool isTimerRunning() const;
};

#endif
