# sdl-audiotest
Test application for SDL pinball audio

## Dependencies
```
sudo apt install libsdl2-dev
```

## Build
```
cmake -DCMAKE_BUILD_TYPE=Release -B build
cmake --build build -- -j$(nproc)
```

## Run
```
./build/sdl-audiotest
```