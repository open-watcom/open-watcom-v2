
/*
 *  Packet De-Fragmentation code for WATTCP
 *  Written by and COPYRIGHT (c)1993 to Quentin Smart
 *                               smart@actrix.gen.nz
 *  all rights reserved.
 *
 *    This software is distributed in the hope that it will be useful,
 *    but without any warranty; without even the implied warranty of
 *    merchantability or fitness for a particular purpose.
 *
 *    You may freely distribute this source code, but if distributed for
 *    financial gain then only executables derived from the source may be
 *    sold.
 *
 *  Murf = Murf@perftech.com
 *  other fragfix = mdurkin@tsoft.net
 *
 *  Based on RFC815
 *
 *  Code used to use pktbuf[] as reassembly buffer. It now allocates
 *  a "bucket" dynamically. There are MAX_IP_FRAGS buckets to handle
 *  at the same time (currently 1).
 *
 *  G.Vanem 1998 <giva@bgnett.no>
 */

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#include "wattcp.h"
#include "strings.h"
#include "language.h"
#include "chksum.h"
#include "ip_out.h"
#include "misc.h"
#include "pcconfig.h"
#include "pcstat.h"
#include "pcqueue.h"
#include "pcsed.h"
#include "pcpkt.h"
#include "pcdbug.h"
#include "pcicmp.h"
#include "pcsed.h"
#include "pcbsd.h"
#include "pctcp.h"

#if defined(USE_FRAGMENTS)

/*@-nullderef@*/

#define MAX_IP_FRAGS    1      /* max # of fragmented IP-packets */
#define FRAGHOLDTIME    15     /* time to hold before discarding */

#define BUCKET_SIZE     (sizeof(HugeIP))  /* ~= 64k bytes */
#define BUCKET_MARKER   0xDEADBEEF

typedef struct huge_ip {
        in_Header hdr;
        BYTE      data [MAX_FRAG_SIZE];   /* 66600 for DOSX */
        DWORD     marker;
      } HugeIP;

typedef struct {
        DWORD  source;
        DWORD  destin;
        WORD   ident;
        BYTE   proto;
        BYTE   tos;
      } frag_key;

typedef struct {
        char        used;     /* this bucket is taken */
        int         active;   /* # of active fragments */
        mac_address mac_src;  /* remember for icmp_timexceed() */
        HugeIP     *ip;       /* malloced, size = (BUCKET_SIZE) */
      } frag_bucket;

typedef struct hd {
        struct hd *next;
        long       start;
        long       end;
      } hole_descr;

typedef struct {
        BYTE        used;     /* this position in table in use */
        frag_key    key;
        DWORD       timer;
        hole_descr *hole_first;
        in_Header  *ip;
        BYTE       *data_offset;
      } frag_hdr;


static frag_hdr    frag_list    [MAX_IP_FRAGS][MAX_FRAGMENTS];
static frag_bucket frag_buckets [MAX_IP_FRAGS];

static long        data_start;
static long        data_end;
static long        data_length;
static int         more_frags;

static int         check_data_start  (const in_Header *ip, DWORD ofs, DWORD len);
static void        setup_first_frag  (const in_Header *ip, int idx, frag_key *key);
static in_Header * alloc_frag_buffer (const in_Header *ip);

#ifdef TEST_PROG
#define MSG(x) printf x
#else
#define MSG(x) ((void)0)
#endif

int _ip_frag_reasm = FRAGHOLDTIME;  /* max time for reassembly */

/*
 * ip_defragment() is called if the frag section of the IP header
 * is non-zero and DF bit not set.
 */
