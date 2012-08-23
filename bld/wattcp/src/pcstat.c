/*
 *  Update and printing of MAC/IP/ICMP/IGMP/UDP/TCP input/output
 *  statistics counters.
 *
 *  G. Vanem <giva@bgnett.no> 1997
 */

#include <stdio.h>
#include <time.h>

#include "wattcp.h"
#include "strings.h"
#include "sock_ini.h"
#include "pcconfig.h"
#include "pcicmp.h"
#include "pcqueue.h"
#include "pcsed.h"
#include "pcpkt.h"
#include "pcstat.h"

int sock_stats (tcp_Socket *s, WORD *days, WORD *inactive,
                WORD *cwindow, WORD *avg,  WORD *sd)
{
  time_t now = time (NULL);

  if (s->ip_type != TCP_PROTO)
     return (0);

  if (days)     *days     = (WORD) (now / (3600*24) - _watt_start_day);
  if (inactive) *inactive = sock_inactive;
#if defined(USE_UDP_ONLY)
  if (cwindow)  *cwindow  = 0;
  if (avg)      *avg      = 0;
  if (sd)       *sd       = 0;
#else
  if (cwindow)  *cwindow  = s->cwindow;
  if (avg)      *avg      = s->vj_sa >> 3;
  if (sd)       *sd       = s->vj_sd >> 2;
#endif
  return (1);
}

#if !defined(USE_STATISTICS)
  void print_ip_stats  (void) {}
  void print_icmp_stats(void) {}
  void print_igmp_stats(void) {}
  void print_udp_stats (void) {}
  void print_tcp_stats (void) {}
  void print_all_stats (void) {}
  void reset_stats     (void) {}

#else  /* rest of file */

struct macstat   macstats;
struct ipstat    ipstats;
struct udpstat   udpstats;
struct tcpstat   tcpstats;
struct icmpstat  icmpstats;
struct igmpstat  igmpstats;
struct pppoestat pppoestats;

void reset_stats (void)
{
  memset (&macstats, 0, sizeof(macstats));
  memset (&ipstats,  0, sizeof(ipstats));
  memset (&udpstats, 0, sizeof(udpstats));
  memset (&tcpstats, 0, sizeof(tcpstats));
  memset (&icmpstats,0, sizeof(icmpstats));
  memset (&igmpstats,0, sizeof(igmpstats));
}

static void show_stat (const char *name, DWORD num)
{
  if (num)
    (*_printf) ("    %-12s %10lu\r\n", name, num);
}

void print_mac_stats (void)
{
  (*_printf)("MAC input stats:\r\n"
             "    IP:     drop %d, waiting %d\r\n"
             "    non-IP: drop %d, waiting %d, recv/sent %lu/%lu\r\n"
             "    unknown type %lu, LLC frames %lu\r\n",
             pkt_dropped_ip(), pkt_waiting_ip(),
             pkt_dropped_arp(),pkt_waiting_arp(),
             macstats.non_ip_recv, macstats.non_ip_sent,
             macstats.num_err_type, macstats.num_llc_frames);

  (*_printf)("MAC output stats:\r\n"
             "    Tx errors    %10lu\r\n", macstats.num_tx_err);
}

void print_arp_stats (void)
{
  (*_printf)("ARP Requests:    %10lu recv\r\n"
             "    Requests:    %10lu sent\r\n"
             "    Replies:     %10lu recv\r\n"
             "    Replies:     %10lu sent\r\n",
             macstats.arp.request_recv, macstats.arp.request_sent,
             macstats.arp.reply_recv, macstats.arp.reply_sent);
}

void print_ip_stats (void)
{
  (*_printf)("IP input stats:\r\n");
  show_stat ("total:",     ipstats.ips_total);
  show_stat ("badsum:",    ipstats.ips_badsum);
  show_stat ("badver:",    ipstats.ips_badvers);
  show_stat ("short:",     ipstats.ips_tooshort);
  show_stat ("frags:",     ipstats.ips_fragments);
  show_stat ("fragdrop:",  ipstats.ips_fragdropped);
  show_stat ("fragtime:",  ipstats.ips_fragtimeout);
  show_stat ("reassemble:",ipstats.ips_reassembled);
  show_stat ("noproto:",   ipstats.ips_noproto);
  show_stat ("delivered:", ipstats.ips_delivered);
  show_stat ("badoptions:",ipstats.ips_badoptions);
  show_stat ("dropped:",   ipstats.ips_idropped);

  (*_printf)("IP output stats:\r\n");
  show_stat ("total:",     ipstats.ips_rawout);
  show_stat ("noroute:",   ipstats.ips_noroute);
  show_stat ("frags:",     ipstats.ips_ofragments);
  show_stat ("dropped:",   ipstats.ips_odropped);
}

