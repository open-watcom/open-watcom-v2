all: p16fpc.obj p16fpi.obj .SYMBOLIC
    @%null
p16fpc.obj: pcodeint.asm p16codes.inc _bitmap_.inc
    masm /mx/s /d__DEBUG__ /d__FP__ /d__FPC__ pcodeint,p16fpcd;
    masm /mx/s /d__FP__ /d__FPC__ pcodeint,p16fpc;
p16fpi.obj: pcodeint.asm p16codes.inc _bitmap_.inc
    masm /mx/s/e /d__DEBUG__ /d__FP__ pcodeint,p16fpid;
    masm /mx/s/e /d__FP__ pcodeint,p16fpi;
