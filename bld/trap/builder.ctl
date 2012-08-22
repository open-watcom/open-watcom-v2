# Debugger Trap Files Control file
# ================================

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

[ BLOCK <1> rel2 cprel2 ]
#========================
  [ IFDEF (os_dos "") <2*> ]
    <CPCMD> lcl/dos/dosr/dos.std/std.trp              <OWRELROOT>/binw/
    <CPCMD> lcl/dos/dosx/rsi/dos.trp/rsi.trp          <OWRELROOT>/binw/
    <CCCMD> lcl/dos/dosx/rsi/dos.srv/rsihelp.exp      <OWRELROOT>/binw/
    <CPCMD> lcl/dos/dosx/pls/dos.trp/pls.trp          <OWRELROOT>/binw/
    <CCCMD> lcl/dos/dosx/pls/dos.srv/plshelp.exp      <OWRELROOT>/binw/
    <CCCMD> lcl/dos/dosx/pls/dosped.srv/pedhelp.exp   <OWRELROOT>/binw/
    <CPCMD> lcl/dos/dosx/cw/dos.trp/cw.trp            <OWRELROOT>/binw/
    <CPCMD> lcl/dos/dosx/cw/dos.srv/cwhelp.exe        <OWRELROOT>/binw/
    <CPCMD> lcl/dos/dosx/cw/dos.srv/cwhelp.cfg        <OWRELROOT>/binw/
    <CPCMD> net/dos.trp/net.trp                       <OWRELROOT>/binw/
    <CPCMD> net/dos.srv/netserv.exe                   <OWRELROOT>/binw/
    <CPCMD> nmp/dos.trp/nmp.trp                       <OWRELROOT>/binw/
    <CPCMD> nmp/dos.srv/nmpserv.exe                   <OWRELROOT>/binw/
    <CPCMD> nov/dos.trp/nov.trp                       <OWRELROOT>/binw/
    <CPCMD> nov/dos.srv/novserv.exe                   <OWRELROOT>/binw/
    <CPCMD> par/dos.trp/par.trp                       <OWRELROOT>/binw/
    <CPCMD> par/dos.srv/parserv.exe                   <OWRELROOT>/binw/
    <CPCMD> ser/dos.trp/ser.trp                       <OWRELROOT>/binw/
    <CPCMD> ser/dos.srv/serserv.exe                   <OWRELROOT>/binw/
#    <CPCMD> tcp/dos.trp/tcp.trp                      <OWRELROOT>/binw/
    <CPCMD> tcp/dos.srv/tcpserv.exe                   <OWRELROOT>/binw/
    <CPCMD> vdm/dos.srv/vdmserv.exe                   <OWRELROOT>/binw/
    <CPCMD> win/dos.trp/win.trp                       <OWRELROOT>/binw/
    <CPCMD> win/dos.srv/winserv.exe                   <OWRELROOT>/binw/

  [ IFDEF (os_os2 "") <2*> ]
    <CPCMD> lcl/os2v2/std/std.d32                     <OWRELROOT>/binp/
    <CPCMD> net/os2v2.trp/net.d32                     <OWRELROOT>/binp/
    <CPCMD> net/os2v2.srv/netserv.exe                 <OWRELROOT>/binp/
    <CPCMD> nmp/os2v2.trp/nmp.d32                     <OWRELROOT>/binp/
    <CPCMD> nmp/os2v2.srv/nmpserv.exe                 <OWRELROOT>/binp/
    <CPCMD> par/os2v2.trp/par.d32                     <OWRELROOT>/binp/
    <CPCMD> par/os2v2.srv/parserv.exe                 <OWRELROOT>/binp/
    <CPCMD> ser/os2v2.trp/ser.d32                     <OWRELROOT>/binp/
    <CPCMD> ser/os2v2.srv/serserv.exe                 <OWRELROOT>/binp/
    <CPCMD> tcp/os2v2.trp/tcp.d32                     <OWRELROOT>/binp/
    <CPCMD> tcp/os2v2.srv/tcpserv.exe                 <OWRELROOT>/binp/
    <CPCMD> vdm/os2v2.trp/vdm.d32                     <OWRELROOT>/binp/
    <CPCMD> lcl/os2v2/wvpmhelp/wdpmhook.dll           <OWRELROOT>/binp/dll/
    <CPCMD> lcl/os2v2/wvpmhelp/wdpmhelp.exe           <OWRELROOT>/binp/
    <CPCMD> lcl/os2v2/splice/wdsplice.dll             <OWRELROOT>/binp/dll/
    <CPCMD> nmp/nmpbind/nmpbind.exe                   <OWRELROOT>/binp/
    <CPCMD> par/os2v2.pio/wdio.dll                    <OWRELROOT>/binp/dll/
