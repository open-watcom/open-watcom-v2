set lib=c:\watcom\lib386;c:\watcom\lib386\dos
ml -c /Zf /Zd /DDEBUG2 /Font_le23p le23p.asm >err.log
d:\linker\wl32\wl32 /m/ds/cs/f/i nt_le23p c2asm,nt_le23p,,system2 >>err.log
rem le23p nt_le23p
rem cwc /m2 nt_le23p
