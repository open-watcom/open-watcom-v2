if not [%DROOT%] == [] goto nostart
    call ..\DSTART.BAT h:\rel2 lang10.dat 10-19-94 10:01:00 -0
:nostart
call ..\MAKEDISK.BAT c 1.4 x:\installs\cbuild\wc10.5
