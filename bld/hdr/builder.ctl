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
    <CCCMD> dos/h/*                   <OWRELROOT>/h/
    <CCCMD> dos/h/sys/*               <OWRELROOT>/h/sys/

    <CCCMD> rdos/h/*                  <OWRELROOT>/rh/
    <CCCMD> rdos/h/sys/*              <OWRELROOT>/rh/sys/
    <CCCMD> rdos/h/arpa/*             <OWRELROOT>/rh/arpa/
    <CCCMD> rdos/h/netinet/*          <OWRELROOT>/rh/netinet/

    <CCCMD> linux/h/*                 <OWRELROOT>/lh/
    <CCCMD> linux/h/arpa/*            <OWRELROOT>/lh/arpa/
    <CCCMD> linux/h/sys/*             <OWRELROOT>/lh/sys/
#    <CCCMD> linux/h/net/*             <OWRELROOT>/lh/net/
    <CCCMD> linux/h/netinet/*         <OWRELROOT>/lh/netinet/
    <CCCMD> linux/h/arch/i386/*       <OWRELROOT>/lh/arch/i386/
    <CCCMD> linux/h/arch/i386/sys/*   <OWRELROOT>/lh/arch/i386/sys/
    <CCCMD> linux/h/arch/mips/*       <OWRELROOT>/lh/arch/mips/
    <CCCMD> linux/h/arch/mips/sys/*   <OWRELROOT>/lh/arch/mips/sys/

    <CCCMD> qnx/h/*                   <OWRELROOT>/qh/
    <CCCMD> qnx/h/sys/*               <OWRELROOT>/qh/sys/

[ BLOCK . . ]

[ INCLUDE <OWROOT>/build/epilog.ctl ]
