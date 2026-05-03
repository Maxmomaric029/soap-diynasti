cmake_minimum_required(VERSION 3.10)
project(SoapDynasty)

set(CMAKE_CXX_STANDARD 17)

# Search for Raylib and dependencies
find_package(raylib REQUIRED)

# Include all source files
add_executable(SoapDynasty 
    src/main.cpp 
    src/Game.cpp 
    src/World.cpp
)

target_include_directories(SoapDynasty PRIVATE src)

# Link raylib. vcpkg's raylib target should handle most things, 
# but we add system libs explicitly just in case for static builds.
target_link_libraries(SoapDynasty PRIVATE raylib)

if(WIN32)
    # These are often required for static raylib linking on Windows
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
