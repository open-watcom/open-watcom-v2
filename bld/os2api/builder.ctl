# os2api Builder Control file
# ==========================

set PROJNAME=os2api

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/prolog.ctl ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <BLDRULE> rel ]
#======================
    cdsay <PROJDIR>

[ BLOCK <BLDRULE> rel cprel ]
#============================
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

[ BLOCK . . ]

[ INCLUDE <OWROOT>/build/epilog.ctl ]
