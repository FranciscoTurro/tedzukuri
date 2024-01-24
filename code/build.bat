@echo off

mkdir D:\tedzukuri\build
pushd D:\tedzukuri\build
cl -FC -Zi D:\tedzukuri\code\win32_tedzukuri.cpp user32.lib gdi32.lib dsound.lib
popd