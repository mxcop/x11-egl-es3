cmake_minimum_required(VERSION 3.15)
project(glad)

add_library(glad
    include/glad/egl.h
    src/egl.c
    include/glad/gl.h
    src/gl.c
    include/glad/gles2.h
    src/gles2.c
    include/glad/glx.h
    src/glx.c
)
target_include_directories(glad PUBLIC include/)

message(STATUS "GLAD = INCLUDED!")
