
wsplice.exe : wsd.exe wsplice.obj
        wlink sys os2 op quiet, newfiles, stub=wsd.exe file wsplice

wsd.exe : wsplice.obj
        wlink d a sys dos file wsplice name wsd.exe option quiet
.c.obj:
        $(comp286) $[* -fo$^@ -d1 -oaxt -w3 -fpc -zq -bt=os2
