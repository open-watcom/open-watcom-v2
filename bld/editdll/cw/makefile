all : .SYMBOLIC weditcw.dll weditcwn.dll

# windows version
weditcw.dll : weditcw.obj
    wlink @weditcw.lnk
    wlib -b -n weditcw.lib +weditcw.dll

weditcw.obj : weditcw.c
    wcc weditcw.c /i=..;$(%watcom)\h\win /mc /d1 /oaxt /zc /zw /zu /zq

# NT version
weditcwn.dll : weditcwn.obj
    wlink debug all @weditcwn.lnk
    wlib -b -n weditcwn.lib +weditcwn.dll

weditcwn.obj : weditcw.c
    wcc386 weditcw.c /i=..;$(%watcom)\h\nt -bt=nt /d1 /oaxt -fo=weditcwn.obj
