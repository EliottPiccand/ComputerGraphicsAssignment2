# Computer Graphics (CSED451-01) - Assignment 2: 2D Animation

by _Team Baguette_

## Playing the game
The game starts immediately on running the executable. The player can change the ship speed with the `W` and `S` keys (respectively increasing and decreasing the boat speed), and rotate it using the `A` and `D` keys (turing the boat respectively left and right by 15°). In addition, the player can shoot missiles with his mouse : pressing the left click enable aiming mode which displays a ray toward the target. In aiming mode, 2 actions ar possible : cancel fire by clicking (press and release) the right click, or fire by releasing the left click. Fullscreen can be toggle by clicking the F11 key.

## Building the game
This project use CMake.

### With pure CMake
```cmd
cmake -B Build/Release -DCMAKE_BUILD_TYPE=Release
cmake -B Build/Debug -DCMAKE_BUILD_TYPE=Debug
cmake -B Build/Profiling -DCMAKE_BUILD_TYPE=Profiling
```

And then, compile with
```cmake
cmake --build Build/Release
cmake --build Build/Debug
cmake --build Build/Profiling
```

### Using the Profiling Profile
[Tracy](https://github.com/wolfpld/tracy) must be added as a Git submodule (see `.gitmodules`) to be able to compile with the Profiling profile. Note that to use Tracy, the submodule should be checkout to the last stable version like
```cmd
git submodule init
cd Lib/tracy
git checkout v0.13.1
```
