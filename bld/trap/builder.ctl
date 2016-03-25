# Debugger Trap Files Control file
# ================================

set PROJDIR=<CWD>
set PROJNAME=trap

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <1> rel ]
    cdsay <PROJDIR>

[ BLOCK <1> rel cprel ]
#======================
    <CCCMD> lcl/dos/dosr/dos.std/std*.trp           <OWRELROOT>/binw/
    <CCCMD> lcl/dos/dosx/rsi/dos.trp/rsi*.trp       <OWRELROOT>/binw/
    <CCCMD> lcl/dos/dosx/rsi/dos.srv/rsihelp.exp    <OWRELROOT>/binw/
    <CCCMD> lcl/dos/dosx/pls/dos.trp/pls*.trp       <OWRELROOT>/binw/
    <CCCMD> lcl/dos/dosx/pls/dos.srv/plshelp.exp    <OWRELROOT>/binw/
    <CCCMD> lcl/dos/dosx/pls/dosped.srv/pedhelp.exp <OWRELROOT>/binw/
    <CCCMD> lcl/dos/dosx/cw/dos.trp/cw*.trp         <OWRELROOT>/binw/
    <CCCMD> lcl/dos/dosx/cw/dos.srv/cwhelp.exe      <OWRELROOT>/binw/
    <CCCMD> lcl/dos/dosx/cw/dos.srv/cwhelp.cfg      <OWRELROOT>/binw/
    <CCCMD> net/dos.trp/net*.trp                    <OWRELROOT>/binw/
    <CCCMD> net/dos.srv/netserv.exe                 <OWRELROOT>/binw/
    <CCCMD> nmp/dos.trp/nmp*.trp                    <OWRELROOT>/binw/
    <CCCMD> nmp/dos.srv/nmpserv.exe                 <OWRELROOT>/binw/
    <CCCMD> nov/dos.trp/nov*.trp                    <OWRELROOT>/binw/
    <CCCMD> nov/dos.srv/novserv.exe                 <OWRELROOT>/binw/
    <CCCMD> par/dos.trp/par*.trp                    <OWRELROOT>/binw/
    <CCCMD> par/dos.srv/parserv.exe                 <OWRELROOT>/binw/
    <CCCMD> ser/dos.trp/ser*.trp                    <OWRELROOT>/binw/
    <CCCMD> ser/dos.srv/serserv.exe                 <OWRELROOT>/binw/
#    <CCCMD> tcp/dos.trp/tcp*.trp                    <OWRELROOT>/binw/
    <CCCMD> tcp/dos.srv/tcpserv.exe                 <OWRELROOT>/binw/
    <CCCMD> vdm/dos.srv/vdmserv.exe                 <OWRELROOT>/binw/
    <CCCMD> win/dos.trp/win*.trp                    <OWRELROOT>/binw/
    <CCCMD> win/dos.srv/winserv.exe                 <OWRELROOT>/binw/

    <CCCMD> lcl/os2v2/std/std*.d32                  <OWRELROOT>/binp/
    <CCCMD> net/os2v2.trp/net*.d32                  <OWRELROOT>/binp/
    <CCCMD> net/os2v2.srv/netserv.exe               <OWRELROOT>/binp/
    <CCCMD> nmp/os2v2.trp/nmp*.d32                  <OWRELROOT>/binp/
    <CCCMD> nmp/os2v2.srv/nmpserv.exe               <OWRELROOT>/binp/
    <CCCMD> par/os2v2.trp/par*.d32                  <OWRELROOT>/binp/
    <CCCMD> par/os2v2.srv/parserv.exe               <OWRELROOT>/binp/
    <CCCMD> ser/os2v2.trp/ser*.d32                  <OWRELROOT>/binp/
    <CCCMD> ser/os2v2.srv/serserv.exe               <OWRELROOT>/binp/
    <CCCMD> tcp/os2v2.trp/tcp*.d32                  <OWRELROOT>/binp/
    <CCCMD> tcp/os2v2.srv/tcpserv.exe               <OWRELROOT>/binp/
    <CCCMD> vdm/os2v2.trp/vdm*.d32                  <OWRELROOT>/binp/
    <CCCMD> lcl/os2v2/wvpmhelp/wdpmhook.dll         <OWRELROOT>/binp/dll/
    <CCCMD> lcl/os2v2/wvpmhelp/wdpmhelp.exe         <OWRELROOT>/binp/
    <CCCMD> lcl/os2v2/splice/wdsplice.dll           <OWRELROOT>/binp/dll/
    <CCCMD> nmp/nmpbind/nmpbind.exe                 <OWRELROOT>/binp/
    <CCCMD> par/os2v2.pio/wdio.dll                  <OWRELROOT>/binp/dll/
