#
# NB! THIS MAKEFILE WAS AUTOMATICALLY GENERATED FROM MAKEFILE.ALL.
#     DO NOT EDIT.
#
# Makefile for Waterloo TCP/IP kernel
#

ASM_SOURCE = asmpkt.asm asmpkt4.asm chksum0.asm


KERNEL_SRC = language.c bsdname.c  btree.c    fragment.c misc.c     \
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
             echo.c     pppoe.c

BSD_SOURCE = accept.c   bind.c     bsddbug.c  close.c    connect.c  \
             fcntl.c    fsext.c    gethost.c  getname.c  getnet.c   \
             getprot.c  getput.c   getserv.c  geteth.c   ioctl.c    \
             listen.c   netaddr.c  neterr.c   nettime.c  nsapaddr.c \
             poll.c     presaddr.c printk.c   receive.c  select.c   \
             shutdown.c signal.c   socket.c   sockopt.c  stream.c   \
             syslog.c   syslog2.c  asc2adr.c  adr2asc.c  linkaddr.c \
             transmit.c w32pcap.c

RES_SOURCE = res_comp.c res_data.c res_debu.c  res_init.c res_loc.c  \
             res_mkqu.c res_quer.c res_send.c

C_SOURCE   = $(KERNEL_SRC) $(BSD_SOURCE) $(RES_SOURCE)



########################################################################


CC     = gcc.exe
CFLAGS = -O2 -g -I../inc -Wall -Winline -malign-loops=2           \
         -malign-jumps=2 -malign-functions=2 -fno-strength-reduce \
         -ffast-math #-fomit-frame-pointer

O      = o
OBJDIR = djgpp
OBJS   = $(addprefix $(OBJDIR)/, $(C_SOURCE:.c=.o) chksum0.o cpumodel.o)
TARGET = ../lib/libwatt.a


$(OBJDIR)/%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<

$(OBJDIR)/%.o: %.s
	$(CC) -Wall -x assembler-with-cpp -o $@ -c $<

all:   $(TARGET)

$(TARGET): $(OBJS)
	ar rs $@ $?

%.c: %.l
	flex -8 -o$@ $<

clean:
	rm -f $(OBJS)
	@echo Cleaning done

include djgpp/depend.wat


########################################################################


