set scan=rem
if not [%DROOT%] == [] goto nostart
    call ..\DSTART.BAT g:\t l10new.dat 10-19-94 10:01:00 d:\dev\setupgui -0 -u
:nostart
rem call ..\packfile *
call ..\MAKEDISK.BAT c 1.4 d:\distrib\wc\install
rem xcopy \vpdisk1\*.* d:\distrib\wc\install\diskimgs\vpdisk1\ /s
rem xcopy \vpdisk2\*.* d:\distrib\wc\install\diskimgs\vpdisk2\ /s
rem ren d:\distrib\wc\install\diskimgs\vpdisk1\setup.exe vpsetup.exe
