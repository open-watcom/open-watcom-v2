/*
 *  WatTCP protocol debugger. Writes to `debug.file' specified in
 *  config-file. File may be stdout/stderr/nul.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>
#include <io.h>

#ifdef __DJGPP__
#include <unistd.h>
#endif

#include "wattcp.h"
#include "strings.h"
#include "udp_dom.h"
#include "misc.h"
#include "sock_ini.h"
#include "chksum.h"
#include "pctcp.h"
#include "pcbsd.h"
#include "pcsed.h"
#include "pcarp.h"
#include "pcqueue.h"
#include "pcpkt.h"
#include "pcstat.h"
#include "pcconfig.h"
#include "pcicmp.h"
#include "pppoe.h"
#include "pcdbug.h"

#define DEBUG_DNS 1       /* 1 = include detailed DNS debugging */
#define DEBUG_RTP 1       /* 1 = include detailed RTP debugging, to-do */

/*
 * tcpState[] is also used in sock_dbu.c/sock_dat.c
 */
const char *tcpState[] = {
           "LISTEN",   "SYNSENT", "SYNREC", "ESTAB",
           "ESTCLOSE", "FINWT1",  "FINWT2", "CLOSWT",
           "CLOSING",  "LASTACK", "TIMEWT", "CLOSED"
         };

#if defined(USE_DEBUG)

#if (DEBUG_DNS)
#include <arpa/nameser.h>
#include <resolv.h>
#endif

/*@-observertrans@*/

/*
 * We don't use language translation in printouts (only in the ourinit()
 * routine). That would make debug-dumps / problem-reports difficult.
 */

#ifdef __HIGHC__          /* disable stack-checking here */
#pragma Off (check_stack)
#pragma stack_size_warn (220000)
#endif

#ifdef __WATCOMC__
#pragma Off (check_stack)
#endif

#ifdef __TURBOC__
  #ifndef OLD_TURBOC
  #pragma option -N-
  #endif
extern unsigned _stklen = 20000;
#endif

#if (DOSX) && defined(USE_FRAGMENTS)
#define STK_BUF_SIZE  200000
#else
#define STK_BUF_SIZE  8500
#endif

#ifndef STDOUT_FILENO
#define STDOUT_FILENO 1
#endif

static int db_fprintf (const char *format, ...)
#ifdef __GNUC__
  __attribute__((format(printf,1,2)))
#endif
;

static int  db_write    (const char *buf);
static void db_putc     (int ch);
static void db_flush    (void);
static void DumpData    (const BYTE *data, unsigned datalen);
static void DumpOptions (int is_ip, const BYTE *opt, int len);

#if (DEBUG_DNS)
  static void  dns_dump    (const BYTE *, unsigned);
  static BYTE *dns_resource(BYTE *, BYTE *, BYTE *);
  static BYTE *dns_labels  (BYTE *, BYTE *, BYTE *);
#endif

static void (*prev_hook) (const char*, const char*) = NULL;
static char  debugName [128] = "WATTCP.DBG";
static char  ip_src [20];
static char  ip_dst [20];
static int   handle = -1;
static DWORD now;         /* ticks or milli-sec      */
static int   outbound;    /* transmitting packet?    */
static int   is_frag;     /* is this a fragment?     */
static int   first_frag;  /* is this 1st fragment?   */
static int   last_frag;   /* is this last fragment?  */
static char *op;          /* out-buffer pointer      */
static char *op_min;      /* first position for 'op' */
static char *op_max;      /* last position for 'op'  */

static struct {
       char MAC;
       char ARP;
       char RARP;
       char IP;
       char BCAST;
     } filter = { 0,0,0,0,0 };

static struct {
       char MAC;
       char ARP;
       char RARP;
       char IP;
       char UDP;
       char TCP;
       char ICMP;
       char IGMP;
     } debug = { 0,1,1,1,1,1,1,1 };

/* These are public so they can be set by application if
 * running without a config-file
 */
BOOL dbg_mode_all    = 1;
BOOL dbg_print_stat  = 1;
BOOL dbg_dns_details = DEBUG_DNS;
BOOL dbg_rtp_details = DEBUG_RTP;   /* !!to-do */


/*----------------------------------------------------------------------*/

/*
 * Other link-layer drivers (ppp/capi/pktdrvr32) can write to same file,
 * but NOT close it.
 */
const int dbug_handle (void)
{
  return (handle);
}

void dbug_open (void)
{
  if (handle < 0)
  {
    if (!stricmp(debugName,"con"))
       handle = STDOUT_FILENO;
    else
    {
      int mode = _fmode;
      _fmode = O_BINARY;    /* Borland defaults to O_TEXT */
      handle = creat (debugName, S_IWRITE);
      _fmode = mode;
    }
    if (handle < 0)
    {
      outsnl ("ERROR: unable to open debug file!");
      exit (3);
    }
  }
}

/*
 * Return TRUE if MAC destination address of received/sent link-layer
 * packet:
 *  - matches our link-layer address.
 *  - is broadcast and we don't filter broadcast.
 */
static __inline int MatchLinkDestination (const mac_address *dst)
{
  if (!memcmp(dst, &_eth_addr, sizeof(_eth_addr)) ||
      (!filter.BCAST && !memcmp(dst, &_eth_brdcast, sizeof(_eth_brdcast))))
     return (1);
  return (0);
}

/*
 * Return TRUE if destination address of received/sent ARP packet:
 *  - matches our ether-address.
 *  - is broadcast and we don't filter broadcast.
 */
static __inline int MatchArpRarp (const arp_Header *arp)
{
  if (!memcmp(&arp->dstEthAddr, &_eth_addr, sizeof(_eth_addr)))
     return (1);
  if (!filter.BCAST &&
      !memcmp(&arp->dstEthAddr, &_eth_brdcast, sizeof(_eth_brdcast)))
     return (1);
  return (0);
}

/*
 * Return TRUE if destination address of received/sent IP packet:
 *  - matches our IP-address.
 *  - is broadcast and we don't filter (directed) IP-broadcast.
 */
static __inline int MatchIpDest (const in_Header *ip)
{
  DWORD destin = intel (ip->destination);

  if (is_local_addr(destin) ||
      (!filter.BCAST && is_ip_brdcast(ip)))
     return (1);
  return (0);
}

/*
 * Return checksum and print "ok" or "ERROR"
 */
static __inline const char *DoCheckSum (WORD value, const void *p, int len)
{
  static char buf[20];
  sprintf (buf, "%04X (%s)", value,
           checksum(p,len) == 0xFFFF ? "ok" : "ERROR");
  return (buf);
}

/*
 * Return name of some known link-layer protocols.
 */
static __inline const char *LinkProtocol (WORD type)
{
  switch (intel16(type))
  {
    case IP_TYPE:   return ("IP");
    case ARP_TYPE:  return ("ARP");
    case RARP_TYPE: return ("RARP");
    default:        return ("unknown");
  }
}