in_Header * ip_defragment (const in_Header *ip, DWORD offset, WORD flags)
{
  frag_hdr   *frag      = NULL;
  hole_descr *hole      = NULL;
  hole_descr *prev_hole = NULL;

  frag_key key;
  int      found    = 0;
  int      got_hole = 0;
  int      i, j;   /* to-do: use j for handling MAX_IP_FRAGS simultaneously */

  STAT (ipstats.ips_fragments++);

#if 0
  MSG (("ip_defrag: src %s, dst %s, prot %d, id %04X\n",
        inet_ntoa (*(struct in_addr*)&ip->source),
        inet_ntoa (*(struct in_addr*)&ip->destination),
        ip->proto, ip->identification));
#endif

  /* Check IP version & header length/checksum
   * !!to-do: probably better to check ip-header after we found the
   *          key. Then we could empty the whole bucket on error.
   */
  if (!_chk_ip_header(ip))
  {
    MSG (("!_chk_ip_header()\r"));
    STAT (ipstats.ips_fragdropped++);
    return (NULL);
  }

  /* Calculate where data should go
   */
  data_start  = (long) offset;
  data_length = (long) intel16 (ip->length) - in_GetHdrLen(ip);
  data_end    = data_start + data_length;
  more_frags  = (flags & IP_MF);

  if (!check_data_start(ip,data_start,data_length))
  {
    DEBUG_RX (NULL, ip);
    STAT (ipstats.ips_fragdropped++);
    return (NULL);
  }

  /* Assemble a fragment match-key
   */
  memset (&key, 0, sizeof(key));
  key.proto  = ip->proto;
  key.source = ip->source;
  key.destin = ip->destination;
  key.ident  = ip->identification;
  key.tos    = ip->tos;

  /* Check if we have a match
   */
  for (i = j = 0; i < MAX_FRAGMENTS; i++)
  {
   /* to-do!! : scan `j' over all our buckets.
    */
    frag = &frag_list[j][i];
    if (frag->used && !memcmp(&frag->key,&key,sizeof(key)))
    {
      found = TRUE;
      break;
    }
  }

  MSG (("bucket %d, key %sfound, i=%d\n", j, found ? "" : "not ", i));

  if (!found)
  {
    /* Can't handle any new frags, biff packet
     */
    if (frag_buckets[j].active == MAX_FRAGMENTS)
    {
      DEBUG_RX (NULL, ip);
      STAT (ipstats.ips_fragdropped++);
    }
    else
    {
      /* Setup first fragment received
       */
      setup_first_frag (ip, j, &key);
    }
    return (NULL);
  }

  MSG (("found, more_frags %d\n", more_frags ? 1 : 0));

  if (!more_frags)           /* Adjust length  */
     frag->ip->length = intel16 ((WORD)(data_end + in_GetHdrLen(ip)));

  hole = frag->hole_first;   /* Hole handling */

  do
  {
    long temp;

    if (hole && (data_start <= hole->end) &&   /* We've found the spot */
        (data_end >= hole->start))
    {
      got_hole = 1;

      /* Find where to insert fragment.
       * Check if there's a hole before the new frag
       */
      temp = hole->end;    /* Pick up old hole end for later */

      if (data_start > hole->start)
      {
        hole->end = data_start - 1;
        prev_hole = hole;  /* We have a new prev */
      }
      else
      {
        /* No, delete current hole
         */
        if (!prev_hole)
             frag->hole_first = hole->next;
        else prev_hole->next  = hole->next;
      }

      /* Is there a hole after the current fragment
       * Only if we're not last and more to come
       */
      if (data_end < hole->end && more_frags)
      {
        hole = (hole_descr*) (data_end + 1 + frag->data_offset);
        hole->start = data_end + 1;
        hole->end   = temp;

        /* prev_hole = NULL if first
         */
        if (!prev_hole)
        {
          hole->next = frag->hole_first;
          frag->hole_first = hole;
        }
        else
        {
          hole->next = prev_hole->next;
          prev_hole->next = hole;
        }
      }
    }
    prev_hole = hole;
    hole = hole->next;
  }
  while (hole);          /* Until we got to the end or found */


  /* Thats all setup so copy in the data
   */
  if (got_hole)
     memcpy (frag->data_offset + data_start,
             (BYTE*)ip + in_GetHdrLen(ip), (size_t)data_length);

  MSG (("got_hole %d, frag->hole_first %lX\n",
        got_hole, (DWORD)frag->hole_first));

  if (!frag->hole_first)    /* Now we have all the parts */
  { 
    if (frag_buckets[j].active >= 1)
        frag_buckets[j].active--;
 
   /* Redo checksum as we've changed the length in the header
    */
    frag->ip->frag_ofs = 0;
    frag->ip->checksum = 0;
    frag->ip->checksum = ~checksum (frag->ip, sizeof(in_Header));

    STAT (ipstats.ips_reassembled++);
    return (frag->ip);       /* MAC-header is in front of IP */
  }
  return (NULL);
}


/*
 *  Prepare and setup for reassembly
 */
