#include <stdio.h>
#include <stdbool.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <unistd.h>

// Define MAX and MIN macros
#define MAX(X, Y) (((X) > (Y)) ? (X) : (Y))
#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))

// Define screen dimensions
#define SCREEN_WIDTH    800
#define SCREEN_HEIGHT   600

#define MONO_BALL_SOUND              "assets/fx_ballhit0.wav"

bool render_loop(SDL_Renderer *renderer, Mix_Chunk *clapnsnare1, SDL_Rect *squareRect, SDL_Rect *pauseRect1, SDL_Rect *pauseRect2, SDL_AudioSpec *audio_spec, SDL_AudioDeviceID audio_device);

void view(SDL_Rect *squareRect, SDL_Rect *pauseRect1, SDL_Rect *pauseRect2);

void play_sound(int channel, Mix_Chunk *chunk);

void print_cwd();

const char *select_device();

void sine(SDL_AudioSpec *audio_spec, SDL_AudioDeviceID audio_device, int time_seconds, int enabled_channels);

char *channel_names(const SDL_AudioSpec *audio_spec, const int enabled_channels);

void test_speakers(SDL_AudioSpec *audio_spec, SDL_AudioDeviceID audio_device);

int main(int argc, char* argv[])
{
    // Unused argc, argv
    (void) argc;
    (void) argv;

    print_cwd();

    // Initialize SDL
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
    {
        printf("SDL could not be initialized!\n"
               "SDL_Error: %s\n", SDL_GetError());
        return 0;
    }


    const char *name = select_device();

    // define audio spec
    SDL_AudioSpec want, audio_spec;
    SDL_zero(want);
    want.freq = 44100;//48000;
    want.format = AUDIO_S16SYS;//AUDIO_F32;
    want.channels = 8;
    want.samples = 4096;
    want.callback = NULL;
    want.userdata = NULL;

    SDL_AudioDeviceID audio_device = SDL_OpenAudioDevice(name, 0, &want, &audio_spec, 0);

    if(audio_device == 0)
    {
        printf("SDL could not open audio device!\n"
               "SDL_Error: %s\n", SDL_GetError());
        return 0;
    }

    printf("SDL audio driver: %s\n", SDL_GetCurrentAudioDriver());

    // print the spec we got including channels
    printf("SDL audio spec:\n"
           "  freq: %d\n"
           "  format: %d\n"
           "  channels: %d\n"
           "  silence: %d\n"
           "  samples: %d\n"
           "  size: %d\n",
           audio_spec.freq, audio_spec.format, audio_spec.channels, audio_spec.silence, audio_spec.samples, audio_spec.size);

    // unpausing the audio device (starts playing):
    SDL_PauseAudioDevice(audio_device, 0);

    //SDL_Delay(3000);


    //Initialize SDL2_mixer
    if(Mix_OpenAudioDevice(22050, MIX_DEFAULT_FORMAT, 6, 4096, name, 0 ) == -1)
    {
        printf("SDL2_mixer could not be initialized!\n"
               "SDL_Error: %s\n", SDL_GetError());
        return 0;
    }

    // make sure the mixer has enough channels to play all the sounds
    int num_channels = 256;
    if(Mix_AllocateChannels(num_channels) != num_channels)
    {
        printf("SDL2_mixer could not allocate channels!\n"
               "SDL_Error: %s\n", SDL_GetError());
        return 0;
    }

#if defined linux && SDL_VERSION_ATLEAST(2, 0, 8)
    // Disable compositor bypass
    if(!SDL_SetHint(SDL_HINT_VIDEO_X11_NET_WM_BYPASS_COMPOSITOR, "0"))
    {
        printf("SDL can not disable compositor bypass!\n");
        return 0;
    }
#endif

    // Create window
    SDL_Window *window = SDL_CreateWindow("SDL2 audio sample (Press SPACE to pause/play)",
                                          SDL_WINDOWPOS_UNDEFINED,
                                          SDL_WINDOWPOS_UNDEFINED,
                                          SCREEN_WIDTH, SCREEN_HEIGHT,
                                          SDL_WINDOW_SHOWN);
    if(!window)
    {
        printf("Window could not be created!\n"
               "SDL_Error: %s\n", SDL_GetError());
    }
    else
    {
        // Create renderer
        SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        if(!renderer)
        {
            printf("Renderer could not be created!\n"
                   "SDL_Error: %s\n", SDL_GetError());
        }
        else
        {
            // print current directory
            print_cwd();

            // Load sounds
            Mix_Chunk *mono_ball_sound = Mix_LoadWAV(MONO_BALL_SOUND);
            if(!mono_ball_sound )
            {
                printf("One of the sounds could not be loaded!\n"
                       "SDL_Error: %s\n", SDL_GetError());
                return 0;
            }

            SDL_Rect squareRect;
            SDL_Rect pauseRect1;
            SDL_Rect pauseRect2;
            view(&squareRect, &pauseRect1, &pauseRect2);


            // Event loop exit flag
            bool quit = false;

            // Event loop
            while(!quit)
            {
                quit = render_loop(renderer, mono_ball_sound, &squareRect,
                                   &pauseRect1, &pauseRect2, &audio_spec, audio_device);
            }

            // Clean up audio
            Mix_FreeChunk(mono_ball_sound);

            // Destroy renderer
            SDL_DestroyRenderer(renderer);
        }

        // Destroy window
        SDL_DestroyWindow(window);
    }

    SDL_CloseAudioDevice(audio_device);

    // Quit SDL2_mixer
    Mix_CloseAudio();

    // Quit SDL
    SDL_Quit();

    return 0;
}