/*
 * Return name of known IP-protocols.
 */
static __inline const char *IpProtocol (WORD prot)
{
  switch (prot)
  {
    case UDP_PROTO:  return ("UDP");
    case TCP_PROTO:  return ("TCP");
    case ICMP_PROTO: return ("ICMP");
    case IGMP_PROTO: return ("IGMP");
    default:         return ("unknown");
  }
}

/*
 * Return name for IP's "Type Of Service".
 */
static __inline const char *TypeOfService (BYTE tos)
{
  static char buf[20];

  buf[0] = 0;
  if (tos & 0x04) strcat (buf," Rel");
  if (tos & 0x08) strcat (buf," ThPut");
  if (tos & 0x10) strcat (buf," LwDly");
  if (buf[0] == 0)
     sprintf (buf, " %d", tos);
  return (buf);
}

/*
 * Format time for "Round Trip Time".
 */
static __inline const char *RTT_str (DWORD delta)
{
  return (delta > 0UL ? time_str(delta) : "--");
}

/*
 * Return string for ARP/RARP opcodes.
 */
static __inline const char *ArpOpcode (WORD code)
{
  if (code == ARP_REQUEST || code == RARP_REQUEST)
     return ("Request");

  if (code == ARP_REPLY || code == RARP_REPLY)
     return ("Reply");

  return ("? op");
}

/*
 * Return hexa-decimal string for an 6 byte MAC-address.
 * Use 2 buffers in round-robin.
 */
static __inline const char *MAC_addr (const void *adr)
{
  static char buf[2][20];
  static char idx = 0;
  char  *rc = buf [(int)idx];
  char  *a  = (char*)adr;

  sprintf (rc, "%02X:%02X:%02X:%02X:%02X:%02X",
           a[0] & 255, a[1] & 255, a[2] & 255,
           a[3] & 255, a[4] & 255, a[5] & 255);
  idx ^= 1;
  return (rc);
}

/*
 * Print IP source/destination addresses and ports;
 *   "host1 (a) -> host2 (b)"
 */
static void DumpAdrPort (const char       *proto,
                         const tcp_Socket *sock,
                         const in_Header  *ip)
{
  const tcp_Header *tcp = (const tcp_Header*) ((BYTE*)ip + in_GetHdrLen(ip));
  char  dst[20];
  char  src[20];

  if (!sock)
       db_fprintf ("%s:  NO SOCKET : %s (%d) -> %s (%d)\n", proto,
                   ip_src, intel16(tcp->srcPort),
                   ip_dst, intel16(tcp->dstPort));

  else if (outbound)
       db_fprintf ("%s:  %s (%d) -> %s (%d)\n", proto,
                   _inet_ntoa(src,my_ip_addr),   sock->myport,
                   _inet_ntoa(dst,sock->hisaddr),sock->hisport);

  else db_fprintf ("%s:  %s (%d) -> %s (%d)\n", proto,
                   _inet_ntoa(src,sock->hisaddr),sock->hisport,
                   _inet_ntoa(dst,my_ip_addr),   sock->myport);
}

/*----------------------------------------------------------------------*/

static int link_head_dump (const union link_Packet *pkt)
{
  WORD type;
  int  len;

  if (_pktdevclass == PD_TOKEN)
  {
    const struct tok_Header *tok = &pkt->tok.head;

    if (filter.MAC && !outbound && !MatchLinkDestination(&tok->destination))
       return (0);

    db_fprintf ("TR:   destin %s, AC %02X, FC %02X\r\n"
                "      source %s, DSAP %02X, SSAP %02X, Ctrl %02X\r\n",
                MAC_addr (&tok->destination), tok->accessCtrl, tok->frameCtrl,
                MAC_addr (&tok->source), tok->DSAP, tok->SSAP, tok->ctrl);
    type = intel16 (tok->type);
    len  = db_fprintf ("      type %s (%04X)", LinkProtocol(type), type);
  }
  else if (_pktdevclass == PD_FDDI)
  {
    const struct fddi_Header *fddi = &pkt->fddi.head;

    if (filter.MAC && !outbound && !MatchLinkDestination(&fddi->destination))
       return (0);

    db_fprintf ("FDDI: destin %s, FC %02X\r\n"
                "      source %s, DSAP %02X, SSAP %02X, Ctrl %02X\r\n",
                MAC_addr (&fddi->destination), fddi->frameCtrl,
                MAC_addr (&fddi->source), fddi->DSAP, fddi->SSAP, fddi->ctrl);
    type = intel16 (fddi->type);
    len  = db_fprintf ("      type %s (%04X)", LinkProtocol(type), type);
  }
  else  /* PD_ETHER */
  {
    const struct eth_Header *eth = &pkt->eth.head;

    if (filter.MAC && !outbound && !MatchLinkDestination(&eth->destination))
       return (0);

    db_fprintf ("ETH:  destin %s\r\n"
                "      source %s\r\n",
                MAC_addr (&eth->destination),
                MAC_addr (&eth->source));
    type = intel16 (eth->type);

    if (type < ETH_MAX_DATA)  /* LLC length field */
         len = db_fprintf ("      IEEE 802.3 encap (LLC) not supported");
    else len = db_fprintf ("      type %s (%04X)", LinkProtocol(type), type);

  }
  db_putc ('\n');
  return (len);
}

/*----------------------------------------------------------------------*/

static void dump_arp_data (void)
{
  char buf[30];
  int  i, printed;

  db_fprintf ("Routing data:\r\n");
  for (i = 0; i < arp_last_gateway; i++)
  {
    struct gate_table *gw = arp_gate_list + i;
  
    db_write ("      ");
    db_fprintf ("network: %-15s ", _inet_ntoa(buf, gw->subnet));
    db_fprintf ("router: %-15s ",  _inet_ntoa(buf, gw->gate_ip));
    db_fprintf ("mask: %s\n", _inet_ntoa(buf, gw->mask ? gw->mask : sin_mask));
  }

  for (i = printed = 0; i < MAX_ARP_DATA; i++)
  {
    struct arp_table *arp = arp_cache_data + i;

    if (arp->flags >= ARP_FLAG_FOUND)
    {
      if (!printed)
         db_fprintf ("ARP Cache:\r\n");
      printed = 1;

      db_fprintf ("      IP: %-15s -> %s, expires: ",
                  _inet_ntoa(buf, arp->ip), MAC_addr (&arp->hardware));

      if (arp->flags == ARP_FLAG_FIXED)
           db_write ("never");
      else if (arp->expiry > now)
           db_fprintf ("%ss", time_str(arp->expiry - now));
      else db_write ("yes");
      db_putc ('\n');
    }
  }
  if (_bootpon || _dhcpon || _rarpon)
     db_write ("      Above routing data may be overridden by "
               "DHCP/BOOTP/RARP\r\n");
  db_putc ('\n');
}

/*----------------------------------------------------------------------*/

