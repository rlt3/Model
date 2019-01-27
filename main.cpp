#include <cstdlib>
#include <time.h>
#include "draw.hpp"
#include "octree.hpp"

#define BOARD_X   12
#define BOARD_Y   12
#define BOARD_Z   12
#define LIVE      1
#define DEAD      0
#define PERIOD 	  250

static int curr_board[BOARD_X][BOARD_Y][BOARD_Z];
static int next_board[BOARD_X][BOARD_Y][BOARD_Z];

void
init_board ()
{
    for (int x = 0; x < BOARD_X; x++)
        for (int y = 0; y < BOARD_Y; y++)
            for (int z = 0; z < BOARD_Z; z++)
                curr_board[y][x][z] = rand() % 2;
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

    for (int x = 0; x < BOARD_X; x++) {
        for (int y = 0; y < BOARD_Y; y++) {
            for (int z = 0; z < BOARD_Z; z++) {
                neighbors = cell_neighbors(x, y, z, LIVE);
                if (curr_board[y][x][z]) {
                    if (neighbors > 18)
                        next_board[y][x][z] = 0;
                } 
                else if (neighbors < 6) {
                    next_board[y][x][z] = 1;
                }
            }
        }
    }

    memcpy(curr_board, next_board, BOARD_X * BOARD_Y * BOARD_Z * sizeof(int));
}

int
main (int argc, char **argv)
{
    std::vector<glm::vec3> objects;

    for (int x = 0; x < BOARD_X; x++)
        for (int y = 0; y < BOARD_Y; y++)
            for (int z = 0; z < BOARD_Z; z++)
                objects.push_back(glm::vec3(x, y, z));

    Octree O(BoundingBox(glm::vec3(0), glm::vec3(BOARD_X, BOARD_Y, BOARD_Z)), objects);
    //O.print();

    Window window;
    window.lookat(BOARD_X / 2, BOARD_Y / 2, BOARD_Z / 2, BOARD_X * 5);

    while (!window.should_close()) {
        window.handle_input();
        for (auto &vec : objects)
            window.draw_cube(vec.x, vec.y, vec.z);
        window.render();
    }

    //unsigned long last_time = 0;
    //Window window;

    //srand(time(NULL));
    //init_board();

    //window.lookat(25, 25, 25);

    //while (!window.should_close()) {
    //    window.handle_input();
    //    for (int y = 0; y < BOARD_Y; y++)
    //        for (int x = 0; x < BOARD_X; x++)
    //            for (int z = 0; z < BOARD_Z; z++)
    //                if (curr_board[y][x][z])
    //                    window.draw_cube(x, y, z);

    //    if (window.get_ticks() - last_time > PERIOD) {
    //        step_board();
    //        last_time = window.get_ticks();
    //    }
    //    window.render();
    //}

    return 0;
}
