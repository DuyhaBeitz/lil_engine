#!/usr/bin/env bash
set -e

rm -rf dist
mkdir -p dist/lib dist/include

cp build/JoltBuild/libJolt.a         dist/lib/
cp build/lib/libr3d.a                dist/lib/
cp build/libtinyfiledialogs.a        dist/lib/
cp build/libimgui.a                  dist/lib/
cp build/libraylib-gizmo.a           dist/lib/
cp build/librlImGui.a                dist/lib/
cp build/liblil_engine.a             dist/lib/
cp build/liblil_editor.a             dist/lib/

if [ -d "include" ]; then
    cp -R include/* dist/include/
fi

cp cmake/lil_engine.cmake dist/
cp imgui.ini dist/