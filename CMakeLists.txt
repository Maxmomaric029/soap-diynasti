cmake_minimum_required(VERSION 3.10)
project(SoapDynasty)

set(CMAKE_CXX_STANDARD 17)

# Search for Raylib and its dependencies
find_package(raylib REQUIRED)
find_package(glfw3 REQUIRED)

# Include all source files
add_executable(SoapDynasty 
    src/main.cpp 
    src/Game.cpp 
    src/World.cpp
)

target_include_directories(SoapDynasty PRIVATE src)

# Link raylib, glfw and system libraries
target_link_libraries(SoapDynasty PRIVATE 
    raylib
    glfw
)

if(WIN32)
    target_link_libraries(SoapDynasty PRIVATE 
        winmm 
        opengl32 
        gdi32 
        shell32 
        user32 
        advapi32
    )
endif()

# Copy assets to build directory
add_custom_command(TARGET SoapDynasty POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_SOURCE_DIR}/assets"
    "$<TARGET_FILE_DIR:SoapDynasty>/assets")
