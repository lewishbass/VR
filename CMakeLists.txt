cmake_minimum_required(VERSION 3.10)

project(VRTestProj)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

message(STATUS "Detecting platform...")

if(CMAKE_SYSTEM_PROCESSOR MATCHES "arm|aarch64" OR CMAKE_HOST_SYSTEM_PROCESSOR MATCHES "arm|aarch64")
    message(STATUS "Configuring for ARM")
    set(PLATFORM_FORK "ARM")
elseif(WIN32)
    message(STATUS "Configuring for Windows")
    set(PLATFORM_FORK "Windows")
elseif(APPLE)
    message(STATUS "Configuring for macOS")
    set(PLATFORM_FORK "macOS")
elseif(UNIX)
    message(STATUS "Configuring for Linux")
    set(PLATFORM_FORK "Linux")
else()
    message(STATUS "Configuring for an unknown platform")
    set(PLATFORM_FORK "Unknown")
endif()

# Add your source files here
add_executable(VRTestProj main.cpp)

add_custom_target(BuildMessage
    COMMAND ${CMAKE_COMMAND} -E echo "Starting build for VRTestProj on platform: ${PLATFORM_FORK} Config: $<CONFIG>"
    COMMENT "Build started"
)

add_dependencies(VRTestProj BuildMessage)