#os2
#    <CPCMD> nov/os2.trp/nov.dll                      <OWRELROOT>/binp/dll/
#    <CPCMD> nov/os2.srv/novserv1.exe                 <OWRELROOT>/binp/
#    <CPCMD> <SRCDIR>/bin/binp/dll/???.dll                          <OWRELROOT>/binp/dll/
#    <CPCMD> <SRCDIR>/bin/binp/dll/std??.dll                        <OWRELROOT>/binp/dll/
#    <CPCMD> <SRCDIR>/bin/binp/dll/???serv.exe                      <OWRELROOT>/binp/dll/
#    <CPCMD> <SRCDIR>/bin/binp/???serv1.exe                         <OWRELROOT>/binp/

  [ IFDEF (os_win "") <2*> ]
    <CPCMD> lcl/win/std/std.dll                       <OWRELROOT>/binw/
    <CPCMD> net/win.trp/net.dll                       <OWRELROOT>/binw/
    <CPCMD> net/win.srv/netservw.exe                  <OWRELROOT>/binw/
    <CPCMD> nmp/win.trp/nmp.dll                       <OWRELROOT>/binw/
    <CPCMD> nmp/win.srv/nmpservw.exe                  <OWRELROOT>/binw/
    <CPCMD> nov/win.trp/nov.dll                       <OWRELROOT>/binw/
    <CPCMD> nov/win.srv/novservw.exe                  <OWRELROOT>/binw/
    <CPCMD> par/win.trp/par.dll                       <OWRELROOT>/binw/
    <CPCMD> par/win.srv/parservw.exe                  <OWRELROOT>/binw/
    <CPCMD> tcp/win.trp/tcp.dll                       <OWRELROOT>/binw/
    <CPCMD> tcp/win.srv/tcpservw.exe                  <OWRELROOT>/binw/
    <CPCMD> vdm/win.srv/vdmservw.exe                  <OWRELROOT>/binw/
    <CPCMD> lcl/win/int32/wint32.dll                  <OWRELROOT>/binw/

  [ IFDEF (os_nt "") <2*> ]
    <CPCMD> lcl/nt/std/std.dll                        <OWRELROOT>/binnt/
    <CPCMD> net/nt.trp/net.dll                        <OWRELROOT>/binnt/
    <CPCMD> net/nt.srv/netserv.exe                    <OWRELROOT>/binnt/
#    <CPCMD> nmp/nt.trp/nmp.dll                       <OWRELROOT>/binnt/
    <CPCMD> nov/nt.trp/nov.dll                        <OWRELROOT>/binnt/
    <CPCMD> nov/nt.srv/novserv.exe                    <OWRELROOT>/binnt/
    <CPCMD> par/nt.trp/par.dll                        <OWRELROOT>/binnt/
    <CPCMD> par/nt.srv/parserv.exe                    <OWRELROOT>/binnt/
    <CPCMD> ser/nt.trp/ser.dll                        <OWRELROOT>/binnt/
    <CPCMD> ser/nt.srv/serserv.exe                    <OWRELROOT>/binnt/
    <CPCMD> tcp/nt.trp/tcp.dll                        <OWRELROOT>/binnt/
    <CPCMD> tcp/nt.srv/tcpserv.exe                    <OWRELROOT>/binnt/
    <CPCMD> vdm/nt.trp/vdm.dll                        <OWRELROOT>/binnt/