static void setup_first_frag (const in_Header *ip, int idx, frag_key *key)
{
  frag_hdr   *frag;
  in_Header  *bucket;
  hole_descr *hole;
  int  i;

  /* Allocate a fragment bucket. MAC-header is in front of bucket.
   */
  bucket = alloc_frag_buffer (ip);
  if (!bucket)
  {
    STAT (ipstats.ips_fragdropped++);
    return;
  }

  /* Find first empty slot
   */
  frag = &frag_list[idx][0];
  for (i = 0; i < MAX_FRAGMENTS; i++, frag++)
      if (!frag->used)
         break;

  frag->used = 1;               /* mark as used             */
  frag_buckets[idx].active++;   /* inc active frags counter */

  MSG (("bucket=%d, active=%u, i=%d\n", idx, frag_buckets[idx].active, i));

  /* Remember MAC source address
   */
  if (!_pktserial)
       memcpy (&frag_buckets[idx].mac_src, MAC_SRC(ip), sizeof(mac_address));
  else memset (&frag_buckets[idx].mac_src, 0, sizeof(mac_address));

  /* Setup frag header data, first packet
   */
  frag->key   = *key;
  frag->ip    = bucket;
  frag->timer = set_timeout (1000 * max(_ip_frag_reasm, ip->ttl));

  /* Set pointers to beginning of IP packet data
   */
  frag->data_offset = (BYTE*)bucket + in_GetHdrLen(ip);

  /* Setup initial hole table
   */
  if (data_start == 0)  /* 1st fragment sent is 1st fragment received */
  {
    WORD  ip_len = intel16 (ip->length);
    BYTE *dst    = (BYTE*)bucket;   

    memcpy (dst, ip, min(ip_len,mtu));
    hole = (hole_descr*) (dst + ip_len + 1);
    frag->hole_first = hole;
  }
  else
  {
    /* !!fix-me: assumes header length of this fragment is same as
     *           in reassembled IP packet (may have IP-options)
     */
    BYTE *dst = frag->data_offset + data_start;
    BYTE *src = (BYTE*)ip + in_GetHdrLen(ip);

    memcpy (dst, src, (size_t)data_length);

    /* Bracket beginning of data
     */
    hole        = frag->hole_first = (hole_descr*)frag->data_offset;
    hole->start = 0;
    hole->end   = data_start - 1;
    if (more_frags)
    {
      hole->next = (hole_descr*) (frag->data_offset + data_length + 1);
      hole = hole->next;
    }
    else
    {
      hole = frag->hole_first->next = NULL;
      /* Adjust length */
      frag->ip->length = intel16 ((WORD)(data_end + in_GetHdrLen(ip)));
    }
  }

  if (hole)
  {
    hole->start = data_length;
    hole->end   = MAX_FRAG_SIZE;
    hole->next  = NULL;

    MSG (("hole %lX, start %lu, end %lu\n",
         (DWORD)hole, hole->start, hole->end));
  }
}


/*
 *  Check and report if fragment data-offset is okay
 */
static int check_data_start (const in_Header *ip, DWORD ofs, DWORD len)
{
  if (len % 8)                /* length must be multiple of 8 */
     return (0);              /* ofs is allready multiple of 8 */

  if (ofs <= MAX_FRAG_SIZE && /* fragment offset okay, < 65528 */
      ofs <= USHRT_MAX-8)
     return (1);

#if defined(USE_DEBUG)
  if (debug_on)
  {
    char src[20];
    char dst[20];
    (*_printf) (_LANG("Bad frag-ofs: fo %lu, ip-prot %u (%s->%s)\r\n"),
                ofs, ip->proto,
                _inet_ntoa (src,intel(ip->source)),
                _inet_ntoa (dst,intel(ip->destination)));
  }
#else
  ARGSUSED (ip);
  ARGSUSED (ip);
#endif
  return (0);
}


/*
 *  Allocate a bucket for doing fragment reassembly
 */