static int arp_dump (const arp_Header *arp)
{
  if (filter.ARP && !outbound && !MatchArpRarp(arp))
     return (0);

  _inet_ntoa (ip_src, intel(arp->srcIPAddr));
  _inet_ntoa (ip_dst, intel(arp->dstIPAddr));

  return db_fprintf ("ARP:  %s (%d), hw %04X, type %04X\r\n"
                     "      %s (%s) -> %s (%s)\r\n",
                     ArpOpcode(arp->opcode), intel16(arp->opcode),
                     arp->hwType, intel16(arp->protType),
                     MAC_addr (&arp->srcEthAddr), ip_src,
                     MAC_addr (&arp->dstEthAddr), ip_dst);
}

/*----------------------------------------------------------------------*/

static int rarp_dump (const rarp_Header *rarp)
{
  if (filter.RARP && !outbound && !MatchArpRarp(rarp))
     return (0);

  _inet_ntoa (ip_src, intel(rarp->srcIPAddr));
  _inet_ntoa (ip_dst, intel(rarp->dstIPAddr));

  return db_fprintf ("RARP: %s (%d), hw %04X, type %04X\r\n"
                     "      %s (%s) -> %s (%s)\r\n",
                     ArpOpcode(rarp->opcode), intel16(rarp->opcode),
                     rarp->hwType, intel16(rarp->protType),
                     MAC_addr (&rarp->srcEthAddr), ip_src,
                     MAC_addr (&rarp->dstEthAddr), ip_dst);
}

/*----------------------------------------------------------------------*/

static void ip_dump (const in_Header *ip)
{
  WORD  ihl, flg;
  DWORD ofs;
  int   opt_len;

  db_fprintf ("IP:   %s -> %s\r\n", ip_src, ip_dst);

  ihl = in_GetHdrLen (ip);
  if (ihl < sizeof(*ip))
  {
    db_write ("      Bad header\r\n");
    return;
  }

  ofs = intel16 (ip->frag_ofs);
  flg = ofs & ~IP_OFFMASK;
  ofs = (ofs & IP_OFFMASK) << 3;

  db_fprintf ("      IHL %u, ver %u, tos%s, len %u,"
              " ttl %u, prot %s (%d), chksum %s\r\n"
              "      id %04X ofs %lu",
              ihl, (BYTE)ip->ver, TypeOfService(ip->tos), intel16(ip->length),
              (BYTE)ip->ttl, IpProtocol (ip->proto), ip->proto,
              DoCheckSum (ip->checksum, ip, ihl),
              intel16 (ip->identification), ofs);

  if (flg & IP_CE) db_write (", CE");
  if (flg & IP_DF) db_write (", DF");
  if (flg & IP_MF)
  {
    is_frag = TRUE;
    if (ofs == 0)           /* first fragment */
    {
      db_write (", MF");
      first_frag = TRUE;
    }
    else
      db_write (", MF (following header invalid)");
  }
  else if (ofs)             /* last fragment */
  {
    db_write (" (last frag), (following header invalid)");
    is_frag   = TRUE;
    last_frag = TRUE;
  }

  db_putc ('\n');
  opt_len = ihl - sizeof(*ip);
  if (opt_len > 0)
     DumpOptions (1, (const BYTE*)(ip+1), opt_len);
}

/*----------------------------------------------------------------------*/

static int icmp_dump (const in_Header *ip)
{
  WORD  len  = in_GetHdrLen (ip);
  const ICMP_PKT *icmp = (const ICMP_PKT*) ((const BYTE*)ip + len);
  char  buf[100] = "";
  char  mask[20];
  int   type, code;

  len  = intel16 (ip->length) - len;   /* ICMP length */
  type = icmp->unused.type;
  code = icmp->unused.code;

  if (len < sizeof(struct icmp_info))
  {
    db_write ("ICMP: Short header\r\n");
    return (1);
  }

  if (!is_frag)
     switch (type)
     {
       case ICMP_UNREACH:
            if (code < DIM(icmp_unreach_str))
                 sprintf (buf, "%s: %s",
                          icmp_type_str[type], icmp_unreach_str[code]);
            else sprintf (buf, "%s: code %d",
                          icmp_type_str[type], code);
            break;

       case ICMP_TIMXCEED:
            if (code < DIM(icmp_exceed_str))
                 sprintf (buf, "%s: %s",
                          icmp_type_str[type], icmp_exceed_str[code]);
            else sprintf (buf, "%s: code %d",
                          icmp_type_str[type], code);
            break;

       case ICMP_REDIRECT:
            if (code < DIM(icmp_redirect_str))
                  strcpy (buf, icmp_redirect_str[code]);
            else  sprintf (buf, "code %d", code);
            break;

       case ICMP_PARAMPROB:
            if (code)
                 sprintf (buf, "Param prob code %d", code);
            else sprintf (buf, "Param prob at %d", icmp->pointer.pointer);
            break;

       case ICMP_MASKREQ:
       case ICMP_MASKREPLY:
            sprintf (buf, "ICMP %s: %s", icmp_type_str[type],
                     _inet_ntoa(mask, intel(icmp->mask.mask)));
            break;

#if 0 /* to-do: */
       case ICMP_ROUTERADVERT:
       case ICMP_ROUTERSOLICIT:
       case ICMP_TSTAMP:
       case ICMP_TSTAMPREPLY:
       case ICMP_IREQ:
       case ICMP_IREQREPLY:
#endif

       default:
            sprintf (buf, "%s (%d), code %d",
                     type < DIM(icmp_type_str) ?
                       icmp_type_str[type] : "Unknown", type, code);
     }

  if (is_frag)
       db_fprintf ("ICMP: %s -> %s\r\n"
                   "      <%sfragment>\r\n",
                   ip_src, ip_dst, last_frag ? "last " : "");
  else db_fprintf ("ICMP: %s -> %s\r\n"
                   "      %s, chksum %s\r\n",
                   ip_src, ip_dst, buf,
                   DoCheckSum (icmp->unused.checksum, icmp, len));

  if (!is_frag && type == ICMP_PARAMPROB)
  {
    const in_Header *ip2 = &icmp->ip.ip; /* original IP + max 8byte udp/tcp */

    len  = intel16 (ip2->length);
    len  = min (len, sizeof(icmp->ip.ip));
    icmp = (ICMP_PKT*)ip2;
    db_write ("Orig IP:\r\n");  /* !!to-do */
  }
  DumpData ((BYTE*)icmp, len);
  return (1);
}

/*----------------------------------------------------------------------*/

