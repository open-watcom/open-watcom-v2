# os2api Builder Control file
# ==========================

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/bat/master.ctl ]
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
    <CPCMD> os2286/*.h          <RELROOT>/h/os21x/
    <CPCMD> os2286/*.lib        <RELROOT>/lib286/os2/
#32-bit stuff
    <CPCMD> os2386/*.h          <RELROOT>/h/os2/
    <CPCMD> os2386/arpa/*.h     <RELROOT>/h/os2/arpa/
    <CPCMD> os2386/machine/*.h  <RELROOT>/h/os2/machine/
    <CPCMD> os2386/net/*.h      <RELROOT>/h/os2/net/
    <CPCMD> os2386/netinet/*.h  <RELROOT>/h/os2/netinet/
    <CPCMD> os2386/protocol/*.h <RELROOT>/h/os2/protocol/
    <CPCMD> os2386/sys/*.h      <RELROOT>/h/os2/sys/
    <CPCMD> os2386/GL/*.h       <RELROOT>/h/os2/GL/
    <CPCMD> os2386/*.lib        <RELROOT>/lib386/os2/

[ BLOCK <1> clean ]
#==================
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h clean

[ BLOCK . . ]
#============

cdsay <PROJDIR>

