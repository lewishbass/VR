


# Multi-platform building / debugging

## Build file works for
 - Linux
 - Windows
 - ARM
## Future Builds
 - Quest
 - Android

## Github actions
 - build across several platforms
 - execute unit tests
 - save all logs / results / performance stats
 - deploy to steam

## Libs
  - vulkan
  - SDL2
  - OpenXR

## Install Steps
- pull repository
 ``` bash
 git clone https://github.com/lewishbass/VR.git
 ```

 - install vcpkg

 - Windows
 ``` bash
 git clone https://github.com/Microsoft/vcpkg.git
 cd vcpkg
 bootstrap-vcpkg.bat
 ```

 - Linux
 ``` bash
 git clone https://github.com/Microsoft/vcpkg.git
 cd vcpkg
 ./bootstrap-vcpkg.sh
 ```

## Build Steps

- Windows
``` bash
vcpkg\vcpkg install
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE="./vcpkg/scripts/buildsystems/vcpkg.cmake"
cmake --build build --config Release
```

## OpenXR integration structure

### Setup
  - Instance (xrCreateInstance) underlying hardware
  - System   (xrGetSystem)      physical hardware
  - Graphics (getGraphicsRequirement) render engine
             (getInstanceExtensions) 
             (vkCreateInstance)
  - Session

  - Reference Spaces
  - Action Set

### Render Loop
  - Poll Events
  - Poll Actions

  - Begin Frame
  - Acquire Swapchain Images
  - Render Views
  - End Frame

foveated rendering can take any sized maps so less time can be spent on specific objects