void test_speakers(SDL_AudioSpec *audio_spec, SDL_AudioDeviceID audio_device) {
    // 0 FL
    // 1 FR
    // 2 FC
    // 3 LFE
    // 4 RL
    // 5 RR
    // 6 SL (FL pulseaudio)
    // 7 SR (FR pulseaudio)
    sine(audio_spec, audio_device, 1, 0b00000001); // 0
    sine(audio_spec, audio_device, 1, 0b00000010); // 1
    sine(audio_spec, audio_device, 1, 0b01000000); // 6
    sine(audio_spec, audio_device, 1, 0b10000000); // 7
    sine(audio_spec, audio_device, 1, 0b00010000); // 4
    sine(audio_spec, audio_device, 1, 0b00100000); // 5
}

void sine(SDL_AudioSpec *audio_spec, SDL_AudioDeviceID audio_device, const int time_seconds, const int enabled_channels) {
    char *enabled_channel_names = channel_names(audio_spec, enabled_channels);
    // print "Play test sound on channels 0 and 1 for 3 seconds"
    printf("Play test sound on %s for %d seconds\n", enabled_channel_names, time_seconds);
    // free the string
    free(enabled_channel_names);

    int16_t *buffer = malloc(sizeof(int16_t) * (*audio_spec).channels);
    uint sample_size = sizeof(int16_t) * (*audio_spec).channels;
    float x = 0;
    for (int i = 0; i < (*audio_spec).freq * time_seconds; i++) {
        x += .010f;
        for(int j = 0; j < (*audio_spec).channels; j++) {
            // check channel bit
            bool enabled = (enabled_channels >> j) & 1;
            if ( enabled){
                // SDL_QueueAudio expects a signed 16-bit value
                // note: "5000" here is just gain so that we will hear something
                buffer[j] = sin(x * 6) * 6000;
            } else {
                buffer[j] = 0;
            }
        }
        SDL_QueueAudio(audio_device, buffer, sample_size);
    }

    // free the buffer
    free(buffer);
}


char *channel_names(const SDL_AudioSpec *audio_spec, const int enabled_channels) {
    char* channels[8] = {"FL", "FR", "FC", "LFE", "RL", "RR", "SL", "SR"};
    char *enabled_channel_names = malloc(sizeof(char) * 100);
    enabled_channel_names[0] = '\0';
    bool first = true;
    for(int i = 0; i < (*audio_spec).channels; i++) {
        // check channel bit
        bool enabled = (enabled_channels >> i) & 1;
        if ( enabled){
            if (first) {
                first = false;
            } else {
                strcat(enabled_channel_names, ", ");
            }
            strcat(enabled_channel_names, channels[i]);
        }
    }
    return enabled_channel_names;
}

const char *select_device() {// list all devices
    int num_audio_devices = SDL_GetNumAudioDevices(0);
    printf("Number of audio devices: %d\n", num_audio_devices);
    // print all devices
    for(int i = 0; i < num_audio_devices; i++)
    {
        printf("Audio device %d: %s\n", i, SDL_GetAudioDeviceName(i, 0));
    }

    // device number to use
    int selected = 0;
    const char *name = SDL_GetAudioDeviceName(selected, 0);
    printf("Using audio device %d: %s\n", selected, name);
    return name;
}

void print_cwd() {
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL)
        fprintf(stdout, "Current working dir: %s\n", cwd);
    else
        perror("getcwd() error");
}

void view(SDL_Rect *squareRect, SDL_Rect *pauseRect1, SDL_Rect *pauseRect2) {// Declare rect of square
// Square dimensions: Half of the min(SCREEN_WIDTH, SCREEN_HEIGHT)
    (*squareRect).w = MIN(SCREEN_WIDTH, SCREEN_HEIGHT) / 2;
    (*squareRect).h = MIN(SCREEN_WIDTH, SCREEN_HEIGHT) / 2;

    // Square position: In the middle of the screen
    (*squareRect).x = SCREEN_WIDTH / 2 - (*squareRect).w / 2;
    (*squareRect).y = SCREEN_HEIGHT / 2 - (*squareRect).h / 2;


    // Declare rects of pause symbol
    (*pauseRect1).h = (*squareRect).h / 2;
    (*pauseRect1).w = 40;
    (*pauseRect1).x = (*squareRect).x + ((*squareRect).w - (*pauseRect1).w * 3) / 2;
    (*pauseRect1).y = (*squareRect).y + (*squareRect).h / 4;
    (*pauseRect2) = (*pauseRect1);
    (*pauseRect2).x += (*pauseRect1).w * 2;
}

