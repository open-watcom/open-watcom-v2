/*
 * Boot-ROM-Code to load (any file or) an operating system across
 * a TCP/IP network.
 *
 * Module:  tftp.c
 * Purpose: Get a file with TFTP protocol
 * Entries: tftp_boot_load, tftp_set_server, tftp_set_boot_fname
 *
 **************************************************************************
 *
 * Copyright (C) 1995,1996,1997 Gero Kuhlmann <gero@gkminix.han.de>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *  Major changes for Watt-32 by G. Vanem <giva@bgnett.no> 1999
 *
 *  This client adheres to RFC-1350 (TFTP v2), but supports only
 *  reading from a remote host.
 *
 *  !!to-do: Support ETFTP (RFC-1986)
 */

#include "socket.h"
#include <netdb.h>
#include <arpa/tftp.h>
#include "udp_dom.h"
#include "tftp.h"

#if defined(USE_TFTP)

#if defined(USE_DEBUG)
  #define TRACE(x)   (*_printf) x
#else
  #define TRACE(x)   ((void)0)
#endif


/*
 * Error codes private to this file:
 */
#define ERR_INV       1       /* invalid packet size */
#define ERR_ERR       2       /* error packet received */
#define ERR_OP        3       /* invalid opcode */
#define ERR_BLOCK     4       /* invalid block number */
#define ERR_TIMEOUT   5       /* timeout while receiving */
#define ERR_UNKNOWN   6       /* unknown error */

/*
 * Various definitions:
 */
#define TFTP_RETRY      5          /* Maximum number of retries */
#define TFTP_TIMEOUT    8          /* 8 seconds timeout */
#define TFTP_HEADSIZE   4          /* th_opcode/th_block size */
#define TFTP_PORT_LOW   1024       /* lowest legal local port */
#define OCTET_STR       "octet"    /* name for 8-bit raw format */
#define NETASCII_STR    "netascii" /* name for netascii format */
#define MAIL_STR        "mail"     /* name for mail format */

/*
 * Public data
 */
int (*tftp_writer) (const void*, size_t) = NULL;
int (*tftp_terminator) (void)            = NULL;

extern const char *icmp_type_str [ICMP_MAXTYPE+1];

/*
 * Local variables
 */
static struct tftphdr *inbuf;    /* TFTP input buffer  */
static struct tftphdr *outbuf;   /* TFTP output buffer */
static sock_type      *sock;     /* Socket for UDP recv/xmit */

static int   currblock;          /* Current data block           */
static int   blocksize;          /* Server's block size          */
static int   ibuflen;            /* Size of data in input buffer */
static int   isopen;             /* TRUE if connection is open   */

static int   tftp_errno   = 0;
static DWORD tftp_server  = 0;
static int   tftp_timeout = TFTP_TIMEOUT;
static int   tftp_retry   = TFTP_RETRY;
static int   tftp_lport   = 0;

static char  tftp_server_name[MAX_HOSTLEN] = "";
static char  tftp_boot_fname [MAX_PATHLEN] = "";
static char  tftp_xfer_mode  [MAX_STRING]  = OCTET_STR;


/*
 * Send a tftp request packet
 */
static void send_req (char request, const char *fname)
{
  char *cp, *mode;
  int   len;
  int   fnamlen = strlen (fname);

  /* The output buffer is setup with the request code, the file name,
   * and the name of the data format.
   */
  memset (outbuf, 0, sizeof(*outbuf));
  outbuf->th_opcode = intel16 (request);
  len = SEGSIZE - sizeof(outbuf->th_opcode) - strlen(tftp_xfer_mode) - 1;
  cp  = (char*)&outbuf->th_stuff;

  for ( ; *fname && len > 0 && fnamlen > 0; len--, fnamlen--)
      *cp++ = *fname++;
  *cp++ = '\0';

  for (mode = tftp_xfer_mode; *mode; )
      *cp++ = *mode++;
  *cp++ = '\0';

  /* Finally send the request
   */
  len = (int) (cp - (char*)outbuf);
  sock_fastwrite (sock, (BYTE*)outbuf, len);
}


