!ifndef v
!  define v
!endif

.c.obj:
    wcc$(v)p $[* -oalit -d1 -zq -w4

.obj.exe:
    wlink form dos DEBUG line OPTION quiet NAME $^@ FILE $[@

all : .symbolic testtiny.exe testdir.exe
    @%null

global : .symbolic
    rm -f *.?bj *.ex?