#if defined(USE_MULTICAST)
static int igmp_dump (const in_Header *ip)
{
  WORD  len               = in_GetHdrLen (ip);
  const IGMP_packet *igmp = (const IGMP_packet*) ((const BYTE*)ip + len);
  char  type[20], dest[20];

  len = intel16 (ip->length) - len;   /* IGMP length */

  if (len < sizeof(*igmp))
  {
    db_write ("IGMP: Short header\r\n");
    return (1);
  }

  switch (igmp->type)
  {
    case IGMP_QUERY:
         strcpy (type, "Query");
         break;
    case IGMP_REPORT:
         strcpy (type, "Report");
         break;
    default:
         sprintf (type, "type %d?", igmp->type);
         break;
  }

  db_fprintf ("IGMP: %s, ver %d, chksum %s, addr %s\r\n",
              type, igmp->version,
              DoCheckSum(igmp->checksum, igmp, len),
              _inet_ntoa (dest, intel(igmp->address)));

  DumpData ((BYTE*)igmp, len);
  return (1);
}
#endif  /* USE_MULTICAST */

/*----------------------------------------------------------------------*/

#if defined(USE_PPPOE)
static char *pppoe_get_tag (const BYTE *tag)
{
  static char buf[50];
  char  *p = buf;
  WORD   type = *(WORD*)tag;
  WORD   len  = intel16 (*(WORD*)(tag+2));

  switch (type)
  {
    case PPPOE_TAG_END_LIST:
         return ("end");

    case PPPOE_TAG_SERVICE_NAME:
         sprintf (buf, "service-name `%.*s'", len, tag+PPPOE_TAG_HDR_SIZE);
         break;

    case PPPOE_TAG_AC_NAME:
         sprintf (buf, "AC-name `%*.s'", len, tag+PPPOE_TAG_HDR_SIZE);
         break;

    case PPPOE_TAG_AC_COOKIE:
         p   += sprintf (p, "AC-cookie ");
         tag += PPPOE_TAG_HDR_SIZE;
         while (len && p < buf+sizeof(buf)-1)
         {
           p += sprintf (p, "%02X-", *tag);
           tag++;
           len--;
         }
         *(p-1) = '\0';
         break;

    case PPPOE_TAG_HOST_UNIQ:
         p   += sprintf (p, "host-uniq ");
         tag += PPPOE_TAG_HDR_SIZE;
         while (len && p < buf+sizeof(buf)-1)
         {
           p += sprintf (p, "%02X-", *tag);
           tag++;
           len--;
         }
         *(p-1) = '\0';
         break;

    case PPPOE_TAG_VENDOR_SPES:
         sprintf (buf, "vendor spec ID %08lX", *(DWORD*)(tag+4));
         break;

    case PPPOE_TAG_RELAY_SESS:
         sprintf (buf, "relay session %d", *(WORD*)(tag+4));
         break;

    case PPPOE_TAG_HOST_URL:
         sprintf (buf, "host URL `%*.s'", len, tag+PPPOE_TAG_HDR_SIZE);
         break;

    case PPPOE_TAG_MOTM:
         sprintf (buf, "msg-of-the-minute `%*.s'", len, tag+PPPOE_TAG_HDR_SIZE);
         break;

    case PPPOE_TAG_IP_ROUTE_ADD:
         {
           char buf1[20], buf2[20], buf3[20];
           char *dest_net  = _inet_ntoa (buf1, intel(*(DWORD*)(tag+4)));
           char *dest_mask = _inet_ntoa (buf2, intel(*(DWORD*)(tag+8)));
           char *gateway   = _inet_ntoa (buf3, intel(*(DWORD*)(tag+12)));
           DWORD metric    = intel (*(DWORD*)(tag+16));

           sprintf ("route add: %s %s %s / %d",
                    dest_net, dest_mask, gateway, metric);
         }
         break;

    case PPPOE_TAG_SERVICE_ERR:
         return ("service error");

    case PPPOE_TAG_AC_SYSTEM_ERR:
         return ("AC-system err");

    case PPPOE_TAG_GENERIC_ERR:
         return ("generic error");

    default:
         sprintf (buf, "unknown %04X", type);
         break;
  }
  return (buf);
}

static char *pppoe_get_code (WORD code)
{
  switch (code)
  {
    case PPPOE_CODE_PADI:
         return ("PADI");
    case PPPOE_CODE_PADO:
         return ("PADO");
    case PPPOE_CODE_PADR:
         return ("PADR");
    case PPPOE_CODE_PADS:
         return ("PADS");
    case PPPOE_CODE_PADT:
         return ("PADT");
    case PPPOE_CODE_PADM:
         return ("PADM");
    case PPPOE_CODE_PADN:
         return ("PADN");
    default:
         return ("??");
  }
}

static __inline int pppoe_head_dump (const struct pppoe_Header *pppoe,
                                     const char *proto)
{
  if (pppoe->ver != 1 || pppoe->type != 1)
  {
    db_write ("PPPOE: bogus header\r\n");
    return (0);
  }
  db_fprintf ("PPPOE: %s, %s (%04X), session %d\r\n",
              proto, pppoe_get_code(pppoe->code), pppoe->code,
              intel16(pppoe->session));
  return (1);
}

static int pppoe_disc_dump (const struct pppoe_Packet *pppoe)
{
  WORD  tlen = intel16 (pppoe->head.length);
  BYTE *tags;

  if (!pppoe_head_dump(&pppoe->head, "Discovery"))
     return (1);

  tags = (BYTE*)&pppoe->data;
  while (tlen > 0)
  {
    WORD tag_len = intel16 (*(WORD*)(tags+2));

    db_fprintf ("       tag: %s\r\n", pppoe_get_tag(tags));
    tlen -= (PPPOE_TAG_HDR_SIZE + tag_len);
    tags += (PPPOE_TAG_HDR_SIZE + tag_len);
  }
  db_write ("\r\n");
  return (1);
}

static int pppoe_sess_dump (const struct pppoe_Packet *pppoe)
{
  if (!pppoe_head_dump(&pppoe->head, "Session"))
     return (1);
  /* !! to-do */
  return (1);
}
#endif

/*----------------------------------------------------------------------*/

static const char *udp_tcp_chksum (const in_Header  *ip,
                                   const udp_Header *udp,
                                   const tcp_Header *tcp)
{
  tcp_PseudoHeader ph;
  int              len;

  memset (&ph, 0, sizeof(ph));
  if (udp)
  {
    len = intel16 (udp->length);
    ph.protocol = UDP_PROTO;
    ph.checksum = checksum (udp, len);
  }
  else
  {
    len = intel16 (ip->length) - in_GetHdrLen (ip);
    ph.protocol = TCP_PROTO;
    ph.checksum = checksum (tcp, len);
  }

  ph.src    = ip->source;
  ph.dst    = ip->destination;
  ph.length = intel16 (len);

  if (checksum(&ph,sizeof(ph)) == 0xFFFF)
     return ("ok");
  return ("ERROR");
}

/*----------------------------------------------------------------------*/

