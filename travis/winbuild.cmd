@set OWECHO=off
@if "%OWTRAVIS_DEBUG%" == "1" set OWECHO=on
@echo %OWECHO%
SETLOCAL EnableExtensions
REM Script to build the Open Watcom bootstrap tools
REM By Microsoft Visual Studio
REM ...
set OWROOT=%CD%
REM ...
call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" amd64
REM ...
@echo %OWECHO%
REM ...
call cmnvars.bat
REM ...
@echo %OWECHO%
REM
REM setup DOSBOX
REM
set OWDOSBOX=%OWROOT%\travis\dosbox\dosbox.exe
set SDL_VIDEODRIVER=dummy
dir %OWROOT%\travis\dosbox
REM ...
if "%OWTRAVIS_ENV_DEBUG%" == "1" (
    set
) else (
    if "%OWTRAVIS_DEBUG%" == "1" (
        echo INCLUDE="%INCLUDE%"
        echo LIB="%LIB%"
        echo LIBPATH="%LIBPATH%"
    )
)
REM ...
%OWDOSBOX% -noautoexec -noconsole -c "DIR" -c "exit"
cd %OWSRCDIR%
if "%OWTRAVISJOB%" == "BOOTSTRAP" (
    cd wmake
    mkdir %OWOBJDIR%
    cd %OWOBJDIR%
    nmake -f ..\nmake clean >>%OWBINDIR%\bootx.log 2>&1
    nmake -f ..\nmake >>%OWBINDIR%\bootx.log 2>&1
    if not errorlevel == 1 (
	cd %OWSRCDIR%\builder
	mkdir %OWOBJDIR%
	cd %OWOBJDIR%
	%OWBINDIR%\wmake -f ..\binmake clean >>%OWBINDIR%\bootx.log 2>&1
	%OWBINDIR%\wmake -f ..\binmake bootstrap=1 builder.exe >>%OWBINDIR%\bootx.log 2>&1
	if not errorlevel == 1 (
	    cd %OWSRCDIR%
	    if "%TRAVIS_EVENT_TYPE%" == "pull_request" (
		builder boot
	    ) else (
		builder -q boot
	    )
	)
    )
)
if "%OWTRAVISJOB%" == "BUILD" (
    if "%TRAVIS_EVENT_TYPE%" == "pull_request" (
        builder rel
    ) else (
        builder -q rel
    )
)
if "%OWTRAVISJOB%" == "BUILD-1" (
    if "%TRAVIS_EVENT_TYPE%" == "pull_request" (
        builder rel1
    ) else (
        builder -q rel1
    )
)
if "%OWTRAVISJOB%" == "BUILD-2" (
    if "%TRAVIS_EVENT_TYPE%" == "pull_request" (
        builder rel2
    ) else (
        builder -q rel2
    )
)
if "%OWTRAVISJOB%" == "BUILD-3" (
    if "%TRAVIS_EVENT_TYPE%" == "pull_request" (
        builder rel3
    ) else (
        builder -q rel3
    )
)
if "%OWTRAVISJOB%" == "DOCS" (
    cd %OWDOCSDIR%
    if "%TRAVIS_EVENT_TYPE%" == "pull_request" (
        builder docs
    ) else (
        builder -q docs
    )
)
if "%OWTRAVISJOB%" == "INST" (
    cd %OWDISTRDIR%
    if "%TRAVIS_EVENT_TYPE%" == "pull_request" (
        builder build os_nt cpu_x64
    ) else (
        builder -q build os_nt cpu_x64
    )
)
set RC=%ERRORLEVEL%
REM sleep 3
ping -n 3 127.0.0.1 >NUL
exit %RC%
