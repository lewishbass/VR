@echo off
setlocal enabledelayedexpansion

set SLANG_VERSION=2024.14.4
set SLANG_DIR=%~dp0tools\slang

echo Detecting platform...
if "%PROCESSOR_ARCHITECTURE%"=="AMD64" (
    set PLATFORM=win64
) else if "%PROCESSOR_ARCHITECTURE%"=="x86" (
    set PLATFORM=win32
) else (
    echo Unsupported architecture: %PROCESSOR_ARCHITECTURE%
    exit /b 1
)

set SLANG_URL=https://github.com/shader-slang/slang/releases/download/v%SLANG_VERSION%/slang-%SLANG_VERSION%-windows-x86_64.zip
set SLANG_ZIP=%TEMP%\slang.zip

echo Installing Slang SDK %SLANG_VERSION% for %PLATFORM%...

if exist "%SLANG_DIR%" (
    echo Slang SDK directory already exists. Removing...
    rmdir /s /q "%SLANG_DIR%"
)

mkdir "%SLANG_DIR%"

echo Downloading Slang SDK...
powershell -Command "& {[Net.ServicePointManager]::SecurityProtocol = [Net.SecurityProtocolType]::Tls12; Invoke-WebRequest -Uri '%SLANG_URL%' -OutFile '%SLANG_ZIP%'}"
if %errorlevel% neq 0 (
    echo Failed to download Slang SDK
    exit /b %errorlevel%
)

echo Extracting Slang SDK...
powershell -Command "& {Expand-Archive -Path '%SLANG_ZIP%' -DestinationPath '%SLANG_DIR%' -Force}"
if %errorlevel% neq 0 (
    echo Failed to extract Slang SDK
    del "%SLANG_ZIP%"
    exit /b %errorlevel%
)

del "%SLANG_ZIP%"

echo Slang SDK installed successfully to %SLANG_DIR%
exit /b 0
