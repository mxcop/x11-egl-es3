cmake_minimum_required(VERSION 3.15)
project(program)

# C++ standard version
set(CMAKE_CXX_STANDARD 17 CACHE STRING "" FORCE)

# Program executable target
add_executable(program main.cpp)
set_target_properties(program PROPERTIES OUTPUT_NAME ${CMAKE_PROJECT_NAME})

# Includes
target_include_directories(program PRIVATE ".")

# Libraries
target_link_libraries(program 
    X11    # Window Compositor API
    EGL    # Context API
    GLESv2 # OpenGL ES API
)
