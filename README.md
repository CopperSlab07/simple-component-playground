# Simple Component Playground (HEAVILY WORK IN PROGRESS!)

**Caution! Ignore this project if you:**

* Wants something fast & efficient to use professionally
* Reject bad & ugly code with a passion (or not, but do that matter anyway?)

## Build instructions:

* Step 1: fetch dependencies and put them inside **vendor/** :
    * Raylib (choose the newest release or build from master branch): https://github.com/raysan5/raylib
    * Raygui (choose the newest release or get raygui.h from master branch): https://github.com/raysan5/raygui
    * nob.h (just get from master branch): https://github.com/tsoding/nob.h

* Step 2: build:
    * Get nob.c from OS-dependent branches (currently linux and windows, the windows do not support MSVC right now.)
    * Change nob.c to your OS's dependencies (below are just examples for system libraries, check comments in nob.c for a little bit more)
        * Windows: -lm -lgdi32 -lwinmm
        * Linux:   -lm -lX11 -lXext -lGL
        * MacOS:   (not yet implemented)

    * Bootstrap the nob.c with gcc/clang (mingw-w64 works as well):
    ```bash
    gcc -o nob nob.c
    ```
    
    * Run the build tool:
    ```bash
    ./nob
    ```

## Extras
* todo.c: Tests for UI stuff with raygui.h, built the same way as main.c.

## Licenses: 
* For todo.c, main.c, components.h and assets/sell.png: MIT license
* For dependencies and nob.c: according to each dependencies:
    * raylib & raygui: Zlib license
    * nob.h & nob.c: MIT license

# TODOs:
**For now, go look in TODO.md.**
