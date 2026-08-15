# Lil Engine - Advanced Rendering Branch
## C++ game engine with an editor
![huh](https://github.com/DuyhaBeitz/lil_engine/blob/advanced_rendering/assets/screenshot_advanced_render.png)
## Features:
- Reflection system (write your class, specify reflected fields and use it in the editor)
- Serialization (load/save your objects or scenes)
- Crossplatform (advanced rendering is Desktop only, for more platforms see master branch)

## Usage:
You still write your regular raylib code + Lil::api

See [src/main.cpp](src/main.cpp) and [src/LilEditor/LilEditor.cpp](src/LilEditor/LilEditor.cpp) for more

## NOTE:
Right now I use precompiled lib binaries (*.a) for quick builds, they are only for linux and web, for other platforms you might want to use fetch content or compile libs yourself

## TODO:
- networking (Probably desktop only)
