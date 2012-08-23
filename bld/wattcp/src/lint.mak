#
# GNU Makefile for checking sources with LC-lint.
# djgpp 2.03 required.
#

.SUFFIXES: .lnt

LINT = $(DJDIR)/contrib/lclint.24/bin/lclint.exe

LFLAGS = +standard -I../inc -I$(DJDIR)/include -sysdirs $(DJDIR)/include \
         -Dlint -D__DJGPP__=2 -D__GNUC__=2 -D__GNUC_MINOR__=9 -DDOSX=2   \
         -warnposix -linelen 80 -nullassign -noeffect -mustfree          \
         -duplicatequals +ignoresigns -predboolothers -predboolint       \
         -boolops +boolint +charint -exportlocal -nullpass -nullret      \
         -unqualifiedtrans -onlytrans -branchstate -globstate -type      \
         -retvalbool -retvalother -retvalint -compmempass -temptrans     \
         -nullstate -compdef -exitarg -nestedextern -statictrans         \
         -immediatetrans -castfcnptr -mayaliasunique -modobserver        \
         -uniondef -usereleased -fullinitblock -macromatchname

SOURCES = language.c bsdname.c  btree.c    fragment.c misc.c     \
          netback.c  pcarp.c    pcrarp.c   pcbootp.c  pcdhcp.c   \
          pcbsd.c    pcbuf.c    pcconfig.c pcdbug.c   pcicmp.c   \
          pcigmp.c   pcintr.c   pcmulti.c  pcping.c   pcpkt.c    \
          pcrecv.c   pcsed.c    pcslip.c   pcstat.c   pctcp.c    \
          pc_cbrk.c  qmsg.c     sock_sel.c sock_dbu.c sock_ini.c \
          sock_io.c  sock_prn.c sock_scn.c sock_in.c  strings.c  \
          loopback.c udp_dom.c  udp_nds.c  version.c  gettod.c   \
          getopt.c   crit.c     country.c  crc.c      pcpkt32.c  \
          pcqueue.c  lookup.c   ip_out.c   tcp_fsm.c  wattcpd.c  \
          fortify.c  tftp.c     timer.c    highc.c    wdpmi.c    \
          oldstuff.c ports.c    chksum.c   profile.c  settod.c   \
          accept.c   bind.c     bsddbug.c  close.c    connect.c  \
          fcntl.c    fsext.c    gethost.c  getname.c  getnet.c   \
          getprot.c  getput.c   getserv.c  geteth.c   ioctl.c    \
          listen.c   netaddr.c  neterr.c   nettime.c  nsapaddr.c \
          poll.c     presaddr.c printk.c   receive.c  select.c   \
          shutdown.c signal.c   socket.c   sockopt.c  stream.c   \
          syslog.c   syslog2.c  asc2adr.c  adr2asc.c  linkaddr.c \
          transmit.c echo.c     w32pcap.c  res_comp.c res_data.c \
          res_debu.c res_init.c res_loc.c  res_mkqu.c res_quer.c \
          res_send.c

LFILES = $(SOURCES:.c=.lnt)

all: $(LFILES)

.c.lnt:
	@echo $<
	@$(LINT) $(LFLAGS) $< > $@

clean:
	rm -f $(LFILES)

