/*
 * echo.c : A simple echo/discard daemon. Listens for traffic on
 *          udp/tcp ports 7 and 9.
 *
 * G. Vanem, Oct-2000
 */

#include <stdio.h>
#include <stdlib.h>

#include "wattcp.h"
#include "wattcpd.h"
#include "strings.h"
#include "pcconfig.h"
#include "pctcp.h"
#include "pcbsd.h"
#include "pcdbug.h"
#include "echo.h"

#if defined(USE_ECHO_DISC)

static udp_Socket udp_echo_sock, udp_disc_sock;
static tcp_Socket tcp_echo_sock, tcp_disc_sock;

static BOOL  do_echo   = 0;
static WORD  echo_port = 7;
static DWORD echo_host = 0;

static BOOL  do_disc   = 0;
static WORD  disc_port = 9;
static DWORD disc_host = 0;

static void echo_discard_daemon (void);
static void udp_handler (sock_type *s, BYTE *data, int len);

static void (*prev_hook) (const char*, const char*) = NULL;

/*
 * Parse and match "echo.daemon = 0/1", "echo.port = <n>" etc.
 */
static void echo_config (const char *name, const char *value)
{
  static struct config_table echo_cfg[] = {
                { "DAEMON", ARG_ATOI,    &do_echo   },
                { "HOST",   ARG_RESOLVE, &echo_host },
                { "PORT",   ARG_ATOI,    &echo_port },
                { NULL }
              };
  static struct config_table disc_cfg[] = {
                { "DAEMON", ARG_ATOI,    &do_disc   },
                { "HOST",   ARG_RESOLVE, &disc_host },
                { "PORT",   ARG_ATOI,    &disc_port },
                { NULL }
              };

  if (!parse_config_table(&echo_cfg[0], "ECHO.", name, value) &&
      !parse_config_table(&disc_cfg[0], "DISCARD.", name, value))
  {
    if (prev_hook)
      (*prev_hook) (name, value);
  }
}

/*
 * Called from sock_init(): Setup config-file parser for "echo..."
 * and "discard.." keywords.
 */
void echo_discard_init (void)
{
  prev_hook = usr_init;
  usr_init  = echo_config;
}

void echo_discard_start (void)
{
#if defined(USE_DEBUG)
  char buf[100], ip1[20], ip2[20];
  sprintf (buf, "echo-daemon %s, %s/%u, discard-daemon %s, %s/%u\r\n\r\n",
           do_echo ? "yes" : "no", _inet_ntoa(ip1,echo_host), echo_port,
           do_disc ? "yes" : "no", _inet_ntoa(ip2,disc_host), disc_port);
  dbug_write_raw (buf);
#endif

  if (do_echo)
  {
    udp_listen (&udp_echo_sock, echo_host, echo_port, 0,
                (ProtoHandler)udp_handler);
    udp_echo_sock.sockmode |= UDP_MODE_NOCHK;
    tcp_listen (&tcp_echo_sock, echo_host, echo_port, 0, NULL, 0);
  }

  if (do_disc)
  {
    udp_listen (&udp_disc_sock, disc_host, disc_port, 0,
                (ProtoHandler)udp_handler);
    udp_disc_sock.sockmode |= UDP_MODE_NOCHK;
    tcp_listen (&tcp_disc_sock, disc_host, disc_port, 0, NULL, 0);
  }

  if (do_echo || do_disc)
     addwattcpd (echo_discard_daemon);
}

/*
 * "background" process handling echo + discard TCP sockets.
 */
static void echo_discard_daemon (void)
{
  sock_type *s = (sock_type*) &tcp_echo_sock;

  if (sock_dataready(s))
  {
    BYTE buf[ETH_MAX_DATA];
    int  len = sock_read (s, buf, sizeof(buf));
    sock_write (s, buf, len);
  }
}

/*
 * callback handler for echo + discard UDP sockets.
 */
static void udp_handler (sock_type *s, BYTE *data, int len)
{
  if (s == (sock_type*)&udp_echo_sock)
  {
    if (!sock_enqueue (s, data, len))
       sock_close (s);
  }
  else   /* discard packet */
  {
  }
}
#endif