static int udp_dump (const tcp_Socket *sock, const in_Header *ip)
{
  const char *chk_ok    = "n/a";
  WORD  iplen           = intel16 (ip->length) - sizeof(*ip);
  const udp_Header *udp = (const udp_Header*) ((BYTE*)ip + in_GetHdrLen(ip));
  WORD  udplen          = intel16 (udp->length) - sizeof(*udp);
  const BYTE *data      = (const BYTE*) (udp+1);

  if (udp->checksum && !is_frag)
     chk_ok = udp_tcp_chksum (ip, udp, NULL);

  if (is_frag || udplen > iplen)
     udplen = min (udplen, iplen);

  if (is_frag && !first_frag)
  {
    DumpData ((const BYTE*)udp, iplen);
    return (1);
  }

  DumpAdrPort ("UDP", sock, ip);

  db_fprintf ("      len %d, chksum %04X (%s)\r\n",
              intel16(udp->length), intel16(udp->checksum), chk_ok);

#if (DEBUG_DNS)
  if (dbg_dns_details && udplen > sizeof(struct dhead) &&
      ((!outbound && udp->srcPort == intel16(DOM_DST_PORT)) ||
       ( outbound && udp->dstPort == intel16(DOM_DST_PORT))))
  {
    db_flush();
    dns_dump (data, udplen);
  }
  else
#endif

  DumpData (data, udplen);
  return (1);
} 

/*----------------------------------------------------------------------*/

static int tcp_dump (const tcp_Socket *sock, const in_Header *ip)
{
  WORD   iplen           = intel16 (ip->length) - sizeof(*ip);
  int    j               = in_GetHdrLen (ip);
  const  tcp_Header *tcp = (const tcp_Header*) ((const BYTE*)ip + j);
  int    i               = tcp->offset << 2;
  WORD   dlen            = intel16 (ip->length) - j - i;
  WORD   olen            = 0;
  const  BYTE *data      = (const BYTE*) tcp + i;
  DWORD  win             = intel16 (tcp->window);
  DWORD  ack, seq;
  char   flgBuf [4*6+1]  = { 0 };
  const  char *chk_ok;

  if (is_frag || dlen > iplen)
     dlen = min (dlen, iplen);

  if (i < sizeof(*tcp))
  {
    db_write ("      Bad header\r\n");
    return (0);
  }

  if (is_frag && !first_frag)
  {
    DumpData ((const BYTE*)tcp, iplen);
    return (1);
  }

  if (tcp->flags & tcp_FlagACK)  strcat (flgBuf, " ACK");
  if (tcp->flags & tcp_FlagFIN)  strcat (flgBuf, " FIN");
  if (tcp->flags & tcp_FlagSYN)  strcat (flgBuf, " SYN");
  if (tcp->flags & tcp_FlagPUSH) strcat (flgBuf, " PSH");
  if (tcp->flags & tcp_FlagRST)  strcat (flgBuf, " RST");
  if (tcp->flags & tcp_FlagURG)  strcat (flgBuf, " URG");

  if (is_frag)
       chk_ok = "n/a";
  else chk_ok = udp_tcp_chksum (ip, NULL, tcp);

  DumpAdrPort ("TCP", sock, ip);

  if (sock)
     win <<= outbound ? sock->send_wscale : sock->recv_wscale;

  ack = intel (tcp->acknum);
  seq = intel (tcp->seqnum);

  db_fprintf ("      flags%s, win %lu, chksum %04X (%s), urg %u\r\n"
              "                 SEQ %10lu,  ACK %10lu\r\n",
              flgBuf, win, intel16(tcp->checksum), chk_ok,
              intel16(tcp->urgent), seq, ack);
  if (sock)
  {
    UINT  state = sock->state;
    long  delta_seq, delta_ack;

    if (outbound)
    {
      if (state == tcp_StateESTAB && sock->last_acknum[0] == 0)
           delta_ack = 0;
      else delta_ack = ack - sock->last_acknum[0];

      if (state == tcp_StateSYNSENT && sock->last_seqnum[0] == 0)
           delta_seq = 0;
      else delta_seq = seq - sock->last_seqnum[0];

      ((tcp_Socket*)sock)->last_seqnum[0] = seq;
      ((tcp_Socket*)sock)->last_acknum[0] = ack;
    }
    else
    {
      if (state == tcp_StateLISTEN || sock->last_seqnum[1] == 0)
           delta_seq = 0;
      else delta_seq = seq - sock->last_seqnum[1];

      if (state == tcp_StateSYNSENT && sock->last_acknum[1] == 0)
           delta_ack = 0;
      else delta_ack = ack - sock->last_acknum[1];

      ((tcp_Socket*)sock)->last_seqnum[1] = seq;
      ((tcp_Socket*)sock)->last_acknum[1] = ack;
    }

    db_fprintf ("      %-8.8s (dSEQ %10ld, dACK %10ld), MS %lu/%lu%s\r\n"
                "      KC %d, vjSA %lu, vjSD %lu, CW %d, WW %d, RTO %d, RTTdiff %s\r\n",
                tcpState[state], delta_seq, delta_ack,
                sock->missed_seg[0], sock->missed_seg[1],
                sock->unhappy ? ", Unhappy" : "",
                sock->karn_count, sock->vj_sa, sock->vj_sd,
                sock->cwindow, sock->wwindow, sock->rto,
                RTT_str (sock->rtt_time - now));
  }

  olen = i - sizeof(*tcp);
  if (olen > 0)
  {
    const BYTE *opt = (const BYTE*) (tcp+1);
    DumpOptions (0, opt, olen);
  }

#if (DEBUG_DNS)
  if (dbg_dns_details && dlen > sizeof(struct dhead) &&
      ((!outbound && tcp->srcPort == intel16(DOM_DST_PORT)) ||
       ( outbound && tcp->dstPort == intel16(DOM_DST_PORT))))
  {
    db_flush();
    dns_dump (data, dlen);
  }
  else
#endif

  DumpData (data, dlen);
  return (1);
}

/*----------------------------------------------------------------------*/

static int db_dump (const void *sock, const in_Header *ip,
                    const char *fname, unsigned line)
{
  static int arp_dumped = 0;
  int    len = 0;

  now = set_timeout (0);
  if (!arp_dumped)
  {
    dump_arp_data();
    arp_dumped = 1;
  }

  db_fprintf ("%s %s (%u), time %s\r\n",
              outbound ? "Transmitted:" : "Received:   ",
              fname, line, time_str(now));

  if (!_pktserial)           /* PD_ETHER / PD_TOKEN / PD_FDDI */
  {
    union link_Packet *pkt = (union link_Packet*) MAC_HDR (ip);
    WORD  type = MAC_TYP (ip);

    if (debug.MAC)
       len = link_head_dump (pkt);

    if (type == ARP_TYPE)
    {
      if (debug.ARP)
         return arp_dump ((const arp_Header*)ip);
      return (len);
    }

    if (type == RARP_TYPE)
    {
      if (debug.RARP)
         return rarp_dump ((const rarp_Header*)ip);
      return (len);
    }

#if defined(USE_PPPOE)
    if (type == PPPOE_DISC_TYPE)
    {
      len += pppoe_disc_dump (&pkt->pppoe);
      return (len);
    }
    if (type == PPPOE_SESS_TYPE)
       len += pppoe_sess_dump (&pkt->pppoe);
#endif
  }

  if (filter.IP && !outbound && !MatchIpDest(ip))
     return (len);

  _inet_ntoa (ip_src, intel(ip->source));
  _inet_ntoa (ip_dst, intel(ip->destination));

  if (debug.IP)
  {
    ip_dump (ip);
    len++;
  }

#if defined(USE_MULTICAST)
  if (ip->proto == IGMP_PROTO && debug.IGMP) return igmp_dump (ip);
#endif

  if (ip->proto == ICMP_PROTO && debug.ICMP) return icmp_dump (ip);
  if (ip->proto ==  UDP_PROTO && debug.UDP)  return udp_dump (sock, ip);
  if (ip->proto ==  TCP_PROTO && debug.TCP)  return tcp_dump (sock, ip);

  if (debug.IP)    /* dump unknown IP-network protocol */
  {
    int   len2  = intel16 (ip->length) - in_GetHdrLen(ip);
    BYTE *data = (BYTE*)ip + in_GetHdrLen(ip);
    DumpData (data, min(MAX_IP_DATA,len2));
  }
  return (len);
}

