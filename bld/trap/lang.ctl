# Debugger Trap Files Control file
# ================================

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

[ BLOCK <1> rel2 cprel2 ]
#========================
  [ IFDEF (os_dos "") <2*> ]
    <CPCMD> <DEVDIR>/trap/lcl/dos/dosr/dos.std/std.trp              <RELROOT>/binw/
    <CPCMD> <DEVDIR>/trap/lcl/dos/dosx/rsi/dos.trp/rsi.trp          <RELROOT>/binw/
    <CCCMD> <DEVDIR>/trap/lcl/dos/dosx/rsi/dos.srv/rsihelp.exp      <RELROOT>/binw/
    <CPCMD> <DEVDIR>/trap/lcl/dos/dosx/pls/dos.trp/pls.trp          <RELROOT>/binw/
    <CCCMD> <DEVDIR>/trap/lcl/dos/dosx/pls/dos.srv/plshelp.exp      <RELROOT>/binw/
    <CCCMD> <DEVDIR>/trap/lcl/dos/dosx/pls/dosped.srv/pedhelp.exp   <RELROOT>/binw/
    <CPCMD> <DEVDIR>/trap/lcl/dos/dosx/cw/dos.trp/cw.trp            <RELROOT>/binw/
    <CPCMD> <DEVDIR>/trap/lcl/dos/dosx/cw/dos.srv/cwhelp.exe        <RELROOT>/binw/
    <CPCMD> <DEVDIR>/trap/lcl/dos/dosx/cw/dos.srv/cwhelp.cfg        <RELROOT>/binw/
    <CPCMD> <DEVDIR>/trap/net/dos.trp/net.trp                       <RELROOT>/binw/
    <CPCMD> <DEVDIR>/trap/net/dos.srv/netserv.exe                   <RELROOT>/binw/
    <CPCMD> <DEVDIR>/trap/nmp/dos.trp/nmp.trp                       <RELROOT>/binw/
    <CPCMD> <DEVDIR>/trap/nmp/dos.srv/nmpserv.exe                   <RELROOT>/binw/
    <CPCMD> <DEVDIR>/trap/nov/dos.trp/nov.trp                       <RELROOT>/binw/
    <CPCMD> <DEVDIR>/trap/nov/dos.srv/novserv.exe                   <RELROOT>/binw/
    <CPCMD> <DEVDIR>/trap/par/dos.trp/par.trp                       <RELROOT>/binw/
    <CPCMD> <DEVDIR>/trap/par/dos.srv/parserv.exe                   <RELROOT>/binw/
    <CPCMD> <DEVDIR>/trap/ser/dos.trp/ser.trp                       <RELROOT>/binw/
    <CPCMD> <DEVDIR>/trap/ser/dos.srv/serserv.exe                   <RELROOT>/binw/
#    <CPCMD> <DEVDIR>/trap/tcp/dos.trp/tcp.trp                      <RELROOT>/binw/
    <CPCMD> <DEVDIR>/trap/tcp/dos.srv/tcpserv.exe                   <RELROOT>/binw/
    <CPCMD> <DEVDIR>/trap/vdm/dos.srv/vdmserv.exe                   <RELROOT>/binw/
    <CPCMD> <DEVDIR>/trap/win/dos.trp/win.trp                       <RELROOT>/binw/
    <CPCMD> <DEVDIR>/trap/win/dos.srv/winserv.exe                   <RELROOT>/binw/

  [ IFDEF (os_os2 "") <2*> ]
    <CPCMD> <DEVDIR>/trap/lcl/os2v2/std/std.d32                     <RELROOT>/binp/
    <CPCMD> <DEVDIR>/trap/net/os2v2.trp/net.d32                     <RELROOT>/binp/
    <CPCMD> <DEVDIR>/trap/net/os2v2.srv/netserv.exe                 <RELROOT>/binp/
    <CPCMD> <DEVDIR>/trap/nmp/os2v2.trp/nmp.d32                     <RELROOT>/binp/
    <CPCMD> <DEVDIR>/trap/nmp/os2v2.srv/nmpserv.exe                 <RELROOT>/binp/
    <CPCMD> <DEVDIR>/trap/par/os2v2.trp/par.d32                     <RELROOT>/binp/
    <CPCMD> <DEVDIR>/trap/par/os2v2.srv/parserv.exe                 <RELROOT>/binp/
    <CPCMD> <DEVDIR>/trap/ser/os2v2.trp/ser.d32                     <RELROOT>/binp/
    <CPCMD> <DEVDIR>/trap/ser/os2v2.srv/serserv.exe                 <RELROOT>/binp/
    <CPCMD> <DEVDIR>/trap/tcp/os2v2.trp/tcp.d32                     <RELROOT>/binp/
    <CPCMD> <DEVDIR>/trap/tcp/os2v2.srv/tcpserv.exe                 <RELROOT>/binp/
    <CPCMD> <DEVDIR>/trap/vdm/os2v2.trp/vdm.d32                     <RELROOT>/binp/
    <CPCMD> <DEVDIR>/trap/lcl/os2v2/wvpmhelp/wdpmhook.dll           <RELROOT>/binp/dll/
    <CPCMD> <DEVDIR>/trap/lcl/os2v2/wvpmhelp/wdpmhelp.exe           <RELROOT>/binp/
    <CPCMD> <DEVDIR>/trap/lcl/os2v2/splice/wdsplice.dll             <RELROOT>/binp/dll/
    <CPCMD> <DEVDIR>/trap/nmp/nmpbind/nmpbind.exe                   <RELROOT>/binp/
    <CPCMD> <DEVDIR>/trap/par/os2v2.pio/wdio.dll                    <RELROOT>/binp/dll/
