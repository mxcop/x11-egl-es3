# X11 (Window Compositor)
find_package(X11 REQUIRED)

message(STATUS "X11_FOUND = ${X11_FOUND}")
message(STATUS "X11_INCLUDE_DIR = ${X11_INCLUDE_DIR}")
message(STATUS "X11_LIBRARIES = ${X11_LIBRARIES}")

include_directories(${X11_INCLUDE_DIR})

link_directories(${X11_LIBRARIES})

target_include_directories(program PRIVATE ${X11_INCLUDE_DIR})
target_link_libraries(program X11)

# Glad (OpenGL (ES))
add_subdirectory("glad")
target_link_libraries(program glad)
