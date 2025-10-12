cmake_minimum_required(VERSION 3.10)
project(ThreeBodySimulation)

# Set C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(PkgConfig REQUIRED)
pkg_check_modules(GLFW3 REQUIRED glfw3)

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)

# Include directories
include_directories(${GLFW3_INCLUDE_DIRS})
include_directories(${OPENGL_INCLUDE_DIRS})
include_directories(${GLEW_INCLUDE_DIRS})



# Create library for shared components
add_library(common_lib
    src/font.cpp
    src/physics.cpp
    src/ui.cpp
)

# Link common library with required libraries
target_link_libraries(common_lib 
    ${GLFW3_LIBRARIES}
    ${OPENGL_LIBRARIES}
    ${GLEW_LIBRARIES}
)

# Main simulation executable
add_executable(sim 
    simul.cpp 
)

# Font test executable
add_executable(testfont
    testfont.cpp
)

# Link executables with common library
target_link_libraries(sim common_lib)
target_link_libraries(testfont common_lib)

# Compiler flags for both executables
target_compile_options(sim PRIVATE ${GLFW3_CFLAGS_OTHER})
target_compile_options(testfont PRIVATE ${GLFW3_CFLAGS_OTHER})

# Set output directory for both
set_target_properties(sim testfont PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)

# Print build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ flags: ${CMAKE_CXX_FLAGS}")
message(STATUS "C++ flags (Release): ${CMAKE_CXX_FLAGS_RELEASE}")
message(STATUS "C++ flags (Debug): ${CMAKE_CXX_FLAGS_DEBUG}")