#os2
#    <CPCMD> <DEVDIR>/trap/nov/os2.trp/nov.dll                      <RELROOT>/binp/dll/
#    <CPCMD> <DEVDIR>/trap/nov/os2.srv/novserv1.exe                 <RELROOT>/binp/
#    <CPCMD> <DEVDIR>/bin/binp/dll/???.dll                          <RELROOT>/binp/dll/
#    <CPCMD> <DEVDIR>/bin/binp/dll/std??.dll                        <RELROOT>/binp/dll/
#    <CPCMD> <DEVDIR>/bin/binp/dll/???serv.exe                      <RELROOT>/binp/dll/
#    <CPCMD> <DEVDIR>/bin/binp/???serv1.exe                         <RELROOT>/binp/

  [ IFDEF (os_win "") <2*> ]
    <CPCMD> <DEVDIR>/trap/lcl/win/std/std.dll                       <RELROOT>/binw/
    <CPCMD> <DEVDIR>/trap/net/win.trp/net.dll                       <RELROOT>/binw/
    <CPCMD> <DEVDIR>/trap/net/win.srv/netservw.exe                  <RELROOT>/binw/
    <CPCMD> <DEVDIR>/trap/nmp/win.trp/nmp.dll                       <RELROOT>/binw/
    <CPCMD> <DEVDIR>/trap/nmp/win.srv/nmpservw.exe                  <RELROOT>/binw/
    <CPCMD> <DEVDIR>/trap/nov/win.trp/nov.dll                       <RELROOT>/binw/
    <CPCMD> <DEVDIR>/trap/nov/win.srv/novservw.exe                  <RELROOT>/binw/
    <CPCMD> <DEVDIR>/trap/par/win.trp/par.dll                       <RELROOT>/binw/
    <CPCMD> <DEVDIR>/trap/par/win.srv/parservw.exe                  <RELROOT>/binw/
    <CPCMD> <DEVDIR>/trap/tcp/win.trp/tcp.dll                       <RELROOT>/binw/
    <CPCMD> <DEVDIR>/trap/tcp/win.srv/tcpservw.exe                  <RELROOT>/binw/
    <CPCMD> <DEVDIR>/trap/vdm/win.srv/vdmservw.exe                  <RELROOT>/binw/
    <CPCMD> <DEVDIR>/trap/lcl/win/int32/wint32.dll                  <RELROOT>/binw/

  [ IFDEF (os_nt "") <2*> ]
    <CPCMD> <DEVDIR>/trap/lcl/nt/std/std.dll                        <RELROOT>/binnt/
    <CPCMD> <DEVDIR>/trap/net/nt.trp/net.dll                        <RELROOT>/binnt/
    <CPCMD> <DEVDIR>/trap/net/nt.srv/netserv.exe                    <RELROOT>/binnt/
#    <CPCMD> <DEVDIR>/trap/nmp/nt.trp/nmp.dll                       <RELROOT>/binnt/
    <CPCMD> <DEVDIR>/trap/nov/nt.trp/nov.dll                        <RELROOT>/binnt/
    <CPCMD> <DEVDIR>/trap/nov/nt.srv/novserv.exe                    <RELROOT>/binnt/
    <CPCMD> <DEVDIR>/trap/par/nt.trp/par.dll                        <RELROOT>/binnt/
    <CPCMD> <DEVDIR>/trap/par/nt.srv/parserv.exe                    <RELROOT>/binnt/
    <CPCMD> <DEVDIR>/trap/ser/nt.trp/ser.dll                        <RELROOT>/binnt/
    <CPCMD> <DEVDIR>/trap/ser/nt.srv/serserv.exe                    <RELROOT>/binnt/
    <CPCMD> <DEVDIR>/trap/tcp/nt.trp/tcp.dll                        <RELROOT>/binnt/
    <CPCMD> <DEVDIR>/trap/tcp/nt.srv/tcpserv.exe                    <RELROOT>/binnt/
    <CPCMD> <DEVDIR>/trap/vdm/nt.trp/vdm.dll                        <RELROOT>/binnt/
