@echo off
cd /d "C:\Users\manda\Git_projects\Project"
rmdir /s /q build 2>nul
mkdir build
cd build

REM Initialize Visual Studio environment
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvarsall.bat" x64

REM Add CMake to PATH
set PATH=%PATH%;C:\Program Files\CMake\bin

REM Configure and build
cmake .. -G "NMake Makefiles"
cmake --build . --config Release

pause
