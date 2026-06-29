cmake_minimum_required(VERSION 3.15)

project(Arma3AttendanceBot VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(Threads REQUIRED)
find_package(CURL REQUIRED)

if(NOT CURL_FOUND)
    message(FATAL_ERROR "libcurl not found. Please install libcurl development packages.")
endif()

# Include directories
include_directories(include)

# Source files
set(SOURCES
    src/extension.cpp
    src/httprequest.cpp
    src/config.cpp
    src/configfile.cpp
    src/utils.cpp
    src/queue.cpp
)

# Create the shared library
add_library(attendance_bot SHARED ${SOURCES})

# Link libraries
target_link_libraries(attendance_bot PRIVATE Threads::Threads ${CURL_LIBRARIES})

# Set output name
set_target_properties(attendance_bot PROPERTIES
    OUTPUT_NAME "attendance_bot"
)

# Enable position-independent code (required for shared libraries on Linux)
set_target_properties(attendance_bot PROPERTIES
    POSITION_INDEPENDENT_CODE ON
)

# Export compile commands for debugging
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