# NT parallel port device driver and installer
    <CPCMD> <DEVDIR>/trap/par/ntsupp/dbgport.sys                    <RELROOT>/binnt/
    <CPCMD> <DEVDIR>/trap/par/ntsupp/dbginst.exe                    <RELROOT>/binnt/

  [ IFDEF (cpu_axp) <2*> ]
    <CPCMD> <DEVDIR>/trap/lcl/nt/stdaxp/std.dll                     <RELROOT>/axpnt/
    <CPCMD> <DEVDIR>/trap/nov/ntaxp.trp/nov.dll                     <RELROOT>/axpnt/
    <CPCMD> <DEVDIR>/trap/nov/ntaxp.srv/novserv.exe                 <RELROOT>/axpnt/
    <CPCMD> <DEVDIR>/trap/tcp/ntaxp.trp/tcp.dll                     <RELROOT>/axpnt/
    <CPCMD> <DEVDIR>/trap/tcp/ntaxp.srv/tcpserv.exe                 <RELROOT>/axpnt/

  [ IFDEF (os_nov "") <2*> ]
    <CPCMD> <DEVDIR>/trap/nov/nlm3.srv/novserv3.nlm                 <RELROOT>/nlm/
    <CPCMD> <DEVDIR>/trap/nov/nlm4.srv/novserv4.nlm                 <RELROOT>/nlm/
    <CPCMD> <DEVDIR>/trap/par/nlm3.srv/parserv3.nlm                 <RELROOT>/nlm/
    <CPCMD> <DEVDIR>/trap/par/nlm4.srv/parserv4.nlm                 <RELROOT>/nlm/
    <CPCMD> <DEVDIR>/trap/ser/nlm3.srv/serserv3.nlm                 <RELROOT>/nlm/
    <CPCMD> <DEVDIR>/trap/ser/nlm4.srv/serserv4.nlm                 <RELROOT>/nlm/

  [ IFDEF (os_qnx) <2*> ]
    <CPCMD> <DEVDIR>/trap/lcl/qnx/pmd/pmd.trp                       <RELROOT>/qnx/watcom/wd/
    <CPCMD> <DEVDIR>/trap/lcl/qnx/std/std.trp                       <RELROOT>/qnx/watcom/wd/
    <CPCMD> <DEVDIR>/trap/par/qnx.trp/par.trp                       <RELROOT>/qnx/watcom/wd/
    <CPCMD> <DEVDIR>/trap/par/qnx.srv/parserv.qnx                   <RELROOT>/qnx/binq/parserv
    <CPCMD> <DEVDIR>/trap/ser/qnx.trp/ser.trp                       <RELROOT>/qnx/watcom/wd/
    <CPCMD> <DEVDIR>/trap/ser/qnx.srv/serserv.qnx                   <RELROOT>/qnx/binq/serserv
    <CPCMD> <DEVDIR>/trap/tcp/qnx.trp/tcp.trp                       <RELROOT>/qnx/watcom/wd/
    <CPCMD> <DEVDIR>/trap/tcp/qnx.srv/tcpserv.qnx                   <RELROOT>/qnx/binq/tcpserv

  [ IFDEF (os_linux "") <2*> ]
    <CPCMD> <DEVDIR>/trap/lcl/linux/std/std.trp                     <RELROOT>/binl/
    <CPCMD> <DEVDIR>/trap/par/linux.trp/par.trp                     <RELROOT>/binl/
    <CPCMD> <DEVDIR>/trap/par/linux.srv/parserv.elf                 <RELROOT>/binl/parserv
    <CPCMD> <DEVDIR>/trap/tcp/linux.trp/tcp.trp                     <RELROOT>/binl/
    <CPCMD> <DEVDIR>/trap/tcp/linux.srv/tcpserv.elf                 <RELROOT>/binl/tcpserv

  [ IFDEF (os_rdos "") <2*> ]
    <CPCMD> <DEVDIR>/trap/lcl/rdos/std/std.dll                      <RELROOT>/rdos/
    <CPCMD> <DEVDIR>/trap/lcl/rdos/std/std.sym                      <RELROOT>/rdos/
    <CPCMD> <DEVDIR>/trap/tcp/rdos.srv/tcpserv.exe                  <RELROOT>/rdos/
    <CPCMD> <DEVDIR>/trap/tcp/rdos.srv/tcpserv.sym                  <RELROOT>/rdos/
    <CPCMD> <DEVDIR>/trap/ser/rdos.srv/serserv.exe                  <RELROOT>/rdos/
    <CPCMD> <DEVDIR>/trap/ser/rdos.srv/serserv.sym                  <RELROOT>/rdos/

[ BLOCK <1> clean ]
#==================
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h clean

[ BLOCK . . ]
#============

cdsay <PROJDIR>
