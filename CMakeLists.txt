cmake_minimum_required(VERSION 3.15)

project(Arma3AttendanceBot VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include directories
include_directories(include)

# Find required packages
find_package(Threads REQUIRED)
find_package(CURL REQUIRED)

if(NOT CURL_FOUND)
    message(FATAL_ERROR "libcurl not found. Please install libcurl development packages.")
endif()

# Source files
set(SOURCES
    src/extension.cpp
    src/httprequest.cpp
    src/config.cpp
    src/utils.cpp
)

# Create the shared library
# On Linux: libattendance_bot.so
# On Windows: attendance_bot.dll
add_library(attendance_bot SHARED ${SOURCES})

# Link libraries
target_link_libraries(attendance_bot PRIVATE Threads::Threads ${CURL_LIBRARIES})

# Set output name without lib prefix for consistency
set_target_properties(attendance_bot PROPERTIES
    OUTPUT_NAME "attendance_bot"
)

# For Linux, ensure PIC is enabled
set_target_properties(attendance_bot PROPERTIES
    POSITION_INDEPENDENT_CODE ON
)

# Export compile commands for debugging
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
