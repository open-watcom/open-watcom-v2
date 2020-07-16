# HDR Builder Control file
# ========================

set PROJNAME=hdr

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/prolog.ctl ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <BLDRULE> rel ]
#======================
    cdsay <PROJDIR>

[ BLOCK <BLDRULE> rel cprel ]
#============================
    <CCCMD> dos/*                   <OWRELROOT>/h/
    <CCCMD> dos/sys/*               <OWRELROOT>/h/sys/
    rm -f <OWRELROOT>/h/makefile
    rm -f <OWRELROOT>/h/crwatpro.sp

    <CCCMD> rdos/*                  <OWRELROOT>/rh/
    <CCCMD> rdos/sys/*              <OWRELROOT>/rh/sys/
    <CCCMD> rdos/arpa/*             <OWRELROOT>/rh/arpa/
    <CCCMD> rdos/netinet/*          <OWRELROOT>/rh/netinet/
    rm -f <OWRELROOT>/rh/makefile
    rm -f <OWRELROOT>/rh/crwatpro.sp

    <CCCMD> linux/*                 <OWRELROOT>/lh/
    <CCCMD> linux/arpa/*            <OWRELROOT>/lh/arpa/
    <CCCMD> linux/sys/*             <OWRELROOT>/lh/sys/
#    <CCCMD> linux/net/*             <OWRELROOT>/lh/net/
    <CCCMD> linux/netinet/*         <OWRELROOT>/lh/netinet/
    <CCCMD> linux/arch/i386/*       <OWRELROOT>/lh/arch/i386/
    <CCCMD> linux/arch/i386/sys/*   <OWRELROOT>/lh/arch/i386/sys/
    <CCCMD> linux/arch/mips/*       <OWRELROOT>/lh/arch/mips/
    <CCCMD> linux/arch/mips/sys/*   <OWRELROOT>/lh/arch/mips/sys/
    rm -f <OWRELROOT>/lh/makefile
    rm -f <OWRELROOT>/lh/crwatpro.sp

    <CCCMD> qnx/*                   <OWRELROOT>/qh/
    <CCCMD> qnx/sys/*               <OWRELROOT>/qh/sys/
    rm -f <OWRELROOT>/qh/makefile
    rm -f <OWRELROOT>/qh/crwatpro.sp

[ BLOCK . . ]

[ INCLUDE <OWROOT>/build/epilog.ctl ]
