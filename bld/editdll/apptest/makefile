NAME            =apptest
MODEL           =l
PROCESSOR       =286
BUILD_TARGET    =windows
system          =windows dll

apptest.exe : apptest.obj
    %write apptest.lnk system windows 
    @%append apptest.lnk debug all
    @%append apptest.lnk option   map, elim, verbose, static
    @%append apptest.lnk library  ..\viw\wini86\wedit.lib
    @%append apptest.lnk option   stack=8k
    @%append apptest.lnk segment  _TEXT PRELOAD FIXED
    @%append apptest.lnk segment  CLASS 'CODE' LOADONCALL MOVEABLE DISCARDABLE
    wlink name apptest @apptest.lnk file apptest.obj

apptest.obj : apptest.c
    wcc apptest.c -i=..;$(%WATCOM)\h\win /d2 /zc /zw
