#!/usr/bin/env bash
set -e

rm -rf lil_engine_sdk
mkdir -p lil_engine_sdk/lib lil_engine_sdk/web-lib lil_engine_sdk/android-lib/arm64-v8a lil_engine_sdk/include

cp build/JoltBuild/libJolt.a             lil_engine_sdk/lib/
cp build/libtinyfiledialogs.a            lil_engine_sdk/lib/
cp build/libimgui.a                      lil_engine_sdk/lib/
cp build/libraylib-gizmo.a               lil_engine_sdk/lib/
cp build/librlImGui.a                    lil_engine_sdk/lib/
cp build/liblil_engine.a                 lil_engine_sdk/lib/
cp build/liblil_editor.a                 lil_engine_sdk/lib/
cp build/libconfigparser.a               lil_engine_sdk/lib/


cp web/libraylib.a                       lil_engine_sdk/web-lib/
cp web-build/JoltBuild/libJolt.a         lil_engine_sdk/web-lib/
cp web-build/libtinyfiledialogs.a        lil_engine_sdk/web-lib/
cp web-build/libimgui.a                  lil_engine_sdk/web-lib/
cp web-build/libraylib-gizmo.a           lil_engine_sdk/web-lib/
cp web-build/librlImGui.a                lil_engine_sdk/web-lib/
cp web-build/liblil_engine.a             lil_engine_sdk/web-lib/
cp web-build/liblil_editor.a             lil_engine_sdk/web-lib/
cp web-build/libconfigparser.a           lil_engine_sdk/web-lib/

cp android/arm64-v8a/libraylib.a                 lil_engine_sdk/android-lib/arm64-v8a/
cp android-build-arm64-v8a/JoltBuild/libJolt.a   lil_engine_sdk/android-lib/arm64-v8a/
cp android-build-arm64-v8a/libimgui.a            lil_engine_sdk/android-lib/arm64-v8a/
cp android-build-arm64-v8a/libraylib-gizmo.a     lil_engine_sdk/android-lib/arm64-v8a/
cp android-build-arm64-v8a/librlImGui.a          lil_engine_sdk/android-lib/arm64-v8a/
cp android-build-arm64-v8a/liblil_engine.a       lil_engine_sdk/android-lib/arm64-v8a/
cp android-build-arm64-v8a/libconfigparser.a     lil_engine_sdk/android-lib/arm64-v8a/

if [ -d "include" ]; then
    cp -R include/* lil_engine_sdk/include/
fi

cp cmake/sdk_basic/lil_engine.cmake lil_engine_sdk/
cp imgui.ini lil_engine_sdk/