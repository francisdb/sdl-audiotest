# sdl-audiotest
Test application for SDL pinball audio

## Dependencies
```bash
sudo apt install libsdl2-dev
```

## Build
```bash
cmake -DCMAKE_BUILD_TYPE=Release -B build
cmake --build build -- -j$(nproc)
```

## Run
```bash
./build/sdl-audiotest
```

### Forcing an audio driver

```bash
SDL_AUDIODRIVER=pipewire ./build/sdl-audiotest
```

For a list of possible values see the [SDL FAQ](hhttps://wiki.libsdl.org/SDL2/FAQUsingSDL#how_do_i_choose_a_specific_audio_driver)
