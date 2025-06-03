#include "cell.h"

bool Cell::isMine() const {
    return status == MINE;
}

bool Cell::isEmpty() const {
    return status == EMPTY;
}

void Cell::activate() {
    activated = true;
}

void Cell::deactivate() {
    activated = false;
}

void Cell::toggleFlag() {
    isFlagged = !isFlagged;
}

void Cell::reset() {
    status = EMPTY;
    activated = false;
    isFlagged = false;
}
