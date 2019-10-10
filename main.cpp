#include <cstdlib>
#include <time.h>
#include "draw.hpp"
#include "octree.hpp"

#define BOARD_X   32
#define BOARD_Y   32
#define BOARD_Z   32
#define LIVE      1
#define DEAD      0
#define PERIOD 	  250

static int curr_board[BOARD_X][BOARD_Y][BOARD_Z] = {};
static int next_board[BOARD_X][BOARD_Y][BOARD_Z] = {};

void
init_board ()
{
    for (int x = 0; x < BOARD_X; x++)
        for (int y = 0; y < BOARD_Y; y++)
            for (int z = 0; z < BOARD_Z; z++)
                if (rand() % 500 > 490)
                    curr_board[x][y][z] = 1;
                else
                    curr_board[x][y][z] = 0;
}

/*
 * Count each neighbor either being dead or living.  This board does not loop
 * so edges & corners will have less possible neighbors.
 */
int
cell_neighbors (int x, int y, int z, int type)
{
    int neighbors = 0;

    for (int dy = y - 1; dy <= y + 1; dy++) {
        if (dy < 0)
            continue;
        if (dy > BOARD_Y)
            continue;

        /* center */
        if (dy != y && curr_board[x][dy][z] == type)
            neighbors++;

        /* top */
        if (z < BOARD_Z && curr_board[x][dy][z + 1] == type)
                neighbors++;
        /* right */
        if (x < BOARD_X && curr_board[x + 1][dy][z] == type)
                neighbors++;
        /* bottom */
        if (z > 0 && curr_board[x][dy][z - 1] == type)
                neighbors++;
        /* left */
        if (x > 0 && curr_board[x - 1][dy][z] == type)
                neighbors++;

        /* top left */
        if ((x > 0 && z < BOARD_Z) && curr_board[x - 1][dy][z + 1] == type)
                neighbors++;
        /* top right */
        if (x < BOARD_X && z < BOARD_Z && curr_board[x + 1][dy][z + 1] == type)
                neighbors++;
        /* bottom right */
        if (x < BOARD_X && z > 0 && curr_board[x + 1][dy][z - 1] == type)
                neighbors++;
        /* bottom left */
        if (x > 0 && z > 0 && curr_board[x - 1][dy][z - 1] == type)
                neighbors++;
    }

    return neighbors;
}

void
step_board ()
{
    int neighbors;
    int axis;
    int dir;
    int ix, iy, iz;

    for (int x = 0; x < BOARD_X; x++) {
        for (int y = 0; y < BOARD_Y; y++) {
            for (int z = 0; z < BOARD_Z; z++) {
                //if (curr_board[x][y][z] == LIVE) {
                //    neighbors = cell_neighbors(x, y, z, LIVE);
                //    if (neighbors < 6 || neighbors > 10)
                //        next_board[x][y][z] = DEAD;
                //    else
                //        next_board[x][y][z] = LIVE;
                //}
                //else if (neighbors == 10) {
                //    next_board[x][y][z] = LIVE;
                //}

                if (curr_board[x][y][z] == DEAD)
                    continue;

                neighbors = cell_neighbors(x, y, z, LIVE);
                if (neighbors > 18) {
                    next_board[x][y][z] = LIVE;
                }
                else {
                    dir = (rand() % 2);
                    axis = rand() % 100;

                    if (dir)
                        dir = -1;
                    else
                        dir = 1;

                    ix = x;
                    iy = y;
                    iz = z;

                    if (axis < 33 && x > 0 && x < BOARD_X - 1)
                        ix += dir;
                    else if (axis < 66 && y > 0 && y < BOARD_Y - 1)
                        iy += dir;
                    else if (z > 0 && z < BOARD_Z - 1)
                        iz += dir;

                    if (next_board[ix][iy][iz] == DEAD)
                        next_board[ix][iy][iz] = LIVE;
                    else
                        next_board[x][y][z] = LIVE;
                }
            }
        }
    }

    memcpy(curr_board, next_board, BOARD_X * BOARD_Y * BOARD_Z * sizeof(int));
    memset(next_board, 0, BOARD_X * BOARD_Y * BOARD_Z * sizeof(int));
}

int
main (int argc, char **argv)
{
    unsigned long last_time = 0;
    Window window;
    window.lookat(BOARD_X / 2, BOARD_Y / 2, BOARD_Z / 2, BOARD_X * 5);

    srand(time(NULL));
    init_board();

    while (!window.should_close()) {
        window.handle_input();

        for (int x = 0; x < BOARD_X; x++)
            for (int y = 0; y < BOARD_Y; y++)
                for (int z = 0; z < BOARD_Z; z++)
                    if (curr_board[x][y][z])
                        window.draw_cube(x, y, z);

        //if (window.get_ticks() - last_time > PERIOD) {
        //    step_board();
        //    last_time = window.get_ticks();
        //}
        window.render();
    }

    return 0;
}