void print_icmp_stats (void)
{
  DWORD  total_in  = 0;
  DWORD  total_out = 0;
  int    i;
  static char *types[ICMP_MAXTYPE+1] = {
              "echoreply:", "", "",   /* 0-2 */
              "unreach:",
              "srcquench:",
              "redirect:", "", "",    /* 5-7 */
              "echorequest:",
              "router adv:", "router sol:",
              "timex:",
              "parm prob:",
              "tstamp req:", "tstamp rep:",
              "info req:",   "info rep:",
              "mask req:",   "mask rep:"
            };

  (*_printf)("ICMP input stats:\r\n");
  for (i = 0; i < ICMP_MAXTYPE; i++)
      total_in += icmpstats.icps_inhist[i];
  show_stat ("total:", total_in);

  show_stat ("badsum:", icmpstats.icps_checksum);
  show_stat ("badcode:",icmpstats.icps_badcode);
  show_stat ("short:",  icmpstats.icps_tooshort);

  for (i = 0; i < ICMP_MAXTYPE; i++)
     show_stat (types[i], icmpstats.icps_inhist[i]);

  (*_printf)("ICMP output stats:\r\n");
  for (i = 0; i < ICMP_MAXTYPE; i++)
      total_out += icmpstats.icps_outhist[i];
  show_stat ("total:", total_out);

  for (i = 0; i < ICMP_MAXTYPE; i++)
     show_stat (types[i], icmpstats.icps_outhist[i]);
}

void print_igmp_stats (void)
{
  (*_printf)("IGMP input stats:\r\n");
  show_stat ("total:",        igmpstats.igps_rcv_total);
  show_stat ("badsum:",       igmpstats.igps_rcv_badsum);
  show_stat ("short:",        igmpstats.igps_rcv_tooshort);
  show_stat ("queries:",      igmpstats.igps_rcv_queries);
  show_stat ("queries (bad):",igmpstats.igps_rcv_badqueries);
  show_stat ("reports:",      igmpstats.igps_rcv_reports);
  show_stat ("reports (bad):",igmpstats.igps_rcv_badreports);
  show_stat ("reports (grp):",igmpstats.igps_rcv_ourreports);

  (*_printf)("IGMP output stats:\r\n");
  show_stat ("reports:", igmpstats.igps_snd_reports);
}

void print_udp_stats (void)
{
  (*_printf)("UDP input stats:\r\n");
  show_stat ("total:",      udpstats.udps_ipackets);
  show_stat ("drops:",      udpstats.udps_hdrops);
  show_stat ("badsum:",     udpstats.udps_badsum);
  show_stat ("no service:", udpstats.udps_noport);
  show_stat ("broadcast:",  udpstats.udps_noportbcast);
  show_stat ("queue full:", udpstats.udps_fullsock);

  (*_printf)("UDP output stats:\r\n");
  show_stat ("total:",      udpstats.udps_opackets);
}

void print_tcp_stats (void)
{
  (*_printf)("TCP input stats:\r\n");
  show_stat ("total:",       tcpstats.tcps_rcvtotal);
  show_stat ("drops:",       tcpstats.tcps_drops);
  show_stat ("badsum:",      tcpstats.tcps_rcvbadsum);
  show_stat ("con-attempt:", tcpstats.tcps_connattempt);
  show_stat ("con-drops:",   tcpstats.tcps_conndrops);
  show_stat ("OOB pkt:",     tcpstats.tcps_rcvoopack);
  show_stat ("OOB byte:",    tcpstats.tcps_rcvoobyte);
  show_stat ("ACK pkt:",     tcpstats.tcps_rcvackpack);
  show_stat ("pers-drop:",   tcpstats.tcps_persistdrop);

  (*_printf)("TCP output stats:\r\n");
  show_stat ("total:",       tcpstats.tcps_sndtotal);
  show_stat ("data pkt:",    tcpstats.tcps_sndpack);
  show_stat ("data byte:",   tcpstats.tcps_sndbyte);
  show_stat ("SYN/FIN/RST:", tcpstats.tcps_sndctrl);
  show_stat ("OOB pkt:",     tcpstats.tcps_sndurg);
  show_stat ("ACK pkt:",     tcpstats.tcps_sndacks);
  show_stat ("ACK dly:",     tcpstats.tcps_delack);
  show_stat ("retrans to:",  tcpstats.tcps_rexmttimeo);
  show_stat ("keepalive pr:",tcpstats.tcps_keepprobe);
  show_stat ("keepalive to:",tcpstats.tcps_keeptimeo);
}

void print_all_stats (void)
{
  print_mac_stats();
  print_arp_stats();
  print_ip_stats();
  print_icmp_stats();
#if defined(USE_MULTICAST)
  print_igmp_stats();
#endif
  print_udp_stats();
  print_tcp_stats();
}

/*
 * Called from the link-layer input routine _eth_arrived().
 */
