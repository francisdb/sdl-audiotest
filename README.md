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

## Linux audio debugging

Use `helvum` to check pipewire wiring.

Use `hdajackretask` from `alsa-tools-gui` to remap audio jacks on the motherboard (Realtek).


```bash
aplay -l                                      
**** List of PLAYBACK Hardware Devices ****
card 0: PCH [HDA Intel PCH], device 0: ALC897 Analog [ALC897 Analog]
  Subdevices: 1/1
  Subdevice #0: subdevice #0
card 1: HDMI [HDA ATI HDMI], device 3: HDMI 0 [LG HDR 4K]
  Subdevices: 1/1
  Subdevice #0: subdevice #0
card 1: HDMI [HDA ATI HDMI], device 7: HDMI 1 [LG ULTRAFINE]
  Subdevices: 1/1
  Subdevice #0: subdevice #0
card 1: HDMI [HDA ATI HDMI], device 8: HDMI 2 [HDMI 2]
  Subdevices: 1/1
  Subdevice #0: subdevice #0
card 1: HDMI [HDA ATI HDMI], device 9: HDMI 3 [HDMI 3]
  Subdevices: 1/1
  Subdevice #0: subdevice #0
card 2: X [Razer Barracuda X], device 0: USB Audio [USB Audio]
  Subdevices: 1/1
  Subdevice #0: subdevice #0
card 3: ICUSBAUDIO7D [ICUSBAUDIO7D], device 0: USB Audio [USB Audio]
  Subdevices: 1/1
  Subdevice #0: subdevice #0
```

```bash
speaker-test -c 8 -t wav -l 1 --device hw:3,0

speaker-test 1.2.9

Playback device is hw:3,0
Stream parameters are 48000Hz, S16_LE, 8 channels
WAV file(s)
Rate set to 48000Hz (requested 48000Hz)
Buffer size range from 96 to 96000
Period size range from 48 to 48000
Using max buffer size 96000
Periods = 4
was set period_size = 24000
was set buffer_size = 96000
 0 - Front Left
 2 - Front Center
 1 - Front Right
 5 - Side Right
 7 - Side Right
 4 - Side Left
 6 - Side Left
 3 - LFE
Time per period = 9,489026
```