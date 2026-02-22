cmake_minimum_required(VERSION 3.16)
project(SuperMarioBrosClone VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Find SDL2 packages
find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL2 REQUIRED sdl2)
pkg_check_modules(SDL2_IMAGE REQUIRED SDL2_image)
pkg_check_modules(SDL2_MIXER REQUIRED SDL2_mixer)

# Collect source files
file(GLOB_RECURSE SOURCES
    src/*.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE
    src
    ${SDL2_INCLUDE_DIRS}
    ${SDL2_IMAGE_INCLUDE_DIRS}
    ${SDL2_MIXER_INCLUDE_DIRS}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    ${SDL2_LIBRARIES}
    ${SDL2_IMAGE_LIBRARIES}
    ${SDL2_MIXER_LIBRARIES}
)

target_compile_options(${PROJECT_NAME} PRIVATE
    ${SDL2_CFLAGS_OTHER}
    -Wall -Wextra -Wpedantic
)

# Copy assets to build directory
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:${PROJECT_NAME}>/assets
)
