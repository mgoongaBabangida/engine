@echo off
set assimp=0

::============================================================
if "%assimp%" equ "1" call :BuildAssimp

echo Done...
goto:eof

::============================================================
:BuildAssimp
cd vendor/assimp
echo Building assimp
call cmake CMakeLists.txt
call cmake --build . --target ALL_BUILD --config Debug
call cmake --build . --target ALL_BUILD --config Release
cd ../..
goto:eof