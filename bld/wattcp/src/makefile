#
# Makefile for Waterloo TCP/IP kernel (Watcom 16-bit large model version)
#

ASM_SOURCE = asmpkt.asm asmpkt4.asm chksum0.asm


KERNEL_SRC = language.c bsdname.c  btree.c    fragment.c misc.c     &
             netback.c  pcarp.c    pcrarp.c   pcbootp.c  pcdhcp.c   &
             pcbsd.c    pcbuf.c    pcconfig.c pcdbug.c   pcicmp.c   &
             pcigmp.c   pcintr.c   pcmulti.c  pcping.c   pcpkt.c    &
             pcrecv.c   pcsed.c    pcslip.c   pcstat.c   pctcp.c    &
             pc_cbrk.c  qmsg.c     sock_sel.c sock_dbu.c sock_ini.c &
             sock_io.c  sock_prn.c sock_scn.c sock_in.c  strings.c  &
             loopback.c udp_dom.c  udp_nds.c  version.c  gettod.c   &
             getopt.c   crit.c     country.c  crc.c      pcpkt32.c  &
             pcqueue.c  lookup.c   ip_out.c   tcp_fsm.c  wattcpd.c  &
             fortify.c  tftp.c     timer.c    highc.c    wdpmi.c    &
             oldstuff.c ports.c    chksum.c   profile.c  settod.c   &
             echo.c     pppoe.c

BSD_SOURCE = accept.c   bind.c     bsddbug.c  close.c    connect.c  &
             fcntl.c    fsext.c    gethost.c  getname.c  getnet.c   &
             getprot.c  getput.c   getserv.c  geteth.c   ioctl.c    &
             listen.c   netaddr.c  neterr.c   nettime.c  nsapaddr.c &
             poll.c     presaddr.c printk.c   receive.c  select.c   &
             shutdown.c signal.c   socket.c   sockopt.c  stream.c   &
             syslog.c   syslog2.c  asc2adr.c  adr2asc.c  linkaddr.c &
             transmit.c w32pcap.c

RES_SOURCE = res_comp.c res_data.c res_debu.c  res_init.c res_loc.c  &
             res_mkqu.c res_quer.c res_send.c

C_SOURCE   = $(KERNEL_SRC) $(BSD_SOURCE) $(RES_SOURCE)

OBJS = $(OBJDIR)asmpkt.obj   $(OBJDIR)asmpkt4.obj   &
       $(OBJDIR)chksum0.obj  $(OBJDIR)cpumodel.obj  &
       $(OBJDIR)wdpmi.obj    $(OBJDIR)language.obj  &
       $(OBJDIR)bsdname.obj  $(OBJDIR)tftp.obj      &
       $(OBJDIR)btree.obj    $(OBJDIR)fragment.obj  &
       $(OBJDIR)misc.obj     $(OBJDIR)netback.obj   &
       $(OBJDIR)pcarp.obj    $(OBJDIR)pcrarp.obj    &
       $(OBJDIR)pcbootp.obj  $(OBJDIR)pcdhcp.obj    &
       $(OBJDIR)pcbsd.obj    $(OBJDIR)pcbuf.obj     &
       $(OBJDIR)pcconfig.obj $(OBJDIR)pcdbug.obj    &
       $(OBJDIR)pcicmp.obj   $(OBJDIR)pcigmp.obj    &
       $(OBJDIR)pcintr.obj   $(OBJDIR)pcmulti.obj   &
       $(OBJDIR)pcping.obj   $(OBJDIR)pcpkt.obj     &
       $(OBJDIR)pcrecv.obj   $(OBJDIR)pcsed.obj     &
       $(OBJDIR)pcslip.obj   $(OBJDIR)pcstat.obj    &
       $(OBJDIR)pctcp.obj    $(OBJDIR)pc_cbrk.obj   &
       $(OBJDIR)qmsg.obj     $(OBJDIR)sock_sel.obj  &
       $(OBJDIR)sock_dbu.obj $(OBJDIR)sock_ini.obj  &
       $(OBJDIR)sock_prn.obj $(OBJDIR)sock_scn.obj  &
       $(OBJDIR)oldstuff.obj $(OBJDIR)sock_in.obj   &
       $(OBJDIR)strings.obj  $(OBJDIR)udp_dom.obj   &
       $(OBJDIR)udp_nds.obj  $(OBJDIR)version.obj   &
       $(OBJDIR)gettod.obj   $(OBJDIR)getopt.obj    &
       $(OBJDIR)crit.obj     $(OBJDIR)country.obj   &
       $(OBJDIR)crc.obj      $(OBJDIR)pcpkt32.obj   &
       $(OBJDIR)pcqueue.obj  $(OBJDIR)lookup.obj    &
       $(OBJDIR)loopback.obj $(OBJDIR)wattcpd.obj   &
       $(OBJDIR)fortify.obj  $(OBJDIR)w32pcap.obj   &
       $(OBJDIR)accept.obj   $(OBJDIR)bind.obj      &
       $(OBJDIR)close.obj    $(OBJDIR)connect.obj   &
       $(OBJDIR)gethost.obj  $(OBJDIR)getname.obj   &
       $(OBJDIR)getnet.obj   $(OBJDIR)getprot.obj   &
       $(OBJDIR)getput.obj   $(OBJDIR)getserv.obj   &
       $(OBJDIR)geteth.obj   $(OBJDIR)ioctl.obj     &
       $(OBJDIR)fcntl.obj    $(OBJDIR)listen.obj    &
       $(OBJDIR)netaddr.obj  $(OBJDIR)receive.obj   &
       $(OBJDIR)select.obj   $(OBJDIR)bsddbug.obj   &
       $(OBJDIR)socket.obj   $(OBJDIR)sockopt.obj   &
       $(OBJDIR)transmit.obj $(OBJDIR)nsapaddr.obj  &
       $(OBJDIR)presaddr.obj $(OBJDIR)neterr.obj    &
       $(OBJDIR)signal.obj   $(OBJDIR)shutdown.obj  &
       $(OBJDIR)stream.obj   $(OBJDIR)syslog.obj    &
       $(OBJDIR)syslog2.obj  $(OBJDIR)printk.obj    &
       $(OBJDIR)nettime.obj  $(OBJDIR)ip_out.obj    &
       $(OBJDIR)tcp_fsm.obj  $(OBJDIR)poll.obj      &
       $(OBJDIR)timer.obj    $(OBJDIR)chksum.obj    &
       $(OBJDIR)adr2asc.obj  $(OBJDIR)linkaddr.obj  &
       $(OBJDIR)ports.obj    $(OBJDIR)sock_io.obj   &
       $(OBJDIR)echo.obj     $(OBJDIR)res_comp.obj  &
       $(OBJDIR)res_data.obj $(OBJDIR)res_debu.obj  &
       $(OBJDIR)res_init.obj $(OBJDIR)res_loc.obj   &
       $(OBJDIR)res_mkqu.obj $(OBJDIR)res_quer.obj  &
       $(OBJDIR)res_send.obj $(OBJDIR)asc2adr.obj   &
       $(OBJDIR)profile.obj  $(OBJDIR)settod.obj    &
       $(OBJDIR)pppoe.obj