/*
 * Send a tftp acknowledge packet
 */
static void send_ack (int block)
{
  struct tftphdr ack;

  ack.th_opcode = intel16 (ACK);
  ack.th_block  = intel16 ((WORD)block);
  sock_fastwrite (sock, (BYTE*)&ack, TFTP_HEADSIZE);
}

#if defined(USE_DEBUG)
/*
 * Return error string for 'th_code'
 */
static const char *tftp_strerror (int code)
{
  static const char *err_tab[] = {
                    "EUNDEF",
                    "ENOTFOUND",
                    "EACCESS",
                    "ENOSPACE",
                    "EBADOP",
                    "EBADID",
                    "EEXISTS",
                    "ENOUSER"
                  };
  if (code < 0 || code > ENOUSER)
     return ("?");
  return (err_tab[code]);
}
#endif


/*
 * Watch out for "ICMP port unreachable".
 */
static void udp_callback (udp_Socket *s, int icmp_type)
{
  if (s == (udp_Socket*)sock && s->ip_type == UDP_PROTO &&
     icmp_type == ICMP_UNREACH)
  {
    /* In lack of a better way, pretend we got a FIN.
     * This causes sock_wait_input() below to break it's loop.
     */
    s->locflags |= LF_GOT_FIN; 
    s->err_msg = icmp_type_str [ICMP_UNREACH];
  }
}

/*
 * Receive a TFTP data packet
 */
static int recv_packet (int block)
{
  int len, status = 0;
 
  /* Use a callback since first block sent might cause a "ICMP
   * port unreachable" to be sent back. Note that the normal mechanism
   * of detecting ICMP errors (through _udp_cancel) doesn't work since
   * we did set 'sock->udp.hisaddr = 0'.
   */
  if (block == 1)
       sock->udp.sol_callb = (sol_upcall) udp_callback;
  else sock->udp.sol_callb = NULL;

  /* Read packet with timeout
   */
  sock_wait_input (sock, tftp_timeout, NULL, &status);

  len = sock_fastread (sock, (BYTE*)inbuf, TFTP_HEADSIZE+SEGSIZE);

  /* Check that the packet has a correct length
   */
  len -= TFTP_HEADSIZE;
  if (len < 0 || len > SEGSIZE)
  {
    TRACE (("tftp: Invalid packet, len = %d\r\n"));
    tftp_errno = ERR_INV;
    return (-1);
  }

  /* Check if we got an error packet
   */
  if (intel16(inbuf->th_opcode) == ERROR)
  {
#if defined(USE_DEBUG)
    int   code = intel16 (inbuf->th_code);
    const char *str = tftp_strerror (code);

    TRACE (("tftp: Error: %s (%d): %.*s\r\n",
           str, code, SEGSIZE, inbuf->th_data));
#endif
    tftp_errno = ERR_ERR;
    return (-1);
  }

  /* Check if we got a valid data packet at all
   */
  if (intel16(inbuf->th_opcode) != DATA)
  {
    TRACE (("tftp: Invalid opcode %d\r\n", intel16(inbuf->th_opcode)));
    tftp_errno = ERR_OP;
    return (-1);
  }

  /* Check if the block number of the data packet is correct
   */
  if (intel16(inbuf->th_block) != block)
  {
    TRACE (("tftp: Block %d != %d\r\n", intel16(inbuf->th_block), block));
    tftp_errno = ERR_BLOCK;
    return (-1);
  }

  tftp_errno = 0;
  return (len);

sock_err:
  if (status == 0)
  {
    TRACE (("tftp: Timeout\n"));
    tftp_errno = ERR_TIMEOUT;
    return (-1);
  }
  /* most likely "Port unreachable"
   */
  TRACE (("tftp: %s\n", sockerr(&sock->tcp)));
  tftp_errno = ERR_UNKNOWN;
  return (-1);
}


