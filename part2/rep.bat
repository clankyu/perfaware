@echo off
setlocal
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"

set PROJECT_ROOT=%~dp0
set PROJECT_ROOT=%PROJECT_ROOT:~0,-1%

if not exist build mkdir build
if not exist lib mkdir lib

nasm -f win64 -o %PROJECT_ROOT%\build\nop_loop.obj src\nop_loop.asm
lib /nologo /OUT:"%PROJECT_ROOT%\lib\nop_loop.lib" "%PROJECT_ROOT%\build\nop_loop.obj"

set "source_files=%PROJECT_ROOT%\src\repetition_test_build.c"
set "includes=/I"%PROJECT_ROOT%\src""

set "compiler_flags=/nologo /O1 /Zi /Fe:repeater.exe /D_CRT_SECURE_NO_WARNINGS"
set "linker_flags=/link /DEBUG /LIBPATH:"%PROJECT_ROOT%\lib" nop_loop.lib"

pushd build
cl %includes% %compiler_flags% %source_files% %linker_flags%
popd

if %ERRORLEVEL% EQU 0 (
    echo [BUILD SUCCESS]
    start raddbg --ipc select_target %PROJECT_ROOT%\build\repeater.exe
)
