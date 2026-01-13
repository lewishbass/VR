cmake_minimum_required(VERSION 3.28.3)

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

# Find and link SDL2
find_package(SDL2 REQUIRED)
# Find and link Vulkan
find_package(Vulkan REQUIRED)
# Find and link OpenXR
find_package(OpenXR REQUIRED)
# Find and link TinyOBJLoader
find_package(tinyobjloader REQUIRED)
# Find OpenGL
find_package(OpenGL)

# find metal
if(APPLE)
    find_library(METAL_LIBRARY Metal)
    if(METAL_LIBRARY)
        message(STATUS "Metal framework found: ${METAL_LIBRARY}")
        else()
        message(WARNING "Metal framework not found!")
    endif()
endif()


# Automatically gather all source files from src directory
file(GLOB_RECURSE SOURCE_FILES 
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
)

# Add your source files here
add_executable(VRTestProj main.cpp ${SOURCE_FILES})

# Include src directory for headers
target_include_directories(VRTestProj PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")

if (Vulkan_FOUND)
    target_link_libraries(VRTestProj Vulkan::Vulkan)
    message(STATUS "Vulkan found and linked")
    target_compile_definitions(VRTestProj PRIVATE VULKAN_LINKED=true)
else()
    message(WARNING "Vulkan not found! Vulkan support will be disabled.")
    target_compile_definitions(VRTestProj PRIVATE VULKAN_LINKED=false)
endif()

if (OpenGL_FOUND)
    target_link_libraries(VRTestProj OpenGL::GL)
    message(STATUS "OpenGL found and linked")
    target_compile_definitions(VRTestProj PRIVATE OPENGL_LINKED=true)
else()
    message(WARNING "OpenGL not found! OpenGL support will be disabled.")
    target_compile_definitions(VRTestProj PRIVATE OPENGL_LINKED=false)
endif()

if(APPLE AND METAL_LIBRARY)
    target_link_libraries(VRTestProj "${METAL_LIBRARY}")
    message(STATUS "Metal linked")
    target_compile_definitions(VRTestProj PRIVATE METAL_LINKED=true)
else()
    message(STATUS "Metal framework not found! Metal support will be disabled.")
    target_compile_definitions(VRTestProj PRIVATE METAL_LINKED=false)
endif()


################ Shader Compilation ################

# Slang shader compilation setup
find_program(SLANGC_EXECUTABLE 
    NAMES slangc slangc.exe
    HINTS 
        $ENV{SLANG_DIR}/bin
        $ENV{SLANG_DIR}/Bin
        ${CMAKE_CURRENT_SOURCE_DIR}/tools/slang/bin
        /usr/bin
        /usr/local/bin
)

if(SLANGC_EXECUTABLE)
    message(STATUS "Found slangc: ${SLANGC_EXECUTABLE}")
    
    # Define shader directory
    set(SHADER_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/shaders")
    set(SHADER_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/shaders")
    
    # Create output directory
    file(MAKE_DIRECTORY ${SHADER_BINARY_DIR})
    
    # Find all Slang shader files
    file(GLOB_RECURSE SHADER_SOURCES
        "${SHADER_SOURCE_DIR}/*.slang"
    )
    
    if(SHADER_SOURCES)
        # Determine target based on available graphics APIs
        set(SLANG_TARGETS "")
        if(Vulkan_FOUND)
            list(APPEND SLANG_TARGETS "spirv")
        endif()
        if(OpenGL_FOUND)
            list(APPEND SLANG_TARGETS "glsl")
        endif()
        if(APPLE AND METAL_LIBRARY)
            list(APPEND SLANG_TARGETS "metal")
        endif()
        
        if(NOT SLANG_TARGETS)
            message(WARNING "No graphics API found for Slang compilation")
        else()
            message(STATUS "Slang targets: ${SLANG_TARGETS}")
        endif()
        
        # Compile each shader for each target
        foreach(SHADER ${SHADER_SOURCES})
            get_filename_component(SHADER_NAME ${SHADER} NAME_WE)
            get_filename_component(SHADER_DIR ${SHADER} DIRECTORY)
            string(REPLACE "${SHADER_SOURCE_DIR}" "" SHADER_REL_DIR ${SHADER_DIR})
            set(SHADER_OUTPUT_DIR "${SHADER_BINARY_DIR}${SHADER_REL_DIR}")
            
            # Entry points for vertex, fragment, compute, and geometry shaders
            set(ENTRY_POINTS "vertex;fragment;compute;geometry")
            
            # Platform-specific null device for suppressing errors
            if(WIN32)
                set(NULL_DEVICE "NUL")
            else()
                set(NULL_DEVICE "/dev/null")
            endif()
            
            foreach(SLANG_TARGET ${SLANG_TARGETS})
                foreach(ENTRY_POINT ${ENTRY_POINTS})
                    if(SLANG_TARGET STREQUAL "spirv")
                        set(SHADER_OUTPUT "${SHADER_OUTPUT_DIR}/${SHADER_NAME}.${ENTRY_POINT}.spv")
                        set(TARGET_FLAG -target spirv)
                    elseif(SLANG_TARGET STREQUAL "glsl")
                        set(SHADER_OUTPUT "${SHADER_OUTPUT_DIR}/${SHADER_NAME}.${ENTRY_POINT}.glsl")
                        set(TARGET_FLAG -target glsl)
                    elseif(SLANG_TARGET STREQUAL "metal")
                        set(SHADER_OUTPUT "${SHADER_OUTPUT_DIR}/${SHADER_NAME}.${ENTRY_POINT}.metal")
                        set(TARGET_FLAG -target metal)
                    endif()
                    
                    add_custom_command(
                        OUTPUT ${SHADER_OUTPUT}
                        COMMAND ${CMAKE_COMMAND} -E make_directory ${SHADER_OUTPUT_DIR}
                        COMMAND ${SLANGC_EXECUTABLE} ${TARGET_FLAG} -entry ${ENTRY_POINT} ${SHADER} -o ${SHADER_OUTPUT} 2> ${NULL_DEVICE} || ${CMAKE_COMMAND} -E true
                        DEPENDS ${SHADER}
                        COMMENT "Compiling shader: ${SHADER_REL_DIR}/${SHADER_NAME}:${ENTRY_POINT} for ${SLANG_TARGET}"
                        VERBATIM
                    )
                    
                    list(APPEND COMPILED_SHADERS ${SHADER_OUTPUT})
                endforeach()
            endforeach()
        endforeach()
        
        # Create custom target for shader compilation
        add_custom_target(CompileShaders ALL DEPENDS ${COMPILED_SHADERS})
        add_dependencies(VRTestProj CompileShaders)
        
        list(LENGTH SHADER_SOURCES SHADER_COUNT)
        message(STATUS "Configured compilation for ${SHADER_COUNT} Slang shader(s)")
    else()
        message(STATUS "No Slang shaders found in ${SHADER_SOURCE_DIR}")
    endif()
else()
    message(WARNING "slangc not found! Shader compilation disabled. Install Slang SDK from https://shader-slang.com/")
endif()
#####################################################


# set verbosity for CLI logging
target_compile_definitions(VRTestProj PRIVATE CLI_LOGGING=3)

# directory shaders are compiled to
target_compile_definitions(VRTestProj PRIVATE SHADER_BINARY_DIR="${SHADER_BINARY_DIR}")



# Link the libraries
target_link_libraries(VRTestProj SDL2::SDL2 OpenXR::openxr_loader tinyobjloader::tinyobjloader)


# Fix for MinGW console app on Windows
if(MINGW)
    target_link_libraries(VRTestProj SDL2::SDL2main)
    target_link_options(VRTestProj PRIVATE -mconsole)
endif()

add_custom_target(BuildMessage
    COMMAND ${CMAKE_COMMAND} -E echo "Starting build for VRTestProj on platform: ${PLATFORM_FORK} Config: $<CONFIG>"
    COMMENT "Build started"
)

add_dependencies(VRTestProj BuildMessage)