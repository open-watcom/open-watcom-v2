ALL     : cw32.exe

cw32.exe        : cw32.asm raw_vcpi.asm interrup.asm ldt.asm memory.asm api.asm \
                int10h.asm int21h.asm int33h.asm decode_c.asm exceptn.asm \
                strucs.inc cw.inc loadle\loadle.asm
 ml /c /DENGLISH=1 /Fmcw32.map /Flcw32.lst cw32.asm
 link cw32;
 del cw32.obj
 cwc /m2 /l85 cw32.exe
