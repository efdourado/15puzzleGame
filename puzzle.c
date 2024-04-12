#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ncurses.h>
#include <unistd.h>

#define SIZE 4
#define DELAY 4

time_t startTime;
int board[SIZE][SIZE], emptyRow, emptyCol;

void iniBoard() {
    int num = 1;
    
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            board[i][j] = num++;
        }
    }

    emptyRow = SIZE - 1;
    emptyCol = SIZE - 1;

    board[emptyRow][emptyCol] = 0;
}

void printBoard(WINDOW *win) {
    wclear(win);

    wprintw(win, "~~~~~~~~~~~~~~~~~~~~~~~~~\n\n");

    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {

            if (board[i][j] == 0)
                wprintw(win, "      ");
            else {

                if (board[i][j] < 10)
                    wprintw(win, "  %02d  ", board[i][j]);
                else
                    wprintw(win, "  %d  ", board[i][j]);
            }
        }
        wprintw(win, "\n\n~~~~~~~~~~~~~~~~~~~~~~~~~\n\n");
    }
    wrefresh(win);
}

int isValidMove(int row, int col) {
    if (row < 0 || row >= SIZE || col < 0 || col >= SIZE)
        return 0;
    if ((row == emptyRow && abs(col - emptyCol) == 1) ||
        (col == emptyCol && abs(row - emptyRow) == 1))
        return 1;
    return 0;
}

void makeMove(int row, int col) {
    if (!isValidMove(row, col))
        return;
    board[emptyRow][emptyCol] = board[row][col];
    board[row][col] = 0;
    emptyRow = row;
    emptyCol = col;
}

int isSolved() {
    int num = 1;

    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            if (board[i][j] != num && !(i == SIZE - 1 && j == SIZE - 1))
                return 0;
            num++;
        }
    }
    return 1;
}

void shuffleBoard() {
    srand(time(NULL));

    for (int i = 0; i < SIZE * SIZE * 10; i++) {
        int direction = rand() % 4;
        int newRow = emptyRow;
        int newCol = emptyCol;
        
        switch (direction) {
            case 0:
                newRow--;
                break;
            case 1:
                newRow++;
                break;
            case 2:
                newCol--;
                break;
            case 3:
                newCol++;
                break;
        }
        if (isValidMove(newRow, newCol))
            makeMove(newRow, newCol);
    }
}

void startTimer() {
    startTime = time(NULL);
}

int stopTimer() {
    time_t endTime = time(NULL);
    return (int)difftime(endTime, startTime);
}

int main() {
    iniBoard();
    shuffleBoard();
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(FALSE);
    refresh();

    int maxY, maxX;
    getmaxyx(stdscr, maxY, maxX);

    int wHeight = 24;
    int wWidth = 36;

    int startY = (maxY - wHeight) / 2;
    int startX = (maxX - wWidth) / 2;

    WINDOW *gamew = newwin(wHeight, wWidth, startY, startX);
    box(gamew, 0, 0);
    wrefresh(gamew);

    printBoard(gamew);

    startTimer();

    while (1) {
        if (isSolved()) {
            int elapsedTime = stopTimer();
            wprintw(gamew, "\n           wp!\n\n\n\ntempo: %d segundos\n", elapsedTime);
            wrefresh(gamew);
            sleep(DELAY);
            break;
        }
        int move = getch();

        switch (move) {
            case KEY_UP:
                makeMove(emptyRow + 1, emptyCol);
                break;
            case KEY_DOWN:
                makeMove(emptyRow - 1, emptyCol);
                break;
            case KEY_LEFT:
                makeMove(emptyRow, emptyCol + 1);
                break;
            case KEY_RIGHT:
                makeMove(emptyRow, emptyCol - 1);
                break;
            case 27:
                endwin();
                exit(0);
            default:
                break;
        }
        printBoard(gamew);
    }
    endwin();
    return 0;
}