void update_in_stat (const void *pkt, WORD proto)
{
  const in_Header *ip;

  if (proto == ARP_TYPE)
  {
    const arp_Header *arp = (const arp_Header*) pkt;

    macstats.non_ip_recv++;
    if (arp->opcode == ARP_REQUEST)
       macstats.arp.request_recv++;
    else if (arp->opcode == ARP_REPLY)
       macstats.arp.reply_recv++;
    return;
  }

#if defined(USE_RARP)
  if (proto == RARP_TYPE)
  {
    const rarp_Header *rarp = (const rarp_Header*) pkt;

    macstats.non_ip_recv++;

    if (rarp->opcode == RARP_REQUEST)
       macstats.rarp.request_recv++;
    else if (rarp->opcode == RARP_REPLY)
       macstats.rarp.reply_recv++;
    return;
  }
#endif

#if defined(USE_PPPOE)
  if (proto == PPPOE_DISC_TYPE || proto == PPPOE_SESS_TYPE)
  {
    if (proto == PPPOE_DISC_TYPE)
       pppoestats.num_disc_recv++;
    else if (proto == PPPOE_SESS_TYPE)
       pppoestats.num_sess_recv++;
    macstats.non_ip_recv++;
    return;
  }
#endif

  if (proto != IP_TYPE)    /* should never happen */
  {
    macstats.non_ip_recv++;
    return;
  }

  ipstats.ips_total++;
  ip = (const in_Header*) pkt;

  switch (ip->proto)
  {
    case ICMP_PROTO:  /* counted in pcicmp.c */
         return;

    case IGMP_PROTO:
         igmpstats.igps_rcv_total++;
         return;

    case UDP_PROTO:
         udpstats.udps_ipackets++;
         return;

    case TCP_PROTO:
         tcpstats.tcps_rcvtotal++;
         return;
  }
}

/*
 * Called from the link-layer output routine _eth_send().
 */
void update_out_stat (const void *pkt, WORD proto)
{
  unsigned   hlen;
  DWORD      ofs;
  in_Header *ip;

  if (proto == ARP_TYPE)
  {
    const arp_Header *arp = (arp_Header*) pkt;

    macstats.non_ip_sent++;
    if (arp->opcode == ARP_REQUEST)
       macstats.arp.request_sent++;
    else if (arp->opcode == ARP_REPLY)
       macstats.arp.reply_sent++;
    return;
  }

#if defined(USE_RARP)
  if (proto == RARP_TYPE)
  {
    const rarp_Header *rarp = (rarp_Header*) pkt;

    macstats.non_ip_sent++;
    if (rarp->opcode == RARP_REQUEST)
       macstats.rarp.request_sent++;
    else if (rarp->opcode == RARP_REPLY)
       macstats.rarp.reply_sent++;
    return;
  }
#endif

#if defined(USE_PPPOE)
  if (proto == PPPOE_DISC_TYPE)
  {
    pppoestats.num_disc_sent++;
    return;
  }
  if (proto == PPPOE_SESS_TYPE)
  {
    pppoestats.num_sess_sent++;
    return;
  }
#endif

  if (proto != IP_TYPE)    /* should never happen */
  {
    macstats.non_ip_sent++;
    return; 
  }

  ipstats.ips_rawout++;   /* count raw IP (fragmented or not) */

  ip  = (in_Header*) pkt;
  ofs = intel16 (ip->frag_ofs);
  ofs = (ofs & IP_OFFMASK) << 3;
   
  if (ofs)       /* don't count each fragment (only with ofs 0) */
     return;  

  hlen = in_GetHdrLen (ip);

  if (ip->proto == ICMP_PROTO)
  {
    const ICMP_PKT *icmp = (const ICMP_PKT*) ((BYTE*)ip + hlen);
    BYTE            type = icmp->unused.type;

    if (type <= ICMP_MAXTYPE)
       icmpstats.icps_outhist[(int)type]++;
    return;
  }

  if (ip->proto == IGMP_PROTO)
  {
    const IGMP_packet *igmp = (const IGMP_packet*) ((BYTE*)ip + hlen);

    if (igmp->type == IGMP_REPORT)
       igmpstats.igps_snd_reports++;
    return;
  }

  if (ip->proto == UDP_PROTO)
  {
    udpstats.udps_opackets++;
    return;
  }

  if (ip->proto == TCP_PROTO)
  {
    tcp_Header *tcp   = (tcp_Header*) ((BYTE*)ip + hlen);
    BYTE        flags = tcp->flags & tcp_FlagMASK;
    int         tlen  = intel16 (ip->length) - hlen - (tcp->offset << 2);

    tcpstats.tcps_sndtotal++;

    if (tlen > 0)                 /* segments with data */
    {
      tcpstats.tcps_sndpack++;
      tcpstats.tcps_sndbyte += tlen;
    }
    if (flags & (tcp_FlagSYN|tcp_FlagFIN|tcp_FlagRST))
       tcpstats.tcps_sndctrl++;
    else if (flags & tcp_FlagACK) /* ACK only */
       tcpstats.tcps_sndacks++;
    if (flags & tcp_FlagURG)
       tcpstats.tcps_sndurg++;
    return;
  }
}
#endif  /* USE_STATISTICS */


