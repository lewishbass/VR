


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

