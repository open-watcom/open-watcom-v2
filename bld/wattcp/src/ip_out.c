/*
 * IP output routines
 *
 *  Version
 *
 *  1.0 : Apr 18, 1999 : G. Vanem - created
 *  1.1 : Jan 29, 2000 : G. Vanem - added functions for
 *                       handling list of IP-fragments.
 */

#include "socket.h"

int _default_ttl     = 254;
int _default_tos     = 0;
int _ip_id_increment = 1;   /* some stacks use 5 */

static WORD ip_id = 0;

/*
 *  Increment IP-identifier before returning it.
 *
 *  Note: Use _get_this_ip_id() for sending IP fragments.
 */
WORD _get_ip_id (void)
{
  ip_id += _ip_id_increment;
  return intel16 (ip_id);
}

/*
 *  Return IP-identifier but don't increment it.
 */
WORD _get_this_ip_id (void)
{
  return intel16 (ip_id);
}

int _ip_output (in_Header  *ip,        /* ip-structure to fill in */
                DWORD       src_ip,    /* from address (network order!) */
                DWORD       dst_ip,    /* dest address (network order!) */
                WORD        protocol,  /* IP-protocol number */
                BYTE        ttl,       /* Time To Live */
                BYTE        tos,       /* Type Of Service, 0 unspecified */
                WORD        ip_id,     /* IP identification, normally 0 */
                int         data_len,  /* length of data after ip header */
                const void *sock,      /* which socket is this */
                const char *file,      /* Debug: from what file */
                unsigned    line)      /*  and line was _ip_output called */
{
  int len = sizeof(*ip) + data_len;
 
 /*
  * Note: the 'ip->frag_ofs' field isn't set here. Normaly it's cleared
  *       in eth_formatpacket(). If sending fragments its set in
  *       send_ip_fragments() below.
  */

  if (src_ip == 0)
      src_ip = intel (my_ip_addr);

  ip->ver            = 4;
  ip->hdrlen         = sizeof(*ip) / 4;
  ip->length         = intel16 (len);
  ip->tos            = tos;
  ip->ttl            = ttl   ? ttl   : _default_ttl;
  ip->identification = ip_id ? ip_id : _get_ip_id();
  ip->proto          = protocol;
  ip->source         = src_ip;
  ip->destination    = dst_ip;
  ip->checksum       = 0;
  ip->checksum       = ~checksum (ip, sizeof(*ip));

#if defined(USE_DEBUG)
  if (_dbugxmit)
    (*_dbugxmit) (sock, ip, file, line);
#else
  ARGSUSED (sock);
  ARGSUSED (file);
  ARGSUSED (line);
#endif

  return _eth_send (len);
}

/*
 * Should belong in ip_in.c (not yet)
 *
 *  Check for correct IP-version, header length and header checksum.
 *  If failed, discard the packet. ref. RFC1122: section 3.1.2.2
 */
int _chk_ip_header (const in_Header *ip)
{
  unsigned ip_hlen = in_GetHdrLen (ip);

  if (ip->ver != 4)    /* We only speak IPv4 */
  {
    DEBUG_RX (NULL, ip);
    STAT (ipstats.ips_badvers++);
    return (0);
  }          
  if (ip_hlen < sizeof(*ip))
  {
    DEBUG_RX (NULL, ip);
    STAT (ipstats.ips_tooshort++);
    return (0);
  }          
  if (checksum(ip,ip_hlen) != 0xFFFF)
  {
    DEBUG_RX (NULL, ip);
    STAT (ipstats.ips_badsum++);
    return (0);
  }
  return (1);
}

#if defined(USE_FRAGMENTS)

static __inline in_Header *make_tcp_pkt (const tcp_Socket *sk,
                                         BOOL first, char **data)
{
  in_Header *ip = (in_Header*) _eth_formatpacket (&sk->hisethaddr, IP_TYPE);

  if (first)
  {
    tcp_Header      *tcp = (tcp_Header*) (ip+1);
    tcp_PseudoHeader ph;
    int tcp_len = sizeof(*tcp);

    tcp->srcPort  = intel16 (sk->myport);
    tcp->dstPort  = intel16 (sk->hisport);
    tcp->seqnum   = intel (sk->seqnum + sk->unacked);
    tcp->acknum   = intel (sk->acknum);
    tcp->window   = intel16 (sk->maxrdatalen - sk->rdatalen);
    tcp->flags    = sk->flags;
    tcp->unused   = 0;
    tcp->checksum = 0;
    tcp->urgent   = 0;
    tcp->offset   = tcp_len/4;

    memset (&ph, 0, sizeof(ph));
    ph.src      = intel (sk->myaddr);
    ph.dst      = intel (sk->hisaddr);
    ph.protocol = TCP_PROTO;
    ph.length   = intel16 (tcp_len);
    ph.checksum = checksum (tcp, tcp_len);
    tcp->checksum = ~checksum (&ph, sizeof(ph));
    *data = (char*)tcp + sizeof(*tcp);
  }
  else
    *data = (char*)(ip+1);

  return (ip);
}

