.c: ..
.cpp: ..
.c.obj:
    wcc386 $[* -i=$(%INCLUDE);.. -d2 -mf -oails -s -w4 -zq -dTRMEM -of+
.cpp.obj:
    wpp386 $[* -i=$(%INCLUDE);.. -d2 -mf -oails -s -w4 -zq -dTRMEM -of+

default: .SYMBOLIC trmemcvr.obj trmem.obj mem.obj
    @@%null