/*
 * Open a TFTP connection on a random local port (our transaction ID).
 * Send the request, wait for first data block and send the first ACK.
 */
static int tftp_open (DWORD server, const char *fname)
{
  int  retry;
  WORD port = 69;

#if defined(USE_BSD_FUNC)
  struct servent *sp = getservbyname ("tftp", "udp");

  if (sp)
     port = intel16 ((WORD)sp->s_port);
#endif

  currblock = 0;
  blocksize = 0;

  for (retry = 0; retry < tftp_retry; retry++)
  {
    WORD our_tid;  /* our transaction ID (local port) */

    if (tftp_lport && tftp_lport < TFTP_PORT_LOW)
       outsnl (_LANG("tftp: Illegal local port."));

    if (tftp_lport >= TFTP_PORT_LOW)
         our_tid = tftp_lport;
    else our_tid = Random (TFTP_PORT_LOW, USHRT_MAX);

    /* Try to open a TFTP connection to the server
     */
    if (!udp_open(&sock->udp, our_tid, server, port, NULL))
    {
      STAT (ipstats.ips_noroute++);
      TRACE (("tftp: ARP timeout\r\n"));
      return (0);
    }

    /* Send the file request block, and then wait for the first data
     * block. If there is no response to the query, retry it with
     * another transaction ID (local port), so that all old packets get
     * discarded automatically.
     */
    send_req (RRQ, fname);

    /* This hack makes it work because the response is sent back on
     * a source-port different from port (69); i.e. the server TID
     * uses a random port. Force the response packet to match a passive
     * socket in udp_handler().
     */
    sock->udp.hisaddr = 0;

    ibuflen = recv_packet (1);
    if (ibuflen >= 0)
    {
      blocksize = ibuflen;
      TRACE (("tftp: blocksize = %d\n", blocksize));
      isopen = TRUE;
      send_ack (1);
      return (1);
    }

    /* If an error (except timeout) occurred, retries are useless
     */
    if (tftp_errno == ERR_ERR || tftp_errno == ERR_UNKNOWN)
       break;
  }
  return (0);
}

/*
 * Close the TFTP connection
 */
static void tftp_close (void)
{
  if (tftp_terminator)
     (*tftp_terminator)();

  if (debug_on)
     outs ("\n");

  if (sock)
  {
    sock_close (sock);
    free (sock);
    sock = NULL;
  }
  if (inbuf)
     free (inbuf);
  if (outbuf)
     free (outbuf);
  inbuf = outbuf = NULL;
}


/*
 * Set the name of TFTP server
 */
char *tftp_set_server (const char *name, int len)
{
  len = min (len, sizeof(tftp_server_name)-1);
  memcpy (tftp_server_name, name, len);
  tftp_server_name[len] = 0;
  return (tftp_server_name);
}

/*
 * Set the name of file to load from TFTP server
 */
char *tftp_set_boot_fname (const char *name, int len)
{
  len = min (len, sizeof(tftp_boot_fname)-1);
  memcpy (tftp_boot_fname, name, len);
  tftp_boot_fname[len] = 0;
  return (tftp_boot_fname);
}

/*
 * Set the mode used for transfer
 */
char *tftp_set_xfer_mode (const char *name, int len)
{
  len = min (len, sizeof(tftp_xfer_mode)-1);
  memcpy (tftp_xfer_mode, name, len);
  tftp_xfer_mode[len] = 0;
  return (tftp_xfer_mode);
}

/*
 * Read the next data packet from a TFTP connection
 */
