ANDROID_NDK_HOME=~/Android/Sdk/ndk/28.2.13676358
ABI=arm64-v8a #or x86_64 for the emulator

cmake -S . -B android-build-$ABI -DPLATFORM=Android -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_TOOLCHAIN_FILE=$ANDROID_NDK_HOME/build/cmake/android.toolchain.cmake \
    -DANDROID_ABI=$ABI -DANDROID_PLATFORM=android-24 -DANDROID_STL=c++_static
cmake --build android-build-$ABI --target lil_engine -j$(nproc)