@echo off
setlocal
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"

set PROJECT_ROOT=%~dp0
set PROJECT_ROOT=%PROJECT_ROOT:~0,-1%

if not exist build mkdir build
if not exist lib mkdir lib

del /Q %PROJECT_ROOT%\build\*.*
del /Q %PROJECT_ROOT%\lib\*.*

set MODE=%1
if "%MODE%"=="" set MODE=read

if /I "%MODE%"=="read" (
    echo [BUILDING READ/WRITE TESTER]
    set "source_files=%PROJECT_ROOT%\src\repetition_test_build.c"
    set "exe_name=repeater.exe"
) else if /I "%MODE%"=="branch" (
    echo [BUILDING BRANCH PREDICTOR TESTER]
    set "source_files=%PROJECT_ROOT%\src\branch_test_build.c"
    set "exe_name=brancher.exe"
) else if /I "%MODE%"=="align" (
    echo [BUILDING ALIGNMENT TESTER]
    set "source_files=%PROJECT_ROOT%\src\alignment_test_build.c"
    set "exe_name=alignment.exe"
) else (
    echo Unknown mode: %MODE%
    echo Usage: rep.bat [read^|branch]
    exit /b 1
)

nasm -f win64 -o %PROJECT_ROOT%\build\nop_loop.obj src\nop_loop.asm
lib /nologo /OUT:"%PROJECT_ROOT%\lib\nop_loop.lib" "%PROJECT_ROOT%\build\nop_loop.obj"

set "includes=/I"%PROJECT_ROOT%\src""

set "compiler_flags=/nologo /O1 /Zi /Fe:%exe_name% /D_CRT_SECURE_NO_WARNINGS"
set "linker_flags=/link /DEBUG /LIBPATH:"%PROJECT_ROOT%\lib" nop_loop.lib bcrypt.lib"

pushd build
cl %includes% %compiler_flags% %source_files% %linker_flags%
popd

if %ERRORLEVEL% EQU 0 (
    echo [BUILD SUCCESS]
    start raddbg --ipc select_target %PROJECT_ROOT%\build\%exe_name%
)
