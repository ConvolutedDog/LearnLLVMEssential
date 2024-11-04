cmake_minimum_required(VERSION 3.20.0)
cmake_policy(SET CMP0116 NEW)

project(Chapter-04 LANGUAGES CXX C)

# Enable installation of output files into a specific directory.
set(CMAKE_BUILD_WITH_INSTALL_NAME_DIR ON)

set(CMAKE_CXX_STANDARD 17 CACHE STRING "C++ standard to conform to")
set(CMAKE_CXX_FLAGS "-Wfatal-errors -std=c++17")
add_compile_options(-w)

# Set the build type to Debug for generating debugging information.
set(CMAKE_BUILD_TYPE Debug)

# Find and configure the LLVM package, which is required for this project.
find_package(LLVM REQUIRED CONFIG)

# Append LLVM specific CMake module paths for locating additional CMake modules.
list(APPEND CMAKE_MODULE_PATH "${LLVM_CMAKE_DIR}")

# Include AddLLVM and HandleLLVMOptions CMake modules for additional setup.
include(AddLLVM)
include(HandleLLVMOptions)

message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

# Add LLVM-specific definitions needed during compilation.
add_definitions(${LLVM_DEFINITIONS})

# Include directories for LLVM.
include_directories(${LLVM_INCLUDE_DIRS})

# Link directories for LLVM libraries.
link_directories(${LLVM_LIBRARY_DIR})

add_subdirectory(PassesDir)

# Link against LLVM libraries.
# llvm_map_components_to_libnames(llvm_libs support core irreader)
# target_link_libraries(LLVMPassname PRIVATE ${llvm_libs})