/*----------------------------------------------------------------------*/

static void dbg_send (const void *sock, const void *ip,
                      const char *fname, unsigned line)
{
  char buf [STK_BUF_SIZE];
  op     = buf;
  op_min = buf;
  op_max = buf + STK_BUF_SIZE - 60;

  if (handle < 0)
     return;

#if defined(__LARGE__)
  watt_largecheck (&buf, sizeof(buf), __FILE__, __LINE__);
#endif

  outbound = 1;
  first_frag = FALSE;
  last_frag  = FALSE;
  is_frag    = FALSE;

  if (db_dump(sock,ip,fname,line))
  {
    db_putc ('\n');
    db_flush();
  }
}

static void dbg_recv (const void *sock, const void *ip,
                      const char *fname, unsigned line)
{
  char buf [STK_BUF_SIZE];
  op     = buf;
  op_min = buf;
  op_max = buf + STK_BUF_SIZE - 60;

  if (handle < 0)
     return;

#if defined(__LARGE__)
  watt_largecheck (&buf, sizeof(buf), __FILE__, __LINE__);
#endif

  outbound = 0;
  first_frag = FALSE;
  last_frag  = FALSE;
  is_frag    = FALSE;

  if (db_dump(sock,ip,fname,line))
  {
    db_putc ('\n');
    db_flush();
  }
}


/*
 * Print IP or TCP-options
 */
static void DumpOptions (int is_ip, const BYTE *opt, int len)
{
  const BYTE *start = opt;
  char  buf[20];
  DWORD val;
  int   i, num;

  db_write ("      Options:");

  while (opt < start+len)
  {
    switch (*opt)  /* Note: IP-option includes copy/class bits */
    {
      case TCPOPT_EOL:
   /* case IPOPT_EOL: */
           db_write (" EOL\r\n");
           return;

      case TCPOPT_NOP:
   /* case IPOPT_NOP: */
           db_write (" NOP");
           opt++;
           continue;

      case TCPOPT_MAXSEG:
           db_fprintf (" MSS %u", intel16(*(WORD*)(opt+2)));
           break;

      case TCPOPT_WINDOW:
           db_fprintf (" Wscale %lu", (2L << *(opt+2)));
           break;

      case TCPOPT_SACK:
           db_write (" SACK ");
           num = (*(opt+1) - 2) / 4;
           for (i = 0; i < num; i++)
           {
             DWORD left  = intel (*(DWORD*)(opt+2+4*i));
             DWORD right = intel (*(DWORD*)(opt+6+4*i));
             db_fprintf ("blk %d: %lu/%lu, ", i+1, left, right);
           }
           break;

      case TCPOPT_SACKOK:
           db_write (" SACK-OK");
           break;

      case TCPOPT_ECHO:
           val = intel (*(DWORD*)(opt+2));
           db_fprintf (" Echo %08lX", val);
           break;

      case TCPOPT_ECHOREPLY:
   /* case IPOPT_RR: */
           if (is_ip)
           {
             db_write (" RR");
             for (i = 0; i < *(opt+1) - 3; i += sizeof(DWORD))
             {
               val = intel (*(DWORD*)(opt+3+i));
               db_putc (' ');
               db_write (_inet_ntoa(buf,val));
             }
           }
           else
           {
             val = intel (*(DWORD*)(opt+2));
             db_fprintf (" Echoreply %08lX", val);
           }
           break;

      case TCPOPT_TIMESTAMP:
           db_fprintf (" Timestamp %lu/%lu",
                       intel(*(DWORD*)(opt+2)), intel(*(DWORD*)(opt+6)));
           break;

      case TCPOPT_CC:
           val = intel (*(DWORD*)(opt+2));
           db_fprintf (" CC %08lX", val);
           break;

      case TCPOPT_CCNEW:
           val = intel (*(DWORD*)(opt+2));
           db_fprintf (" CCnew %08lX", val);
           break;

      case TCPOPT_CCECHO:
           val = intel (*(DWORD*)(opt+2));
           db_fprintf (" CCecho %08lX", val);
           break;

      case IPOPT_TS:
           {
             DWORD ip = intel (*(DWORD*)(opt+4));
             DWORD ts = intel (*(DWORD*)(opt+8));
             db_fprintf (" TS %s/%lu..", _inet_ntoa(buf,ip), ts);
           }
           break;

      case IPOPT_SECURITY:
           val = intel (*(DWORD*)(opt+2));
           db_fprintf (" SEC %08lX", val);
           break;

      case IPOPT_SATID:
           db_fprintf (" SATID %04X", intel16(*(WORD*)(opt+2)));
           break;

      case IPOPT_RA:
           db_fprintf (" RA %u", intel16(*(WORD*)(opt+2)));
           break;

      case IPOPT_LSRR:
      case IPOPT_SSRR:
           db_write (*opt == IPOPT_LSRR ? " LSRR" : " SSRR");

           for (i = 0; i < *(opt+1) - 3; i += sizeof(DWORD))
           {
             DWORD route = intel (*(DWORD*)(opt+3+i));
             db_fprintf (" %s", _inet_ntoa(buf,route));
           }
           break;

      default:
           db_fprintf (" opt %d?", *opt);
    }
    opt += *(opt+1);
  }
  db_putc ('\n');
}

/*----------------------------------------------------------------------*/