static __inline in_Header *make_udp_pkt (const udp_Socket *sk, BOOL first,
                                         int len, char **data)
{
  in_Header *ip = (in_Header*) _eth_formatpacket (&sk->hisethaddr, IP_TYPE);

  if (first)
  {
    udp_Header      *udp = (udp_Header*) (ip+1);
    tcp_PseudoHeader ph;

    udp->srcPort  = intel16 (sk->myport);
    udp->dstPort  = intel16 (sk->hisport);
    udp->checksum = 0;
    udp->length   = intel16 (sizeof(*udp)+len);
    memset (&ph, 0, sizeof(ph));
    ph.src = intel (sk->myaddr);
    ph.dst = intel (sk->hisaddr);

    if (!(sk->sockmode & UDP_MODE_NOCHK))
    {
      ph.protocol = UDP_PROTO;
      ph.length   = udp->length;
      ph.checksum = checksum (udp, sizeof(*udp)) + checksum (*data, len);
      udp->checksum = ~checksum (&ph, sizeof(ph));
    }
    *data = (char*)udp + sizeof(*udp);
  }
  else
    *data = (char*)(ip+1);

  return (ip);
}


#define FRAG_CHUNK_SIZE 128 //  ((mtu/8) << 3)

int send_ip_fragments (sock_type *sk, WORD proto, DWORD dest,
                       const void *buf, int len,
                       const char *file, unsigned line)
{
  int  i, num_frags, trans_size, rc = 0;
  int  frag_size, frag_ofs;
  WORD ip_id;

  num_frags = len / FRAG_CHUNK_SIZE;
  if (len % FRAG_CHUNK_SIZE)
     num_frags++;

  /* Increment IP-identifier. Use same value for all fragments
   */
  ip_id = _get_ip_id();

  for (frag_ofs = i = 0; i < num_frags; i++)
  {
    in_Header *ip   = NULL;
    char      *data = (char*) buf;

    switch (proto)
    {
      case UDP_PROTO:
           ip = make_udp_pkt (&sk->udp, frag_ofs == 0, len, &data);
           break;

      case TCP_PROTO:
           ip = make_tcp_pkt (&sk->tcp, frag_ofs == 0, &data);
           break;

      default:
           fprintf (stderr, "Illegal protocol %04X at %s(%d)\n",
                    proto, __FILE__, __LINE__);
           exit (1);
    }

    trans_size = 0;          /* size of transport header+options */

    if (i == num_frags-1)    /* last fragment */
    {
      frag_size    = ((len+7)/8) << 3;
      ip->frag_ofs = intel16 (frag_ofs/8);
      memset (data+frag_size-8, 0, len);
    }
    else
    {
      frag_size    = FRAG_CHUNK_SIZE;
      ip->frag_ofs = intel16 (frag_ofs/8 | IP_MF);
#if 0
      if (i == 0)
      {
        trans_size = (proto == UDP_PROTO) ? sizeof(udp_Header)
                                          : sizeof(tcp_Header);
        frag_size -= trans_size;
      }
#endif
    }

    memcpy (data, (const char*)buf + frag_ofs, frag_size);

    if (_ip_output (ip, 0, dest, proto, 0, 0, ip_id, trans_size + frag_size,
                    sk, file, line) <= 0)
    {
      SOCK_DEBUGF ((NULL, ", ENETDOWN"));
      SOCK_ERR (ENETDOWN);
      rc = -1;
      break;
    }
    STAT (ipstats.ips_ofragments++);

    frag_ofs += frag_size;
    len      -= frag_size;
    rc       += frag_size;
  }

  tcp_tick (NULL);  /* !! for viewing loopback trace */
  return (rc);
}
#endif /* USE_FRAGMENTS */