static int tftp_get (char **buf)
{
  int retry;

  /* Don't do anything if no TFTP connection is active.
   */
  if (!isopen)
     return (0);

  /* If the block number is 0 then we are still dealing with the first
   * data block after opening a connection. If the data size is smaller
   * than 'blocksize' just close the connection again.
   */
  if (currblock == 0)
  {
    currblock++;
    if (ibuflen < blocksize)
       isopen = FALSE;
    *buf = &inbuf->th_data[0];
    return (ibuflen);
  }

  /* Wait for the next data packet. If no data packet is coming in,
   * resend the ACK for the last packet to restart the sender. Maybe
   * he didn't get our first ACK.
   */
  for (retry = 0; retry < tftp_retry; retry++)
  {
    if (debug_on)
       (*_outch) ('.');

    ibuflen = recv_packet (currblock+1);
    if (ibuflen >= 0)
    {
      currblock++;
      send_ack (currblock);
      if (ibuflen < blocksize)  /* last block received */
         isopen = FALSE;
      *buf = &inbuf->th_data[0];
      return (ibuflen);
    }
    if (tftp_errno == ERR_ERR || tftp_errno == ERR_UNKNOWN)
       break;

    send_ack (currblock);
  }
  isopen = FALSE;
  return (-1);
}

/*
 * Load the BOOT-file from TFTP server
 */
int tftp_boot_load (void)
{
  int rc = 0;

  /* Allocate socket and buffers
   */
  sock   = malloc (sizeof(sock->udp));
  inbuf  = malloc (TFTP_HEADSIZE+SEGSIZE);
  outbuf = malloc (TFTP_HEADSIZE+SEGSIZE);

  if (!sock || !inbuf || !outbuf)
  {
    outsnl (_LANG("No memory for TFTP boot."));
    return (0);
  }

  if (!tftp_boot_fname[0])
  {
    outsnl (_LANG("No TFTP boot filename defined."));
    return (0);
  }

  if (tftp_server_name[0] && !tftp_server)
  {
    tftp_server = resolve (tftp_server_name);
    outs (_LANG("Cannot resolve TFTP-server "));
    outsnl (tftp_server_name);
    return (0);
  }

  if (!tftp_server)
  {
    outsnl (_LANG("No TFTP-server defined."));
    return (0);
  }

  if (debug_on)
     outs (_LANG("Doing TFTP boot load..."));

  /* Open connection and request file
   */
  if (!tftp_open (tftp_server, tftp_boot_fname))
     return (0);

  while (1)
  {
    char *buf;
    int   size = tftp_get (&buf);

    if (size < 0)  /* error in transfer */
       break;

    if (size > 0 && (*tftp_writer)(buf,size) <= 0)
    {
      rc = -1;     /* writer failed, errno set */
      break;
    }
    if (size < blocksize)    /* got last block */
    {
      rc = 1;
      break;
    }
  }
  tftp_close();
  return (rc);
}


/*
 * Config-file handler for TFTP-client
 */
static void (*prev_hook) (const char*, const char*) = NULL;

static void tftp_cfg_hook (const char *name, const char *value)
{
  static struct config_table tftp_cfg[] = {
                { "BOOT_FILE", ARG_FUNC,    (void*)tftp_set_boot_fname },
                { "SERVER",    ARG_RESOLVE, (void*)&tftp_server        },
                { "TIMEOUT",   ARG_ATOI,    (void*)&tftp_timeout       },
                { "RETRY",     ARG_ATOI,    (void*)&tftp_retry         },
                { "MODE",      ARG_FUNC,    (void*)tftp_set_xfer_mode  },
                { "PORT",      ARG_ATOI,    (void*)&tftp_lport         },
                { NULL }
              };
  if (!parse_config_table(tftp_cfg, "TFTP.", name, value) && prev_hook)
     (*prev_hook) (name, value);
}

/*
 * Initialize config-hook for TFTP protocol.
 */ 
int tftp_init (void)
{
  prev_hook = usr_init;
  usr_init  = tftp_cfg_hook;
  return (TRUE);
}

/*
 * A small test program, for djgpp/Watcom only
 */
#if defined(TEST_PROG) 

#include "getopt.h"
#include "udp_nds.h"
#include "pcdbug.h"
#include "pcbsd.h"
#include "pcarp.h"

