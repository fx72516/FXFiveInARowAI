@echo off
  
::git pull

if not exist Debug (mkdir Debug)

cd Debug

if exist CMakeCache.txt (del CMakeCache.txt)

call "D:/Program Files (x86)/Microsoft Visual Studio/2017/Community/Common7/Tools/VsDevCmd.bat"

cmake -G "Visual Studio 15" -DCMAKE_BUILD_TYPE=Debug ../src

msbuild FXFiveInARowAI.sln /m

cd ..

@echo on