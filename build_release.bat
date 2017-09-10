@echo off
  
::git pull

if not exist Release (mkdir Release)

cd Release

if exist CMakeCache.txt (del CMakeCache.txt)

call "D:/Program Files (x86)/Microsoft Visual Studio/2017/Community/Common7/Tools/VsDevCmd.bat"

cmake -G "Visual Studio 15" -D CMAKE_BUILD_TYPE=Release ../src

msbuild FXSudokuDecoder.sln /m

cd ..

@echo on