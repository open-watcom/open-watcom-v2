host_CPU = axp
host_OS=nt

!include cproj.mif

wsplice.exe : wsplice.obj
        wlink sys ntaxp op quiet file wsplice referenc mainCRTStartup option start=mainCRTStartup

.c:..
.c.obj:
        $(cc) $[* -fo$^@ -oaxt -w3 -zq