# NT parallel port device driver and installer
    <CPCMD> par/ntsupp/dbgport.sys                    <OWRELROOT>/binnt/
    <CPCMD> par/ntsupp/dbginst.exe                    <OWRELROOT>/binnt/

  [ IFDEF (cpu_axp) <2*> ]
    <CPCMD> lcl/nt/stdaxp/std.dll                     <OWRELROOT>/axpnt/
    <CPCMD> nov/ntaxp.trp/nov.dll                     <OWRELROOT>/axpnt/
    <CPCMD> nov/ntaxp.srv/novserv.exe                 <OWRELROOT>/axpnt/
    <CPCMD> tcp/ntaxp.trp/tcp.dll                     <OWRELROOT>/axpnt/
    <CPCMD> tcp/ntaxp.srv/tcpserv.exe                 <OWRELROOT>/axpnt/

  [ IFDEF (os_nov "") <2*> ]
    <CPCMD> nov/nlm3.srv/novserv3.nlm                 <OWRELROOT>/nlm/
    <CPCMD> nov/nlm4.srv/novserv4.nlm                 <OWRELROOT>/nlm/
    <CPCMD> par/nlm3.srv/parserv3.nlm                 <OWRELROOT>/nlm/
    <CPCMD> par/nlm4.srv/parserv4.nlm                 <OWRELROOT>/nlm/
    <CPCMD> ser/nlm3.srv/serserv3.nlm                 <OWRELROOT>/nlm/
    <CPCMD> ser/nlm4.srv/serserv4.nlm                 <OWRELROOT>/nlm/

  [ IFDEF (os_qnx) <2*> ]
    <CPCMD> lcl/qnx/pmd/pmd.trp                       <OWRELROOT>/qnx/watcom/wd/
    <CPCMD> lcl/qnx/std/std.trp                       <OWRELROOT>/qnx/watcom/wd/
    <CPCMD> par/qnx.trp/par.trp                       <OWRELROOT>/qnx/watcom/wd/
    <CPCMD> par/qnx.srv/parserv.qnx                   <OWRELROOT>/qnx/binq/parserv
    <CPCMD> ser/qnx.trp/ser.trp                       <OWRELROOT>/qnx/watcom/wd/
    <CPCMD> ser/qnx.srv/serserv.qnx                   <OWRELROOT>/qnx/binq/serserv
    <CPCMD> tcp/qnx.trp/tcp.trp                       <OWRELROOT>/qnx/watcom/wd/
    <CPCMD> tcp/qnx.srv/tcpserv.qnx                   <OWRELROOT>/qnx/binq/tcpserv

  [ IFDEF (os_linux "") <2*> ]
    <CPCMD> lcl/linux/std/std.trp                     <OWRELROOT>/binl/
    <CPCMD> par/linux.trp/par.trp                     <OWRELROOT>/binl/
    <CPCMD> par/linux.srv/parserv.elf                 <OWRELROOT>/binl/parserv
    <CPCMD> tcp/linux.trp/tcp.trp                     <OWRELROOT>/binl/
    <CPCMD> tcp/linux.srv/tcpserv.elf                 <OWRELROOT>/binl/tcpserv

  [ IFDEF (os_rdos "") <2*> ]
    <CPCMD> lcl/rdos/std/std.dll                      <OWRELROOT>/rdos/
    <CPCMD> lcl/rdos/std/std.sym                      <OWRELROOT>/rdos/
    <CPCMD> tcp/rdos.srv/tcpserv.exe                  <OWRELROOT>/rdos/
    <CPCMD> tcp/rdos.srv/tcpserv.sym                  <OWRELROOT>/rdos/
    <CPCMD> ser/rdos.srv/serserv.exe                  <OWRELROOT>/rdos/
    <CPCMD> ser/rdos.srv/serserv.sym                  <OWRELROOT>/rdos/

[ BLOCK <1> clean ]
#==================
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h clean

[ BLOCK . . ]
#============

cdsay <PROJDIR>
