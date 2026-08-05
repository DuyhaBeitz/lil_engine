# Lil Engine
## C++ game engine with an editor
![huh](https://github.com/DuyhaBeitz/lil_engine/blob/master/assets/Screenshot_2026-07-31_13-01-11.png)
## Features:
- Reflection system (write your class, specify reflected fields and use it in the editor)
- Serialization (load/save your objects or scenes)
- Crossplatform

## Usage:
You still write your regular raylib code + Lil::api

See [src/main.cpp](src/main.cpp) and [src/LilEditor/LilEditor.cpp](src/LilEditor/LilEditor.cpp) for more

## NOTE:
Right now I use precompiled lib binaries (*.a) for quick builds, they are only for linux and web, for other platforms you might want to use fetch content or compile libs yourself

## TODO:
- networking (Probably desktop only)
- there will be a continuation of this project (or just a separate branch, or maybe a module) that uses R3D library for advanced rendering (desktop only)
