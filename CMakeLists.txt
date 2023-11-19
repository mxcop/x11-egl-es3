cmake_minimum_required(VERSION 3.15)
project(x11-game)

# C++ standard version
set(CMAKE_CXX_STANDARD 17 CACHE STRING "" FORCE)

# Program executable target
add_executable(main)
set_target_properties(main PROPERTIES OUTPUT_NAME ${CMAKE_PROJECT_NAME})

# Add DEBUG compiler macro
target_compile_definitions(main PRIVATE "DEBUG=$<CONFIG:Debug>")

# Post build command (copies the asset files)
add_custom_command(TARGET main PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory 
    ${PROJECT_SOURCE_DIR}/assets 
    ${PROJECT_BINARY_DIR}/assets
)

# Includes
target_include_directories(main PRIVATE "./inc/" "./src/" "./src/engine/")
target_precompile_headers(main PRIVATE "./inc/libs.h")

# Shared libraries
target_link_libraries(main 
    X11    # Window Compositor API
    Xfixes # Extension for X11 (for hiding cursor)
    EGL    # Context API
    GLESv2 # OpenGL ES API
)

# Glob all source files
file(GLOB_RECURSE SOURCES
	"src/*.cpp"
)

# Add source files to the program
target_sources(main PRIVATE
	${SOURCES}
)

# Sub-directories
add_subdirectory("lib")
