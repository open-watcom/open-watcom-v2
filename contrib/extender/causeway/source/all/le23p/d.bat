ml -c /DDEBUG2 /Zf /Zd /Font_le23p le23p.asm
wlink @nt_le23p.lnk
pause
le23p nt_le23p
rem cwc /m2 nt_le23p
d:\devel\watcom\wmapcwd moo.map nt_le23p.map
