echo on
if "%OWIMAGE%" == "vs2017-win2016" call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" amd64
if "%OWIMAGE%" == "windows-2019" call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Enterprise\VC\Auxiliary\Build\vcvarsall.bat" amd64
if "%OWIMAGE%" == "windows-2022" call "C:\Program Files\Microsoft Visual Studio\2022\Enterprise\VC\Auxiliary\Build\vcvarsall.bat" amd64

echo on

call %OWROOT%\cmnvars.bat

echo on
%OWCOVERITY_TOOL_CMD% --dir cov-int %OWCOVERITY_SCRIPT%
