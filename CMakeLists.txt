cmake_minimum_required(VERSION 3.27)
project(sdl-audiotest C)

set(CMAKE_C_STANDARD 11)

add_executable(sdl-audiotest main.c)

# add linker flags for ubuntu global SDL2 and math
target_link_libraries(sdl-audiotest m -lSDL2 -lSDL2_mixer)

#copy assets to build folder
file(COPY assets DESTINATION ${CMAKE_BINARY_DIR})
