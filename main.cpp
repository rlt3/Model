#include "draw.hpp"

int
main (int argc, char **argv)
{
    Window window;

    while (!window.should_close()) {
        window.render();
    }

    return 0;
}
