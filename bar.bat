@echo off
setlocal enabledelayedexpansion
set INSTALL_FLAG=OFF
set BUILD_ONLY=OFF
if "%1"=="--install" set INSTALL_FLAG=ON
if "%1"=="-i" set INSTALL_FLAG=ON

if "%1"=="--build-only" set BUILD_ONLY=ON
if "%1"=="-b" set BUILD_ONLY=ON

if "%1"=="--clear" if exist build rmdir /s /q build && set INSTALL_FLAG=ON
if "%1"=="-c" if exist build rmdir /s /q build && set INSTALL_FLAG=ON


REM Set SLANG_DIR for CMake to find slangc
set SLANG_DIR=%~dp0tools\slang

cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE="./vcpkg/scripts/buildsystems/vcpkg.cmake" -DVCPKG_MANIFEST_INSTALL=%INSTALL_FLAG%
if %errorlevel% neq 0 exit /b %errorlevel%
cmake --build build --config Release
if %errorlevel% neq 0 exit /b %errorlevel%

if "%BUILD_ONLY%"=="ON" exit /b 0
cls
build\Release\VRTestProj.exe