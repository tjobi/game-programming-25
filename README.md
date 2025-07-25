# Game Programming 25
Repository for the IT University of Copenhagen Game Programming course.

## Setup
You are free to use any development setup you want, as long as you can confortably build and debug CMake projects.

If you don't have an IDE setup, you can follow this quick platform-independent setup:
1. download and install [CMake](https://cmake.org/download/) (for mac users, see `brew`)
2. download [VSCode](https://code.visualstudio.com/download) and install the C/C++ Extension Pack (the one from Microsoft)
4. clone the exercise repository `git clone --recurse-submodules https://github.com/Chris-Carvelli/game-programming-25.git` 
5. open repository in VSCode
6. in `Preferences->Settings`, search for `cmake path` and replace the content with the path to your CMake executable (you can find in typing `where cmake` or `which cmake` on the command line)
7. restart the editor

After reopening the editor, you should see all available targets in the cmake tab, in the `Project Outline` section.

Build and run them from there, or set one to be the "default" target (`right-click->Set Launch/Debug Target`)
