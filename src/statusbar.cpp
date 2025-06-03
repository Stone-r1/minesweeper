#include "statusbar.h"
#include <cstdio>

StatusBar::StatusBar(int width_, int height_, int flagCount_) {
    width = width_;
    height = height_;
    flagCount = flagCount_;
    startTime = 0.0;
    elapsedTime = 0.0;
    isRunning = false;
}

void StatusBar::start() {
    startTime = GetTime();
    isRunning = true;
}

void StatusBar::reset(int flags) {
    flagCount = flags;
    elapsedTime = 0.0;
    isRunning = false;
}

void StatusBar::update() {
    if (isRunning) {
        elapsedTime = GetTime() - startTime;
    }
}

bool StatusBar::isRestartClicked(int mouseX, int mouseY) const {
    int margin = 10;
    int size = height - 20;
    int x1 = (width - size) / 2, x2 = x1 + size;
    int y1 = margin, y2 = y1 + size;

    return (mouseX >= x1 && mouseX <= x2 && mouseY >= y1 && mouseY <= y2);
}

void StatusBar::draw() {
    DrawRectangle(0, 0, width, height, STATUSBAR_BACKGROUND);

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

    DrawRectangle(leftBoxX, sideBoxY, sideBoxWidth, restartButtonSize, STATUSBAR_BOX); // Flags box
    DrawRectangle(rightBoxX, sideBoxY, sideBoxWidth, restartButtonSize, STATUSBAR_BOX); // Time box
    DrawRectangle(restartButtonX, restartButtonY, restartButtonSize, restartButtonSize, STATUSBAR_BOX); // Button box

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

bool StatusBar::isTimerRunning() const {
    return isRunning;
}
