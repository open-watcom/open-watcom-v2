cflags = -bt=DOS -mf-fpc-oxt-zq-w4 -i=$(uidir)\h;$(trmem_dir) -fo=$[& /of+

!include ..\main.mif

bindit : .PROCEDURE
        $(pharlap)bind386 $(pharlap)run386b.exe $^*.exp -exe $^@

linkit : .PROCEDURE
        @%append $(lnk) system pharlap
        @%append $(lnk) runtime maxreal=640k
        @%append $(lnk) op maxdata=1024K
        @%append $(lnk) OP OFFSET=64K
        @%append $(lnk) NAME $(name).exp
        @%append $(lnk) lib $(uidir)\dos\uilibf.lib