static void DumpData (const BYTE *data, unsigned datalen)
{
  unsigned ofs;

  if (datalen <= 0 || !dbg_mode_all)
     return;

  for (ofs = 0; (ofs < datalen) && (op < op_max); ofs += 16)
  {
    unsigned j;

    if (ofs == 0)
         db_fprintf ("%4u: %04X: ", datalen, ofs);
    else db_fprintf ("      %04X: ", ofs);

    for (j = 0; j < 16 && j+ofs < datalen; j++)
        db_fprintf ("%02X%c", (unsigned)data[j+ofs], j == 7 ? '-' : ' ');

    for ( ; j < 16; j++)
        db_write ("   ");

    for (j = 0; j < 16 && j+ofs < datalen; j++)
    {
      if (data[j+ofs] < ' ')
           db_putc ('.');
      else db_putc (data[j+ofs]);
    }
#if 0   /* might be reassembled packet */
    if (ofs > mtu-sizeof(in_Header))
    {
      db_write ("\r\nMTU exceeded!?\r\n");
      break;
    }
#endif
    db_putc ('\n');
  }
}

/*----------------------------------------------------------------------*/

static void set_debug_file (const char *value)
{
  strncpy (debugName, value, sizeof(debugName)-2);
  debugName [sizeof(debugName)-1] = 0;
  dbug_open();
}

static void set_debug_mode (const char *value)
{
  if (!strcmp(value,"ALL"))       dbg_mode_all = 1;
  if (!strncmp(value,"HEADER",6)) dbg_mode_all = 0;
}

static void set_debug_filter (const char *value)
{
  if (strstr(value,"ALL"))
     memset (&filter, 1, sizeof(filter));

  else if (strstr(value,"NONE"))
     memset (&filter, 0, sizeof(filter));
  else
  {
    filter.MAC   = (strstr(value,"ETH" )  != NULL) || (strstr(value,"MAC") != NULL);
    filter.ARP   = (strstr(value,"ARP" )  != NULL);
    filter.RARP  = (strstr(value,"RARP")  != NULL);
    filter.IP    = (strstr(value,"IP"  )  != NULL);
    filter.BCAST = (strstr(value,"BCAST") != NULL);
  }
}

static void set_debug_proto (const char *value)
{
  if (!strcmp(value,"ALL"))
     memset (&debug, 1, sizeof(debug));
  else
  {
    memset (&debug, 0, sizeof(debug));
    debug.MAC  = (strstr(value,"ETH" ) != NULL) || (strstr(value,"MAC") != NULL);
    debug.ARP  = (strstr(value,"ARP" ) != NULL);
    debug.RARP = (strstr(value,"RARP") != NULL);
    debug.IP   = (strstr(value,"IP"  ) != NULL);
    debug.TCP  = (strstr(value,"TCP" ) != NULL);
    debug.UDP  = (strstr(value,"UDP" ) != NULL);
    debug.ICMP = (strstr(value,"ICMP") != NULL);
    debug.IGMP = (strstr(value,"IGMP") != NULL);
  }
}

static void ourinit (const char *name, const char *value)
{
  static struct config_table debug_cfg[] = {
                { "FILE",    ARG_FUNC,  (void*)set_debug_file   },
                { "MODE",    ARG_FUNC,  (void*)set_debug_mode   },
                { "FILTER",  ARG_FUNC,  (void*)set_debug_filter },
                { "PROTO",   ARG_FUNC,  (void*)set_debug_proto  },
                { "STAT",    ARG_ATOI,  (void*)&dbg_print_stat  },
                { "DNS",     ARG_ATOI,  (void*)&dbg_dns_details },
                { "RTP",     ARG_ATOI,  (void*)&dbg_rtp_details }, 
                { NULL }
              };
  char val[80];

  strncpy (val, value, sizeof(val)-1);
  val [sizeof(val)-1] = '\0';
  strupr (val);

  if (!parse_config_table(&debug_cfg[0], "DEBUG.", name, val) && prev_hook)
     (*prev_hook) (name, value);
}

#define CHECK_TRUNCATED()                          \
        do {                                       \
          if (op > op_max - 12) {                  \
             strncpy (op,"\r\n<more>\r\n\r\n",12); \
             op += 12;                             \
          }                                        \
        } while (0)


static int db_fprintf (const char *format, ...)
{
  int len = 0;

  if (handle > 0 && op < op_max)
  {
    va_list arg;
    va_start (arg, format);

#if defined(__HIGHC__) || defined(__WATCOMC__)
    len = _vbprintf (op, op_max-op-1, format, arg);
    if (len < 0)
    {
      op  = op_max;
      len = abs (len);
    }
    else
      op += len;
#else
    len = vsprintf (op, format, arg);
    op += len;
#endif
    va_end (arg);
    CHECK_TRUNCATED();
  }
  return (len);
}

void dbug_write_raw (const char *buf)
{
  if (handle > 0)
     write (handle, buf, strlen(buf));
}

static void db_flush (void)
{
  if (handle > 0 && op > op_min)
     write (handle, op_min, op - op_min);
  op = op_min;
}

static int db_write (const char *buf)
{
  int len = strlen (buf);

  if (op+len < op_max)
  {
    memcpy (op, buf, len);
    op += len;
    CHECK_TRUNCATED();
  }
  return (len);
}

static void db_putc (int ch)
{
  if (op+2 < op_max)
  {
    if (ch == '\n')
       *op++ = '\r';
    *op++ = ch;
  }
}

/*
 * Public initialisation
 */
void dbug_init (void)
{
  prev_hook = usr_init;
  usr_init  = ourinit;
  _dbugxmit = dbg_send;
  _dbugrecv = dbg_recv;

#if defined(USE_BSD_FUNC)
  {
    extern void _sock_dbug_init (void); /* in bsddbug.c */
    _sock_dbug_init();
  }
#endif

  /* Give user a warning
   */
  if (_watt_is_init)
  {
    fprintf (stderr, "`dbug_init()' called after `sock_init()'.\n");
    sleep (1);
  }
}

static void dbg_dump_stats (void)
{
#if defined(USE_STATISTICS)
  int (*_save_printf) (const char *fmt,...) = _printf;
  char buf [STK_BUF_SIZE];

  op     = buf;
  op_min = buf;
  op_max = buf + STK_BUF_SIZE - 60;

  _printf = db_fprintf;
  print_all_stats();
  db_flush();
  _printf = _save_printf;
#endif
}

#else   /* USE_DEBUG */

void dbug_init (void)
{
  outsnl ("Debug-mode disabled");
}
#endif


/*
 * dbug_exit - Print statistics counters to debug-file.
 *
 * Should not be called from atexit() function, because order is unspecified.
 * Called from tcp_shutdown() after DHCP_release() and tcp_abort() but
 * before _eth_release() is called. pkt-drop counters are not available
 * once _eth_release() has been called.
 */
void dbug_exit (void)
{
#if defined(USE_DEBUG)
  if (handle > 0)
  {
    if (!_watt_fatal_error && dbg_print_stat)
       dbg_dump_stats();
    close (handle);
    handle = -1;
  }
#endif /* USE_DEBUG */
}


#if defined(USE_DEBUG) && (DEBUG_DNS)
/*
 * Debug of DNS (udp) records
 *
 * Author: Mike Borella <mike_borella@mw.3com.com>
 *
 * Changed for WatTCP and pcdbug.c by G.Vanem 1998 <giva@bgnett.no>
 *
 * !! to-do: parse the SRV resource record (RFC 2052)
 */

