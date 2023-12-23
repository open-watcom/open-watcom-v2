REM Script to build the Open Watcom tools
REM using the host platform's native C/C++ compiler or OW tools.
REM

mkdir %OWROOT%\build\%OWOBJDIR%

mkdir %OWROOT%\bld\wmake\%OWOBJDIR%
cd %OWROOT%\bld\wmake\%OWOBJDIR%
nmake -f ..\nmake
mkdir %OWROOT%\bld\builder\%OWOBJDIR%
cd %OWROOT%\bld\builder\%OWOBJDIR%
%OWROOT%\build\%OWOBJDIR%\wmake -f ..\binmake bootstrap=1
cd %OWROOT%\bld
builder boot
builder build os_nt cpu_x64
