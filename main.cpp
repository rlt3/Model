#include "draw.hpp"

int
main (int argc, char **argv)
{
    Window window;

    while (!window.should_close()) {
        for (int y = 0; y < 5; y++)
            for (int x = 0; x < 5; x++)
                window.draw_cube(x * 1.2f, y * 1.2f, 0.0f);
        window.render();
    }

    return 0;
}
