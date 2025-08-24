@echo off

pushd ..
vendor\Walnut\vendor\bin\premake\Windows\premake5.exe --file=Build-Luxim-Project.lua vs2022
popd
pause