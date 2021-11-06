REM Script to build the Open Watcom tools
REM using the host platform's native C/C++ compiler or OW tools.
REM

mkdir %OWBINDIR%\%OWOBJDIR%

mkdir %OWSRCDIR%\wmake\%OWOBJDIR%
cd %OWSRCDIR%\wmake\%OWOBJDIR%
nmake -f ..\nmake
mkdir %OWSRCDIR%\builder\%OWOBJDIR%
cd %OWSRCDIR%\builder\%OWOBJDIR%
%OWBINDIR%\%OWOBJDIR%\wmake -f ..\binmake bootstrap=1
cd %OWSRCDIR%
builder boot
builder build os_nt cpu_x64
