@echo off
if not exist build (mkdir build)
cd build
cmake ..
cd ..
echo You can now open the .sln in the build directory