void play_sound(int channel, Mix_Chunk *chunk) {
    printf("Playing sound on channel %d\n", channel);

    // set position to front left
    int random_angle = rand() % 360;
    if(Mix_SetPosition(channel, random_angle, 128) == 0)
    {
        printf("Could not set position!\n"
               "SDL_Error: %s\n", SDL_GetError());
        return;
    }

    int channel_played = Mix_PlayChannel(channel, chunk, 0);
    printf("Played sound on channel %d\n", channel_played);
    if(channel_played == -1)
    {
        printf(".OGG sound could not be played!\n"
               "SDL_Error: %s\n", SDL_GetError());
    }
}

bool render_loop(SDL_Renderer *renderer, Mix_Chunk *mono_ball_sound, SDL_Rect *squareRect, SDL_Rect *pauseRect1, SDL_Rect *pauseRect2, SDL_AudioSpec *audio_spec, SDL_AudioDeviceID audio_device){
    bool quit = false;
    SDL_Event e;

    // check what keys are down
//    const Uint8 *state = SDL_GetKeyboardState(NULL);
//    if (state[SDL_SCANCODE_UP]) {
//        play_sound(1, clapnsnare3);
//    }

    // Wait indefinitely for the next available event
    int result = SDL_WaitEventTimeout(&e, 5);
    if(result == 0) {
        //printf("No event\n");
        return quit;
    }

    // User requests quit
    if(e.type == SDL_QUIT)
    {
        quit = true;
    }
    else if(e.type == SDL_KEYDOWN)
    {
        switch (e.key.keysym.sym)
        {
            case SDLK_SPACE:
                if(Mix_Paused(-1))
                    Mix_Resume(-1);
                else
                    Mix_Pause(-1);
                break;

            case SDLK_RETURN:
                play_sound(2, mono_ball_sound);
                break;
        }
    }
    else if(e.type == SDL_MOUSEBUTTONDOWN)
    {
        // get mouse button
        int button = e.button.button;
        //switch on button
        switch(button)
        {
            case SDL_BUTTON_LEFT: {
                // get mouse position
                int x, y;
                SDL_GetMouseState(&x, &y);
                printf("Mouse click at %d, %d\n", x, y);

                // scale to 0-255
                x = (x * 255) / SCREEN_WIDTH;
                y = (y * 255) / SCREEN_HEIGHT;

                // convert to angle and distance
                int angle = 90 + (int) (atan2(y - 128, x - 128) * 180 / M_PI);
                int distance = (int) (sqrt((x - 128) * (x - 128) + (y - 128) * (y - 128)) * 255 / 128);

                printf("Angle: %d, distance: %d\n", angle, distance);

                // set position to mouse position
                if (Mix_SetPosition(1, angle, distance) == 0) {
                    printf("Could not set position!\n"
                           "SDL_Error: %s\n", SDL_GetError());
                    return quit;
                }

                // play sound
                if (Mix_PlayChannel(1, mono_ball_sound, 0) == -1) {
                    printf(".OGG sound could not be played!\n"
                           "SDL_Error: %s\n", SDL_GetError());
                }
                break;
            }
            case SDL_BUTTON_RIGHT:
                test_speakers(audio_spec, audio_device);
                break;
            case SDL_BUTTON_MIDDLE:
                break;
            default:
                break;
        }


    }
    else if(e.type == SDL_AUDIODEVICEADDED){
        // get the data
        SDL_AudioDeviceEvent *adev = (SDL_AudioDeviceEvent*) &e;
        printf("SDL_AUDIODEVICEADDED: %d\n", adev->which);
    }
    else if(e.type == SDL_AUDIODEVICEREMOVED){
        // get the data
        SDL_AudioDeviceEvent *adev = (SDL_AudioDeviceEvent*) &e;
        printf("SDL_AUDIODEVICEREMOVED: %d\n", adev->which);
    }

    // Initialize renderer color white for the background
    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);

    // Clear screen
    SDL_RenderClear(renderer);

    // Set renderer color blue to draw the square
    SDL_SetRenderDrawColor(renderer, 0x19, 0x71, 0xA9, 0xFF);

    // Draw filled square
    SDL_RenderFillRect(renderer, squareRect);

    // Check pause status
    if(Mix_Paused(-1))
    {
        // Set renderer color white to draw the pause symbol
        SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);

        // Draw pause symbol
        SDL_RenderFillRect(renderer, pauseRect1);
        SDL_RenderFillRect(renderer, pauseRect2);
    }


    // Update screen
    SDL_RenderPresent(renderer);
    return quit;
}
