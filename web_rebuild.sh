emcmake cmake -S . -B build --config Debug -DPLATFORM=Web -DCMAKE_BUILD_TYPE=Release
cp -r web/shell.html build/ #for --shell-file
cp -r assets/ build/
cmake --build build -j$(nproc)