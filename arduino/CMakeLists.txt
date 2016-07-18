cmake_minimum_required (VERSION 3.5.2)
# path to toolchain
set(CMAKE_TOOLCHAIN_FILE ../../toolchain/arduino/arduino-cmake/cmake/ArduinoToolchain.cmake)

set(VERBOSE 1)

project (client)


add_subdirectory (sources)

