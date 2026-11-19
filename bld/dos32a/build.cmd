@echo off
setlocal enabledelayedexpansion
rem Hermetic Windows .cmd build script for MS-DOS builds of DOS/32A v9.1.2

set "SCRIPT_DIR=%~dp0"
for %%I in ("%SCRIPT_DIR%\..\..") do set "OW_ROOT=%%~fI"
set "D32_ROOT=%OW_ROOT%\contrib\extender\dos32a"
set "D32_SRC=%D32_ROOT%\src"
if "%~1"=="" (
    set "OUT_DIR=%SCRIPT_DIR%build"
) else (
    set "OUT_DIR=%~f1"
)

if not defined WATCOM (
    if exist "%OW_ROOT%/rel/h" set "WATCOM=%OW_ROOT%/rel"
)
if not defined WATCOM (
    echo ERROR: WATCOM environment variable is not set. 1>&2
    exit /b 1
)

if exist "%WATCOM%\binnt64\wasm.exe" (
    set "OW_BIN=%WATCOM%\binnt64"
) else (
    set "OW_BIN=%WATCOM%\binnt"
)
set "PATH=%OW_BIN%;%PATH%"
set "INCLUDE=%WATCOM%\h;%D32_ROOT%\h32;%D32_SRC%\sutils\misc"

set "OBJ_DIR=%OUT_DIR%\obj"
set "BIN_DIR=%OUT_DIR%\binw"
if not exist "%OBJ_DIR%" mkdir "%OBJ_DIR%"
if not exist "%BIN_DIR%" mkdir "%BIN_DIR%"

wcl386 -zq -fr= -ox "%D32_SRC%\sutils\mzalign\mzalign.c" -fe="%OBJ_DIR%\mzalign.exe" || exit /b 1

pushd "%D32_SRC%\dos32a"
wasm -bt=dos -fpi87 -wx -we -zld -zq -dEXEC_TYPE=0 -dDOS_LEGACY_125_20=1 -dDOS32A_BOUND_STUB=0 -i="..\sutils\misc" -i="." -i="text" -fo="%OBJ_DIR%\kernel.obj" kernel.asm || exit /b 1
wasm -bt=dos -fpi87 -wx -we -zld -zq -dEXEC_TYPE=0 -dDOS_LEGACY_125_20=1 -dDOS32A_BOUND_STUB=0 -i="..\sutils\misc" -i="." -i="text" -fo="%OBJ_DIR%\dos32a.obj" dos32a.asm || exit /b 1
wasm -bt=dos -fpi87 -wx -we -zld -zq -dEXEC_TYPE=0 -dDOS_LEGACY_125_20=1 -dDOS32A_BOUND_STUB=1 -i="..\sutils\misc" -i="." -i="text" -fo="%OBJ_DIR%\dos32a_stb.obj" dos32a.asm || exit /b 1
wasm -bt=dos -fpi87 -wx -we -zld -zq -dEXEC_TYPE=0 -dDOS_LEGACY_125_20=0 -dDOS32A_BOUND_STUB=0 -i="..\sutils\misc" -i="." -i="text" -fo="%OBJ_DIR%\kernel_stock.obj" kernel.asm || exit /b 1
wasm -bt=dos -fpi87 -wx -we -zld -zq -dEXEC_TYPE=0 -dDOS_LEGACY_125_20=0 -dDOS32A_BOUND_STUB=0 -i="..\sutils\misc" -i="." -i="text" -fo="%OBJ_DIR%\dos32a_stock.obj" dos32a.asm || exit /b 1
popd

wlink system dos option quiet, fullheader, packcode=0, packdata=0 name "%BIN_DIR%\dos32a.exe" file "%OBJ_DIR%\dos32a.obj", "%OBJ_DIR%\kernel.obj" || exit /b 1
"%OBJ_DIR%\mzalign.exe" "%BIN_DIR%\dos32a.exe" "%BIN_DIR%\dos32a.exe" --pad-relocs 0x007E || exit /b 1

wlink system dos option quiet, fullheader, packcode=0, packdata=0 name "%BIN_DIR%\dos32a.stb" file "%OBJ_DIR%\dos32a_stb.obj", "%OBJ_DIR%\kernel.obj" || exit /b 1
"%OBJ_DIR%\mzalign.exe" "%BIN_DIR%\dos32a.stb" "%BIN_DIR%\dos32a.stb" --pad-relocs 0x007E || exit /b 1

