.c: ..
.cpp: ..
.c.obj:
    wcc386 $[* -i=$(%include);.. -d2 -mf -oails -s -w4 -zq -dTRMEM -of+
.cpp.obj:
    wpp386 $[* -i=$(%include);.. -d2 -mf -oails -s -w4 -zq -dTRACKER -of+

default: .symbolic trmemcvr.obj trmem.obj mem.obj
    @@%null
