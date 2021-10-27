echo on
if "%OWTOOLSV%" == "VS2017" call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" amd64
if "%OWTOOLSV%" == "VS2019" call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Enterprise\VC\Auxiliary\Build\vcvarsall.bat" amd64
if "%OWTOOLSV%" == "VS2022" call "C:\Program Files\Microsoft Visual Studio\2022\Preview\VC\Auxiliary\Build\vcvarsall.bat" amd64

echo on
set OWDOSBOXPATH=%OWCIBIN32%
set OWDOSBOX=dosbox.exe

call %OWROOT%\cmnvars.bat

echo on
%OWCOVERITY_TOOL_CMD% --dir cov-int %OWCOVERITY_SCRIPT%
