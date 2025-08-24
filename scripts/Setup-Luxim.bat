@echo off
cd ..\vendor\libtinyfiledialogs
mkdir build
cd build
cmake ..\

cd ..\..\..\scripts
Setup-ProjectFiles.bat

pause
