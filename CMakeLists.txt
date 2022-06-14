cmake_minimum_required(VERSION 3.0.0)

# Set <PROJNAME> to be your project name. We set C here as the target language.
project(hw3 VERSION 0.1.0 LANGUAGES C)

# If you want to see "under the hood" of CMake, uncomment this:
# set(CMAKE_VERBOSE_MAKEFILE ON)

# Define variables containing compiler flags for gcc
set(MTM_FLAGS_DEBUG "-std=c99 -g -Wall -lpthread")
set(MTM_FLAGS_RELEASE "${MTM_FLAGS_DEBUG} -DNDEBUG")

# Set the flags for gcc
set(CMAKE_C_FLAGS ${MTM_FLAGS_DEBUG})


# Add the names of your source files here
set(C_FILES ./server.c ./request.c ./segel.c)

add_executable(os_hw3 ${C_FILES})