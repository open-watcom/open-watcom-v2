batserv.exe : batserv.obj batpipe.obj
        wlink $(%ldebug) op symfile sys nt file batserv,batpipe op stack=32K

all : batserv.exe batcli.obj batpipe.obj tst.exe .symbolic

.c.obj:
        wcc386 $[* -oaxt $(%debug) $(%pch) -w3 -bt=nt -i$(watcomdir)\nt_h

tst.exe : tst.obj batcli.obj batpipe.obj
        wlink sys nt debug all file tst,batcli,batpipe

tst.obj : ..\os2\tst.c
        wcc386 $[* -oaxt $(%debug) -w3 -bt=nt -i$(watcomdir)\nt_h

debug : .symbolic
        set debug=-d2
        set pch=-fh
        set ldebug=debug all

.before :
        set debug=-d1
        set pch=
        set ldebug=
