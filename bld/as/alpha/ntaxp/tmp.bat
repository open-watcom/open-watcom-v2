set include=d:\bld\as\alpha\h;d:\bld\as\h;d:\bld\owl\h;d:\bld\cpp;d:\bld\watcom\h;d:\bld\trmem;;e:\nlang\h;e:\nlang\h\nt;d:\bld\watcom\h
wcl386 -zq -l=nt -ie:\nlang\h d:\bld\as\c\mkstrid.c -zq -w4 -we -dNDEBUG  -bt=nt -oaxt -d1 -fhq -dAS_ALPHA -d_STANDALONE_ -dNDEBUG  
mkstrid msg.gh