#os2
#    <CCCMD> nov/os2.trp/nov*.dll                    <OWRELROOT>/binp/dll/
#    <CCCMD> nov/os2.srv/novserv1.exe                <OWRELROOT>/binp/
#    <CCCMD> <OWSRCDIR>/bin/binp/dll/???.dll         <OWRELROOT>/binp/dll/
#    <CCCMD> <OWSRCDIR>/bin/binp/dll/std??.dll       <OWRELROOT>/binp/dll/
#    <CCCMD> <OWSRCDIR>/bin/binp/dll/???serv.exe     <OWRELROOT>/binp/dll/
#    <CCCMD> <OWSRCDIR>/bin/binp/???serv1.exe        <OWRELROOT>/binp/

    <CCCMD> lcl/win/std/std*.dll                    <OWRELROOT>/binw/
    <CCCMD> net/win.trp/net*.dll                    <OWRELROOT>/binw/
    <CCCMD> net/win.srv/netservw.exe                <OWRELROOT>/binw/
    <CCCMD> nmp/win.trp/nmp*.dll                    <OWRELROOT>/binw/
    <CCCMD> nmp/win.srv/nmpservw.exe                <OWRELROOT>/binw/
    <CCCMD> nov/win.trp/nov*.dll                    <OWRELROOT>/binw/
    <CCCMD> nov/win.srv/novservw.exe                <OWRELROOT>/binw/
    <CCCMD> par/win.trp/par*.dll                    <OWRELROOT>/binw/
    <CCCMD> par/win.srv/parservw.exe                <OWRELROOT>/binw/
    <CCCMD> tcp/win.trp/tcp*.dll                    <OWRELROOT>/binw/
    <CCCMD> tcp/win.srv/tcpservw.exe                <OWRELROOT>/binw/
    <CCCMD> vdm/win.srv/vdmservw.exe                <OWRELROOT>/binw/
    <CCCMD> lcl/win/int32/wint32.dll                <OWRELROOT>/binw/

    <CCCMD> lcl/nt/std/std*.dll                     <OWRELROOT>/binnt/
    <CCCMD> lcl/nt/std/std*.sym                     <OWRELROOT>/binnt/
    <CCCMD> net/nt.trp/net*.dll                     <OWRELROOT>/binnt/
    <CCCMD> net/nt.trp/net*.sym                     <OWRELROOT>/binnt/
    <CCCMD> net/nt.srv/netserv.exe                  <OWRELROOT>/binnt/
    <CCCMD> net/nt.srv/netserv.sym                  <OWRELROOT>/binnt/
#    <CCCMD> nmp/nt.trp/nmp*.dll                     <OWRELROOT>/binnt/
#    <CCCMD> nmp/nt.trp/nmp*.sym                     <OWRELROOT>/binnt/
    <CCCMD> nov/nt.trp/nov*.dll                     <OWRELROOT>/binnt/
    <CCCMD> nov/nt.trp/nov*.sym                     <OWRELROOT>/binnt/
    <CCCMD> nov/nt.srv/novserv.exe                  <OWRELROOT>/binnt/
    <CCCMD> nov/nt.srv/novserv.sym                  <OWRELROOT>/binnt/
    <CCCMD> par/nt.trp/par*.dll                     <OWRELROOT>/binnt/
    <CCCMD> par/nt.trp/par*.sym                     <OWRELROOT>/binnt/
    <CCCMD> par/nt.srv/parserv.exe                  <OWRELROOT>/binnt/
    <CCCMD> par/nt.srv/parserv.sym                  <OWRELROOT>/binnt/
    <CCCMD> ser/nt.trp/ser*.dll                     <OWRELROOT>/binnt/
    <CCCMD> ser/nt.trp/ser*.sym                     <OWRELROOT>/binnt/
    <CCCMD> ser/nt.srv/serserv.exe                  <OWRELROOT>/binnt/
    <CCCMD> ser/nt.srv/serserv.sym                  <OWRELROOT>/binnt/
    <CCCMD> tcp/nt.trp/tcp*.dll                     <OWRELROOT>/binnt/
    <CCCMD> tcp/nt.trp/tcp*.sym                     <OWRELROOT>/binnt/
    <CCCMD> tcp/nt.srv/tcpserv.exe                  <OWRELROOT>/binnt/
    <CCCMD> tcp/nt.srv/tcpserv.sym                  <OWRELROOT>/binnt/
    <CCCMD> vdm/nt.trp/vdm*.dll                     <OWRELROOT>/binnt/
    <CCCMD> vdm/nt.trp/vdm*.sym                     <OWRELROOT>/binnt/