static in_Header *alloc_frag_buffer (const in_Header *ip)
{
  BYTE *p = NULL;
  int   i;

  for (i = 0; i < MAX_IP_FRAGS; i++)
  {
    if (frag_buckets[i].used)
       continue;

    if (!frag_buckets[i].ip)
    {
      p = calloc (BUCKET_SIZE + _pkt_ip_ofs, 1);
      if (!p)
      {
        MSG (("calloc() failed\n"));
        return (NULL);
      }
      frag_buckets[i].ip = (HugeIP*) (p + _pkt_ip_ofs);
      ((HugeIP*)p)->marker = BUCKET_MARKER;
    }
    else
    {
      p = (BYTE*)frag_buckets[i].ip - _pkt_ip_ofs;
      if (((HugeIP*)p)->marker != BUCKET_MARKER)
      {
        (*_printf) ("frag_buckets[%d] destroyed\r\n!", i);
     /* free (p);    heap probably corrupt, don't free */
        frag_buckets[i].ip = NULL;
        continue;
      }
    }
    MSG (("alloc_frag() = %lX\n", (DWORD)frag_buckets[i].ip));

    frag_buckets[i].used = 1;

    if (!_pktserial)
       memcpy (p, MAC_HDR(ip), _pkt_ip_ofs);  /* remember MAC-head */
    return (&frag_buckets[i].ip->hdr);
  }
  MSG (("all buckets full\n"));
  return (NULL);
}


/*
 *  Free/release the reassembled IP-packet.
 *  Note: we don't free it to the heap.
 */
int free_fragment (const in_Header *ip)
{
  int i, j;

  for (j = 0; j < MAX_IP_FRAGS; j++)
      if (ip == &frag_buckets[j].ip->hdr &&
          frag_buckets[j].used)
      {
        frag_buckets[j].used   = 0;
        frag_buckets[j].active = 0;
        for (i = 0; i < MAX_FRAGMENTS; i++)
            frag_list[j][i].used = 0;

        MSG (("free_fragment(%lX), bucket=%d\n", (DWORD)ip, j));
        return (1);
      }
  return (0);
}


/*
 *  Check if each fragment has timed-out
 */
void chk_timeout_frags (void)
{
  int j;

  for (j = 0; j < MAX_IP_FRAGS; j++)
  {
    int i;

    for (i = 0; i < MAX_FRAGMENTS; i++)
    {
      if (frag_buckets[j].active &&
          frag_list[j][i].used   &&
          chk_timeout(frag_list[j][i].timer))
      {
        const in_Header *ip = frag_list[j][i].ip;
        
        if (!_pktserial)    /* send a ICMP_TIMXCEED (code 1) */
             icmp_timexceed (ip, (void*) &frag_buckets[j].mac_src);
        else icmp_timexceed (ip, NULL);

        STAT (ipstats.ips_fragtimeout++);
        MSG (("chk_timeout_frags(), ip = %lX\n", (DWORD)ip));
        if (free_fragment(ip))
           break;
      }
    }
  }
}
#endif /* USE_FRAGMENTS */


/*----------------------------------------------------------------------*/

#if defined(TEST_PROG)  /* a small djgpp test program */
#undef FP_OFF
#undef enable
#undef disable

#include <unistd.h>
#include <assert.h>
#include <dos.h>

#include "sock_ini.h"
#include "udp_nds.h"
#include "loopback.h"
#include "pcarp.h"
#include "pcdbug.h"

static DWORD to_host   = 0;
static WORD  frag_ofs  = 0;
static int   max_frags = 5;
static int   frag_size = 1000;
static int   rand_frag = 0;
static int   time_frag = 0;

void usage (char *argv0)
{
  printf ("%s [-n num] [-s size] [-h ip] [-r] [-t]\n"
          "Send fragmented ICMP Echo Request (ping)\n\n"
          "options:\n"
          "  -n  number of fragments to send   (default %d)\n"
          "  -s  size of each fragment         (default %d)\n"
          "  -h  specify destination IP        (default 127.0.0.1)\n"
          "  -r  send fragment in random order (default %s)\n"
          "  -t  simulate fragment timeout     (default %s)\n",
          argv0, max_frags, frag_size,
          rand_frag ? "yes" : "no",
          time_frag ? "yes" : "no");
  exit (0);
}

