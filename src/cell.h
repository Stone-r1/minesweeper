#pragma once

class Cell {
public:
    int x;
    int y;
    int status;
    bool activated;
    bool isFlagged;

    bool isMine() const;
    bool isEmpty() const;
    void activate();
    void deactivate();
    void toggleFlag();
    void reset();

    static const int MINE = 9;
    static const int EMPTY = 0;
};