# NT parallel port device driver and installer
    <CCCMD> par/ntsupp/dbgport.sys                  <OWRELROOT>/binnt/
    <CCCMD> par/ntsupp/dbginst.exe                  <OWRELROOT>/binnt/

    <CCCMD> lcl/nt/stdaxp/std*.dll                  <OWRELROOT>/axpnt/
    <CCCMD> nov/ntaxp.trp/nov*.dll                  <OWRELROOT>/axpnt/
    <CCCMD> nov/ntaxp.srv/novserv.exe               <OWRELROOT>/axpnt/
    <CCCMD> tcp/ntaxp.trp/tcp*.dll                  <OWRELROOT>/axpnt/
    <CCCMD> tcp/ntaxp.srv/tcpserv.exe               <OWRELROOT>/axpnt/

    <CCCMD> nov/nlm3.srv/novserv3.nlm               <OWRELROOT>/nlm/
    <CCCMD> nov/nlm4.srv/novserv4.nlm               <OWRELROOT>/nlm/
    <CCCMD> par/nlm3.srv/parserv3.nlm               <OWRELROOT>/nlm/
    <CCCMD> par/nlm4.srv/parserv4.nlm               <OWRELROOT>/nlm/
    <CCCMD> ser/nlm3.srv/serserv3.nlm               <OWRELROOT>/nlm/
    <CCCMD> ser/nlm4.srv/serserv4.nlm               <OWRELROOT>/nlm/

    <CCCMD> lcl/qnx/pmd/pmd*.trp                    <OWRELROOT>/qnx/watcom/wd/
    <CCCMD> lcl/qnx/std/std*.trp                    <OWRELROOT>/qnx/watcom/wd/
    <CCCMD> par/qnx.trp/par*.trp                    <OWRELROOT>/qnx/watcom/wd/
    <CCCMD> par/qnx.srv/parserv.exe                 <OWRELROOT>/qnx/binq/parserv
    <CCCMD> ser/qnx.trp/ser*.trp                    <OWRELROOT>/qnx/watcom/wd/
    <CCCMD> ser/qnx.srv/serserv.exe                 <OWRELROOT>/qnx/binq/serserv
    <CCCMD> tcp/qnx.trp/tcp*.trp                    <OWRELROOT>/qnx/watcom/wd/
    <CCCMD> tcp/qnx.srv/tcpserv.exe                 <OWRELROOT>/qnx/binq/tcpserv

    <CCCMD> lcl/linux/std/std*.trp                  <OWRELROOT>/binl/
    <CCCMD> lcl/linux/std/std*.sym                  <OWRELROOT>/binl/
    <CCCMD> par/linux.trp/par*.trp                  <OWRELROOT>/binl/
    <CCCMD> par/linux.trp/par*.sym                  <OWRELROOT>/binl/
    <CCCMD> par/linux.srv/parserv.exe               <OWRELROOT>/binl/parserv
    <CCCMD> par/linux.srv/parserv.sym               <OWRELROOT>/binl/
    <CCCMD> tcp/linux.trp/tcp*.trp                  <OWRELROOT>/binl/
    <CCCMD> tcp/linux.trp/tcp*.sym                  <OWRELROOT>/binl/
    <CCCMD> tcp/linux.srv/tcpserv.exe               <OWRELROOT>/binl/tcpserv
    <CCCMD> tcp/linux.srv/tcpserv.sym               <OWRELROOT>/binl/

    <CCCMD> lcl/rdos/std/std*.dll                   <OWRELROOT>/rdos/
    <CCCMD> lcl/rdos/std/std*.sym                   <OWRELROOT>/rdos/
    <CCCMD> tcp/rdos.srv/tcpserv.exe                <OWRELROOT>/rdos/
    <CCCMD> tcp/rdos.srv/tcpserv.sym                <OWRELROOT>/rdos/
    <CCCMD> ser/rdos.srv/serserv.exe                <OWRELROOT>/rdos/
    <CCCMD> ser/rdos.srv/serserv.sym                <OWRELROOT>/rdos/

    <CCCMD> net/ntx64.trp/net*.dll                  <OWRELROOT>/binnt64/
    <CCCMD> nov/ntx64.trp/nov*.dll                  <OWRELROOT>/binnt64/
    <CCCMD> ser/ntx64.trp/ser*.dll                  <OWRELROOT>/binnt64/
    <CCCMD> tcp/ntx64.trp/tcp*.dll                  <OWRELROOT>/binnt64/
# NT parallel port device driver and installer
    <CCCMD> par/ntsupp.x64/dbgport.sys              <OWRELROOT>/binnt64/
    <CCCMD> par/ntsupp.x64/dbginst.exe              <OWRELROOT>/binnt64/

[ BLOCK . . ]
#============
cdsay <PROJDIR>
