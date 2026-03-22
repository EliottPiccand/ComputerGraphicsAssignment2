# Computer Graphics (CSED451-01) - Assignment 2: 2D Animation

by _Team Baguette_

## Playing the game
The game starts immediately on running the executable. The player can change the ship speed with the `W` and `S` keys (respectively increasing and decreasing the boat speed), and rotate it using the `A` and `D` keys (turing the boat respectively left and right by 15°). In addition, the player can shoot missiles with his mouse : pressing the left click enable aiming mode which displays a ray toward the target. In aiming mode, 2 actions ar possible : cancel fire by clicking (press and release) the right click, or fire by releasing the left click. Fullscreen can be toggle by clicking the F11 key.

## Building the game
To build the game, you need to :
- compile every `.cpp` file inside (recursively) `Src`
- set the include directory to `Includes`
- link against every lib inside `Lib`
- enable C++23 features

### Using CMake
Run those 2 commands into the project directory : 
```cmd
cmake -S . -B ./Build/CMake -G Ninja -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_MAKE_PROGRAM=ninja -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
cmake --build ./Build/CMake
```

### Using VisualStudio
If you are creating the project from scratch, you need to add those lines :
```xml
<ItemDefinitionGroup>
  <ClCompile>
    <ObjectFileName>$(IntDir)a/a/a/%(RelativeDir)%(Filename)%(Extension).obj</ObjectFileName>
  </ClCompile>
</ItemDefinitionGroup>
```

to the project's `.vcxproj`.

This remove linking issues with files that have the same name but not the same path.