struct DNSHdr {
       WORD  dns_id;
       WORD  dns_fl_rcode : 4;
       WORD  dns_fl_zero  : 3;
       WORD  dns_fl_ra    : 1;
       WORD  dns_fl_rd    : 1;
       WORD  dns_fl_tc    : 1;
       WORD  dns_fl_aa    : 1;
       WORD  dns_fl_opcode: 4;
       WORD  dns_fl_qr    : 1;
       WORD  dns_num_q;
       WORD  dns_num_ans;
       WORD  dns_num_auth;
       WORD  dns_num_add;
     };

static char *dns_opcodes[]   = {
            "standard", "inverse", "server status"
          };

static char *dns_responses[] = {
            "no error",   "format error",    "server error",
            "name error", "not implemented", "service refused"
          };

/*
 * dns_query()
 *
 * Return a string describing the numeric value of a DNS query type
 */
#if defined(USE_BIND)
  #define dns_query(type) __p_type(type)
#else
  static __inline char *dns_query (WORD type)
  {
    switch (type)
    {
      case T_A:     return ("A");
      case T_NS:    return ("NS");
      case T_CNAME: return ("CNAME");
      case T_PTR:   return ("PTR");
      case T_HINFO: return ("HINFO");
      case T_MX:    return ("MX");
      case T_AXFR:  return ("AXFR");
      case T_ANY:   return ("ANY");
      default:      return ("??");
    }
  }
#endif

/*
 * dns_dump()
 *
 * Parse DNS packet and dump fields
 */

static void dns_dump (const BYTE *bp, unsigned length)
{
  struct DNSHdr *dns = (struct DNSHdr*) bp;
  BYTE  *p  = (BYTE*)bp + sizeof(*dns);
  BYTE  *ep = (BYTE*)bp + length;
  char  *oc = dns->dns_fl_opcode < DIM(dns_opcodes) ?
                dns_opcodes [dns->dns_fl_opcode] : NULL;
  char  *ra = dns->dns_fl_rcode < DIM(dns_responses) ?
                dns_responses [dns->dns_fl_rcode] : NULL;
  int    i, t;

  db_fprintf ("DNS:  Ident %u, %s, Opcode: %s\r\n",
              intel16(dns->dns_id), dns->dns_fl_qr ? "Response" : "Query",
              oc ? oc : "??");

  db_fprintf ("  auth answer %d, truncated %d, rec-req %d, rec-avail %d,"
              " zero %d, %s\r\n",
              dns->dns_fl_aa, dns->dns_fl_tc, dns->dns_fl_rd, dns->dns_fl_ra,
              dns->dns_fl_zero, ra ? ra : "??");

  if (!op || !ra)
  {
    db_write ("  Looks like a bogus packet\r\n");
    return;
  }

 /* Do the question part of the packet.
  */
  i = intel16 (dns->dns_num_q);

  while (i > 0)
  {
    WORD qtype, qclass;

    db_write ("  Question: query name ");
    p = dns_labels (p, (BYTE*)bp, ep);

    qtype  = intel16 (*(WORD*)p);  p += sizeof(qtype);
    qclass = intel16 (*(WORD*)p);  p += sizeof(qclass);

    db_fprintf ("    query type %d (%s), class %d\r\n",
                qtype, dns_query(qtype), qclass);
    i--;
  }

 /* dump the resource records for the answers
  */
  i = intel16 (dns->dns_num_ans);
  t = i;
  while (i > 0)
  {
    db_fprintf ("  Answer %d: ", t-i+1);
    p = dns_resource (p, (BYTE*)bp, ep);
    i--;
  }

 /* dump the resource records for the authoritative answers
  */
  i = intel16 (dns->dns_num_auth);
  t = i;
  while (i > 0)
  {
    db_fprintf ("  Auth %d: ", t-i+1);
    p = dns_resource (p, (BYTE*)bp, ep);
    i--;
  }

 /* dump the resource records for the additional info
  */
  i = intel16 (dns->dns_num_add);
  t = i;
  while (i > 0)
  {
    db_fprintf ("  Additional %d: ", t-i+1);
    p = dns_resource (p, (BYTE*)bp, ep);
    i--;
  }
}
 
/*
 * dns_resource()
 *
 * Print the contents of a resource record
 */
static BYTE *dns_resource (BYTE *p, BYTE *bp, BYTE *ep)
{
  int   i;
  DWORD ttl;
  WORD  qtype, qclass, reslen;

  db_write ("server name: ");
  p = dns_labels (p, bp, ep);

 /* Do query type, class, ttl and resource length
  */
  qtype  = intel16 (*(WORD*)p);  p += sizeof(qtype);
  qclass = intel16 (*(WORD*)p);  p += sizeof(qclass);
  ttl    = intel  (*(DWORD*)p);  p += sizeof(ttl);
  reslen = intel16 (*(WORD*)p);  p += sizeof(reslen);

  db_fprintf ("    type %d (%s), class %d, ttl %luh, length %d\r\n",
              qtype, dns_query(qtype), qclass, ttl/3600UL, reslen);

 /* Do resource data.
  */
  switch (qtype)
  {
    case T_A:     /* A record; ip address(es) */
         for (i = 1; i <= reslen; i += sizeof(DWORD))
         {
           char buf[20];
           char *ip = _inet_ntoa (buf, intel(*(DWORD*)p));
           db_fprintf ("    IP address: %s\r\n", ip);
           p += sizeof(DWORD);
         }
         break;

    case T_NS:    /* NS record; Name Server */
         db_write ("    auth host: ");
         p = dns_labels (p, bp, ep);
         break;

    case T_CNAME: /* CNAME record; canonical name */
         db_write ("    canon host: ");
         p = dns_labels (p, bp, ep);
         break;

    case T_PTR:   /* PTR record; hostname for IP */
         db_write ("    host name: ");
         p = dns_labels (p, bp, ep);
         break;

    default:
         p += reslen;
  }
  return (p);
}

/*
 * dns_labels()
 *
 * Recursively parse a label entry in a DNS packet
 */

static BYTE *dns_labels (BYTE *p, BYTE *bp, BYTE *ep)
{
  while (1)
  {
    BYTE count = *p++;

    if (count >= 192)
    {
     /* There's a pointer in this label.  Sigh.  Let's grab the
      * 14 low-order bits and run with them...
      */
      WORD offset = ((unsigned)(count - 192) << 8) + *p++;

      dns_labels (bp+offset, bp, ep);
      return (p);
    }
    if (count == 0)
       break;

    while (count > 0)
    {
      if (p <= ep)
         db_putc (*p++);
      else
      {
        db_write ("\nPacket length exceeded");
        goto quit;
      }
      count--;
    }
    db_putc ('.');
  }
quit:
  db_putc ('\n');
  return (p);
}
#endif  /* USE_DEBUG && DEBUG_DNS */