extern unsigned int sleep (unsigned int);

static FILE *file;
static char *fname;

static int write_func (const void *buf, size_t length)
{
  static int block = 0;

  debug_on = 2;

  if (block == 0)
  {
    fname = tftp_boot_fname;  /* !!to-do: map into local filesystem */
    fprintf (stderr, "opening `%s'\n", fname);
    file = fopen (fname, "wb");
  }
  if (!file)
  {
    perror (fname);
    return (0);
  }
  fwrite (buf, 1, length, file);
  return (++block);
}

int close_func (void)
{
  if (file && fname)
  {
    fprintf (stderr, "closing `%s'\n", fname);
    fclose (file);
    file = NULL;
    return (1);
  }
  return (0);
}

void usage (char *argv0)
{
  char buf1[20], buf2[20], *env = getenv (_watt_environ_name);

  printf ("Usage: %s [[-n] [-a] [-h host] [-f file]\n"
          "\t\t [-i ip] [-m mask]] [-t timeout] [-r retry]\n"
          "\t -n run with no config file (default %s\\%s)\n"
          "\t -a add random MAC address for tftp host\n"
          "\t -h specify tftp host\n"
          "\t -f specify file to load\n"
          "\t -i specify ip-address      (default %s)\n"
          "\t -m specify network mask    (default %s)\n"
          "\t -t specify total timeout   (default %d)\n"
          "\t -r specify retry count     (default %d)\n",
          argv0, env ? env : ".", _watt_config_name,
          _inet_ntoa(buf1,my_ip_addr),
          _inet_ntoa(buf2,sin_mask),
          tftp_timeout, tftp_retry);
  exit (-1);
}

int main (int argc, char **argv)
{
  eth_address eth = { 1,2,3,4,5,6 };
  int a_flag = 0;
  int h_flag = 0;
  int i_flag = 0;
  int f_flag = 0;
  int n_flag = 0;
  int m_flag = 0;
  int ch;

  while ((ch = getopt(argc, argv, "an?h:i:f:m:t:r:")) != EOF)
     switch (ch)
     {
       case 'a':
            a_flag = 1;
            break;
       case 'n':        
            n_flag = 1;
            break;
       case 'h':
            h_flag = 1;
            tftp_server = aton (optarg);
            break;
       case 'i':
            i_flag = 1;
            my_ip_addr = aton (optarg);
            break;
       case 'f':
            f_flag = 1;
            tftp_set_boot_fname (optarg, strlen(optarg));
            break;
       case 'm':
            m_flag = 1;
            sin_mask = aton (optarg);
            break;
       case 't':
            tftp_timeout = atoi (optarg);
            break;
       case 'r':
            tftp_retry = atoi (optarg);
            break;
       case '?':
       default:
            usage (argv[0]);
     }

  if (n_flag)  /* Demonstrate running with no config file */
  {
    _watt_no_config = 1;
    dbg_mode_all    = 1;
    dbg_print_stat  = 1;
    debug_on = 2;

    if (!m_flag)  sin_mask    = aton ("255.255.0.0");
    if (!i_flag)  my_ip_addr  = aton ("192.168.0.1");
    if (!h_flag)  tftp_server = aton ("192.168.0.2");
    if (!f_flag)  tftp_set_boot_fname ("test", 4);
    if (a_flag)  _arp_add_cache (tftp_server, &eth, 0);
  }
  else if (m_flag || i_flag || h_flag || f_flag || a_flag)
  {
    puts ("Cannot use this option without -n flag");
    return (-1);
  }

  dbug_init();
  if (n_flag)
     dbug_open();

  /* Must set our hook first
   */
  tftp_writer     = write_func;
  tftp_terminator = close_func;

  sock_init();

  sleep (1);       /* drain network buffers */
  tcp_tick (NULL);
  return (0);
}
#endif /* TEST_PROG */
#endif /* USE_TFTP */
