NAME            =apptest
MODEL           =l
PROCESSOR       =286
BUILD_TARGET    =windows
system          =windows dll

apptest.exe : apptest.obj
    wlink @apptest.lnk

apptest.obj : apptest.c
    wcc apptest.c -i=..;$(%watcom)\h\win /d2 /zc /zw
