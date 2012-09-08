# os2api Builder Control file
# ==========================

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

cdsay .

[ BLOCK <1> build rel2 ]
#=======================
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h

[ BLOCK <1> rel2 ]
#=================
    cdsay <PROJDIR>

[ BLOCK <1> rel2 cprel2 acprel2 ]
#================================
#16-bit stuff
    <CCCMD> os2286/h/*.h          <OWRELROOT>/h/os21x/
    <CCCMD> os2286/lib/*.lib      <OWRELROOT>/lib286/os2/
#32-bit stuff
    <CCCMD> os2386/h/*.h          <OWRELROOT>/h/os2/
    <CCCMD> os2386/h/arpa/*.h     <OWRELROOT>/h/os2/arpa/
    <CCCMD> os2386/h/machine/*.h  <OWRELROOT>/h/os2/machine/
    <CCCMD> os2386/h/net/*.h      <OWRELROOT>/h/os2/net/
    <CCCMD> os2386/h/netinet/*.h  <OWRELROOT>/h/os2/netinet/
    <CCCMD> os2386/h/protocol/*.h <OWRELROOT>/h/os2/protocol/
    <CCCMD> os2386/h/sys/*.h      <OWRELROOT>/h/os2/sys/
    <CCCMD> os2386/h/GL/*.h       <OWRELROOT>/h/os2/GL/
    <CCCMD> os2386/lib/*.lib      <OWRELROOT>/lib386/os2/

[ BLOCK <1> clean ]
#==================
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h clean

[ BLOCK . . ]
#============

cdsay <PROJDIR>

