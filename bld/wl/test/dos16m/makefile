# Build DOS/16M test executables.
#
# Note: DOS/4GW refuses to load DOS/16M executables unless they're called
# 'RSIHELP' or 'WSAMPRSI'. The full DOS/4G does not have this restriction.
#

D16INC = -I"../../../d16api/include"

all : atest.exp ctest.exp wsamprsi.exe

atest.exp : atest.asm d16info.inc
    wasm -zq atest.asm
    wlink op quiet, map name $@ form dos16m runtime auto f atest

ctest.exp : ctest.c
    wcl -zq -Fm $(D16INC) -l=dos16m ctest.c

wsamprsi.exe : ctest.c
    wcc -zq $(D16INC) ctest.c
    wlink op quiet, map name $@ sys dos16m f ctest op stub=wstubq.exe

clean : .symbolic
    @rm -f *.obj
    @rm -f *.exp
    @rm -f *.exe
    @rm -f *.map
