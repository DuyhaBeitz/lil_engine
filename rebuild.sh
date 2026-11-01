cmake -S . -B build -DPLATFORM=Desktop -DBACKEND=Basic -DCMAKE_BUILD_TYPE=Debug
cmake --build build -j$(nproc)

ln -sf "$(pwd)/assets" build/