BYTE *init_frag (int argc, char **argv)
{
  int   i, ch;
  BYTE *data;

  while ((ch = getopt(argc, argv, "h:n:s:rt?")) != EOF)
     switch (ch)
     {
       case 'h': to_host = inet_addr (optarg);
                 if (to_host == INADDR_NONE)
                 {
                   printf ("Illegal IP-address\n");
                   exit (-1);
                 }
                 break;
       case 'n': max_frags = atoi (optarg);
                 break;
       case 's': frag_size = atoi (optarg) / 8;  /* multiples of 8 */
                 frag_size <<= 3;
                 break;
       case 'r': rand_frag = 1;
                 break;
       case 't': time_frag = 1;   /* !! to-do */
                 break;
       case '?':
       default : usage (argv[0]);
     }

  if (max_frags < 1 || max_frags > FD_SETSIZE)
  {
    printf ("# of fragments is 1 - %d\n", FD_SETSIZE);
    exit (-1);
  }

  if (frag_size < 8 || frag_size > MAX_IP_DATA)
  {
    printf ("Fragsize range is 8 - %ld\n", MAX_IP_DATA);
    exit (-1);
  }

  if (frag_size * max_frags > USHRT_MAX)
  {
    printf ("Total fragsize > 64kB!\n");
    exit (-1);
  }

  data = calloc (frag_size * max_frags, 1);
  if (!data)
  {
    printf ("no memory\n");
    exit (-1);
  }

  for (i = 0; i < max_frags; i++)
     memset (data + i*frag_size, 'a'+i, frag_size);

  loopback_enable = 1;
  dbug_init();
  sock_init();

  if (!to_host)
     to_host = htonl (INADDR_LOOPBACK);
  return (data);
}

/*----------------------------------------------------------------------*/

int rand_packet (fd_set *fd, int max)
{
  int count = 0;

  while (1)
  {
    int i = Random (0, max);
    if (i < max && !FD_ISSET(i,fd))
    {
      FD_SET (i, fd);
      return (i);
    }
    if (++count == 10*max)
       return (-1);
  }
}

/*----------------------------------------------------------------------*/

int main (int argc, char **argv)
{
  fd_set      is_sent;
  int         i;
  eth_address eth;
  in_Header  *ip;
  ICMP_PKT   *icmp;
  WORD        frag_flag;
  BYTE       *data = init_frag (argc, argv);

  if (!_arp_resolve (ntohl(to_host), &eth, 0))
  {
    printf ("ARP failed\n");
    return (-1);
  }

  ip   = (in_Header*) _eth_formatpacket (&eth, IP_TYPE);
  icmp = (ICMP_PKT*) data;

  ip->hdrlen         = sizeof(*ip)/4;
  ip->ver            = 4;
  ip->tos            = 0;
  ip->identification = _get_ip_id();
  ip->ttl            = 15;           /* max 15sec reassembly time */
  ip->proto          = ICMP_PROTO;

  icmp->echo.type       = ICMP_ECHO;
  icmp->echo.code       = 0;
  icmp->echo.identifier = 0;
  icmp->echo.index      = 1;
  icmp->echo.sequence   = set_timeout (1);  /* "random" id */
  icmp->echo.checksum   = 0;
  icmp->echo.checksum   = ~checksum (icmp, max_frags*frag_size);

  FD_ZERO (&is_sent);

#if 0  /* test random generation */
  if (rand_frag)
     for (i = 0; i < max_frags; i++)
     {
       int j = rand_packet (&is_sent, max_frags);
       printf ("index %d\n", j);
     }
  exit (0);
#endif

  for (i = 0; i < max_frags; i++)
  {
    int j = rand_frag ? rand_packet (&is_sent,max_frags) : i;

    if (j < 0)
       break;
    if (i == max_frags-1)
         frag_flag = 0;
    else frag_flag = IP_MF;

    frag_ofs = (j * frag_size);
    memcpy ((BYTE*)(ip+1), data+frag_ofs, frag_size);

    /* The loopback device swaps src/dest IP; hence we must set them
     * on each iteration of the loop
     */
    ip->source      = intel (my_ip_addr);
    ip->destination = to_host;
    ip->frag_ofs    = intel16 (frag_ofs/8 | frag_flag);
    ip->length      = intel16 (frag_size + sizeof(*ip));
    ip->checksum    = 0;
    ip->checksum    = ~checksum (ip, sizeof(*ip));

    DEBUG_TX (NULL, ip);

    _eth_send (frag_size+sizeof(*ip));
    STAT (ipstats.ips_ofragments++);
    tcp_tick (NULL);
  }

  outsnl ("tcp_tick()");

  /* poll the IP-queue, reassemble fragments and
   * dump reassembled IP/ICMP packet to debugger (look in wattcp.dbg)
   */
  if (to_host != htonl (INADDR_LOOPBACK))
     usleep (1000000 + 100*max_frags*frag_size); /* wait for reply */

  tcp_tick (NULL);
  return (0);
}
#endif  /* TEST_PROG */
