@echo off
windres resource.rc -O coff -o resource.res
g++ main.cpp grabber.cpp resource.res -o "NitroGen_by_Rubilacxe.exe" ^
    -mwindows -std=c++17 -O2 -static -lwinhttp -lcomctl32 -lshell32
echo Done. -> NitroGen_by_Rubilacxe.exe
