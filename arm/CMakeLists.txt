cmake_minimum_required (VERSION 3.5.2)
# path to toolchain
set(CMAKE_TOOLCHAIN_FILE ../../toolchain/armv7/arm-cmake.txt)

#CMAKE_PREFIX_PATH

project (controller)

add_subdirectory (sources)