########################################################################

#
# Watcom's wmake is weird and requires special handling.
#
#.SILENT
.SUFFIXES: .l

CC     = wcc
AFLAGS = -0s
!ifdef __LINUX__
CFLAGS = -ml -6 -I../inc;$(%WATCOM)/h
TARGET = ../lib/wattcpwl.lib
OBJDIR = watcom/large/
!else
CFLAGS = -ml -6 -I..\inc;$(%WATCOM)\h
TARGET = ..\lib\wattcpwl.lib
OBJDIR = watcom\large\
!endif
LIBARG = $(OBJDIR)wlib.arg

AFLAGS = $(AFLAGS) -zq -bt=dos -fr=nul -d1
CFLAGS = $(CFLAGS) -zq -bt=dos -fr=nul -w4 -fpc -zm -s -oilrtfm

# C-flags used:
#   -m{s,l,f} memory model; small, large or flat
#   -3s       386 stack call convention
#   -3r       386 register calls
#   -s        no stack checking
#   -zq       quiet compiling
#   -d3       generate full debug info
#   -fpi      inline math + emulation
#   -fr       write errors to file (and stdout)
#   -bt=dos   target system - dos
#   -zm       place each function in separate segment
#   -oilrt    optimization flags
#     i:      expand intrinsics
#     l:      loop optimisations
#     r:      reorder instructions
#     t:      favor execution time
#     f:      always use stack frames
#     m:      generate inline code for math functions
#
#  This should make the smallest code on a 386
#    -oahkrs -s -em -zp1 -3r -fp3
#

AS = wasm

all: $(TARGET)

$(TARGET): $(OBJS) $(LIBARG)
      wlib -q -b -c $(TARGET) @$(LIBARG)

$(OBJDIR)asmpkt.obj:  asmpkt.asm
$(OBJDIR)asmpkt4.obj: asmpkt4.asm

.ERASE
.c{$(OBJDIR)}.obj: .AUTODEPEND
          *$(CC) $[@ $(CFLAGS) -fo=$@

.ERASE
.asm{$(OBJDIR)}.obj:
          *$(AS) $[@ $(AFLAGS) -fo=$@

check_arg: .SYMBOLIC
!ifneq __MAKEOPTS__ ms f
          @echo Usage: 'wmake -ms -f $(__MAKEFILES__)'
!endif

# Not needed unless you change the lexical analyser...
#
# language.c: language.l
#          flex -8 -o$@ $[@

clean: .SYMBOLIC
          rm -f $(OBJDIR)*.obj
          rm -f $(C_ARGS)
          rm -f $(LIBARG)
          rm -f $(TARGET)
          @echo Cleaning done

$(LIBARG): $(__MAKEFILES__)
          @echo Building librarian command script
          @%create $^@
          @for %f in ($(OBJS)) do @%append $^@ +- %f

########################################################################