wlink system dos option quiet, fullheader, packcode=0, packdata=0 name "%BIN_DIR%\dos32a_stock.exe" file "%OBJ_DIR%\dos32a_stock.obj", "%OBJ_DIR%\kernel_stock.obj" || exit /b 1
"%OBJ_DIR%\mzalign.exe" "%BIN_DIR%\dos32a_stock.exe" "%BIN_DIR%\dos32a_stock.exe" --pad-relocs 0x007E || exit /b 1

pushd "%D32_SRC%\stub32a"
wasm -bt=dos -wx -we -zld -zq -dEXEC_TYPE=0 -i="..\sutils\misc" -fo="%OBJ_DIR%\stub32a.obj" stub32a.asm || exit /b 1
wasm -bt=dos -wx -we -zld -zq -dEXEC_TYPE=0 -i="..\sutils\misc" -fo="%OBJ_DIR%\stub32c.obj" stub32c.asm || exit /b 1
popd
wlink system dos option quiet, fullheader name "%BIN_DIR%\stub32a.exe" file "%OBJ_DIR%\stub32a.obj" || exit /b 1
wlink system dos option quiet, fullheader name "%BIN_DIR%\stub32c.exe" file "%OBJ_DIR%\stub32c.obj" || exit /b 1

set "WCL32_FLAGS=-zq -fr= -oneatx -ohirbk -ol+ -ei -zp16 -6 -fp6 -fpi87 -bt=dos"

pushd "%D32_SRC%\sb"
wasm -bt=dos -3p -mf -wx -we -zld -zq -i="..\sutils\misc" -fo="%OBJ_DIR%\sbind.obj" sbind.asm || exit /b 1
wcl386 %WCL32_FLAGS% -c -fo="%OBJ_DIR%\sb_main.obj" main.c || exit /b 1
popd
wlink system dos32a option quiet, stub="%BIN_DIR%\dos32a.stb", stack=65536 name "%BIN_DIR%\sb.exe" file "%OBJ_DIR%\sbind.obj", "%OBJ_DIR%\sb_main.obj" || exit /b 1

pushd "%D32_SRC%\ss"
wasm -bt=dos -3p -mf -wx -we -zld -zq -i="..\sutils\misc" -fo="%OBJ_DIR%\setup.obj" setup.asm || exit /b 1
wcl386 %WCL32_FLAGS% -c -fo="%OBJ_DIR%\ss_main.obj" main.c || exit /b 1
popd
wlink system dos32a option quiet, stub="%BIN_DIR%\dos32a.stb", stack=65536 name "%BIN_DIR%\ss.exe" file "%OBJ_DIR%\setup.obj", "%OBJ_DIR%\ss_main.obj" || exit /b 1

pushd "%D32_SRC%\sc"
wasm -bt=dos -3p -mf -wx -we -zld -zq -i="..\sutils\misc" -fo="%OBJ_DIR%\scomp.obj" scomp.asm || exit /b 1
wasm -bt=dos -3p -mf -wx -we -zld -zq -i="..\sutils\misc" -fo="%OBJ_DIR%\sload.obj" sload.asm || exit /b 1
wcl386 %WCL32_FLAGS% -c -fo="%OBJ_DIR%\sc_encode.obj" encode.c || exit /b 1
wcl386 %WCL32_FLAGS% -c -fo="%OBJ_DIR%\sc_main.obj" main.c || exit /b 1
popd
wlink system dos32a option quiet, stub="%BIN_DIR%\dos32a.stb", stack=65536 name "%BIN_DIR%\sc.exe" file "%OBJ_DIR%\scomp.obj", "%OBJ_DIR%\sload.obj", "%OBJ_DIR%\sc_encode.obj", "%OBJ_DIR%\sc_main.obj" || exit /b 1

pushd "%D32_SRC%\sd"
wasm -bt=dos -4p -mf -fpi87 -wx -we -zld -zq -dSVR=0 -i="..\sutils\misc" -fo="%OBJ_DIR%\sd.obj" sd.asm || exit /b 1
popd
wlink system dos32a option quiet, stub="%BIN_DIR%\dos32a.stb" name "%BIN_DIR%\sd.exe" file "%OBJ_DIR%\sd.obj" || exit /b 1

pushd "%D32_SRC%\sutils\sver"
wcl -zq -fr= -oneatx -bt=dos -ml -c -fo="%OBJ_DIR%\sver.obj" main.c || exit /b 1
popd
wlink system dos option quiet, fullheader name "%BIN_DIR%\sver.exe" file "%OBJ_DIR%\sver.obj" || exit /b 1
"%OBJ_DIR%\mzalign.exe" "%BIN_DIR%\sver.exe" "%BIN_DIR%\sver.exe" || exit /b 1

echo ==^> [DOS/32A] Built MS-DOS artifacts in %BIN_DIR%
exit /b 0
