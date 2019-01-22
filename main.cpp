#include <cstdlib>
#include <time.h>
#include "draw.hpp"

#define BOARD_X   120
#define BOARD_Y   80
#define LIVE      1
#define DEAD      0
#define WHITE     1.0f, 1.0f, 1.0f, 1.0f
#define BLACK       0,   0,   0, 1.0f
#define PERIOD 	  250

static int curr_board[BOARD_Y][BOARD_X];
static int next_board[BOARD_Y][BOARD_X];

void
init_board ()
{
    for (int y = 0; y < BOARD_Y; y++)
        for (int x = 0; x < BOARD_X; x++)
            curr_board[y][x] = rand() % 2;
}

/*
 * Count each neighbor either being dead or living.  This board does not loop
 * so edges & corners will have less possible neighbors.
 */
int
cell_neighbors (int x, int y, int type)
{
    int neighbors = 0;

    /* top */
    if (y > 0 && curr_board[y - 1][x] == type)
            neighbors++;
    /* right */
    if (x < BOARD_X && curr_board[y][x + 1] == type)
            neighbors++;
    /* bottom */
    if (y < BOARD_Y && curr_board[y + 1][x] == type)
            neighbors++;
    /* left */
    if (x > 0 && curr_board[y][x - 1] == type)
            neighbors++;

    /* top left */
    if ((x > 0 && y > 0) && curr_board[y - 1][x - 1] == type)
            neighbors++;
    /* top right */
    if ((x < BOARD_X && y > 0) && curr_board[y - 1][x + 1] == type)
            neighbors++;
    /* bottom right */
    if ((x < BOARD_X && y < BOARD_Y) && curr_board[y + 1][x + 1] == type)
            neighbors++;
    /* bottom left */
    if ((x > 0 && y < BOARD_Y) && curr_board[y + 1][x - 1] == type)
            neighbors++;

    return neighbors;
}

void
step_board ()
{
    int neighbors;

    for (int y = 0; y < BOARD_Y; y++) {
        for (int x = 0; x < BOARD_X; x++) {
            neighbors = cell_neighbors(x, y, LIVE);
            if (curr_board[y][x]) {
                /* 
                 * Any live cell with fewer than two live neighbours dies, as
                 * if caused by under-population. Any live cell with more than
                 * three live neighbours dies, as if by over-population.
                 */
                if (neighbors < 2 || neighbors > 3)
                    next_board[y][x] = 0;
                /* 
                 * Any live cell with two or three live neighbours lives on to
                 * the next generation.
                 */
                else
                    next_board[y][x] = 1;
            } 
            /* 
             * Any dead cell with exactly three live neighbours becomes a live
             * cell, as if by reproduction.
             */
            else if (neighbors == 3) {
                next_board[y][x] = 1;
            }
        }
    }

    memcpy(curr_board, next_board, BOARD_Y * BOARD_X * sizeof(int));
}

int
main (int argc, char **argv)
{
    unsigned long last_time = 0;
    Window window;

    srand(time(NULL));
    init_board();

    while (!window.should_close()) {
        window.handle_input();
        for (int y = 0; y < BOARD_Y; y++)
            for (int x = 0; x < BOARD_X; x++)
                if (curr_board[y][x])
                    window.draw_cube(x, 0.0f, y);

        if (window.get_ticks() - last_time > PERIOD) {
            step_board();
            last_time = window.get_ticks();
        }
        window.render();
    }

    return 0;
}
