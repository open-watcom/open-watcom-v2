
all : batserv.exe batcli.obj .symbolic

batserv.exe : batserv.obj
        wlink sys os2v2 file batserv op stack=16k

batserv.obj : batserv.c
        wcc386 batserv.c -oaxt -d1 -w3 -i$(watcomdir)\os220_h

batcli.obj : batcli.c
        wcc batcli.c -oaxt -d1 -w3

tst.exe : tst.c batcli.obj
        wcl /d2 tst.c batcli.obj /l=dos
