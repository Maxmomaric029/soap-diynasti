cmake_minimum_required(VERSION 3.10)
project(SoapDynasty_CPP)

set(CMAKE_CXX_STANDARD 17)

# Search for Raylib
# You can install raylib via vcpkg: vcpkg install raylib
find_package(raylib REQUIRED)

add_executable(SoapDynasty_CPP src/main.cpp)

target_link_libraries(SoapDynasty_CPP raylib)

# Copy assets to build directory
add_custom_command(TARGET SoapDynasty_CPP POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_SOURCE_DIR}/assets"
    "$<TARGET_FILE_DIR:SoapDynasty_CPP>/assets")
