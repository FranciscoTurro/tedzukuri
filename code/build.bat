@echo off

mkdir ..\build
pushd ..\build
cl -Zi ..\code\win32_tedzukuri.cpp user32.lib
popd