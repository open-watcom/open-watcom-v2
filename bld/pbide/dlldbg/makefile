dlldbg.exe : dlldbg.obj dlldbg.res
        wlink op map debug all sys windows file dlldbg
        wrc -k dlldbg.res

dlldbg.obj : dlldbg.c
        wcc /bt=windows /w4 /d2 /zW /ml dlldbg

dlldbg.res : dlldbg.rc debugger.ico
        wrc -bt=windows -r dlldbg.rc

