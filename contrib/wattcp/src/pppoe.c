/*
 *   PPPoE (PPP-over-Ethernet) for Watt-32. Refer RFC-2516 for spec.
 *
 *   Version
 *
 *   0.5 : Aug 07, 2001 : G. Vanem - created
 *   0.6 : Aug 08, 2001 : Added extensions from
                          draft-carrel-info-pppoe-ext-00.txt
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "wattcp.h"
#include "misc.h"
#include "strings.h"
#include "pcconfig.h"
#include "pcdbug.h"
#include "pcqueue.h"
#include "pctcp.h"
#include "pcsed.h"
#include "pcpkt.h"
#include "pppoe.h"

#if defined(USE_PPPOE)
#include <netinet/in.h>
#include <net/ppp_defs.h>

enum States {
     StateDiscovery = 1,
     StateSession
   };

static void (*prev_hook) (const char*, const char*) = NULL;

static mac_address macAddr;   /* AC's MAC address */
static WORD        session;
static BOOL        got_PADO;
static BOOL        got_PADS;
static BOOL        got_PADT;
static enum States state;

static struct {
         int   enable;
         int   timeout;
         int   retries;
         char *serviceName;
       } cfg = { 0, 5, 3, NULL };

static int  pppoe_send (int code);
static int  pppoe_wait (int code);

static void pppoe_config (const char *name, const char *value)
{
  static struct config_table pppoe_cfg[] = {
                { "ENABLE",      ARG_ATOI,   (void*)&cfg.enable      },
                { "TIMEOUT",     ARG_ATOI,   (void*)&cfg.timeout     },
                { "RETRIES",     ARG_ATOI,   (void*)&cfg.retries     },
                { "SERVICENAME", ARG_STRDUP, (void*)&cfg.serviceName },
                { NULL }
              };
  if (!parse_config_table(&pppoe_cfg[0], "PPPOE.", name, value) && prev_hook)
     (*prev_hook) (name, value);
}

/*
 * Set config-parser hook and initial values
 */
void pppoe_init (void)
{
  prev_hook = usr_init;
  usr_init  = pppoe_config;
  state = StateDiscovery;
  got_PADO = got_PADS = got_PADT = FALSE;
  memset (&macAddr, 0xFF, sizeof(macAddr)); /* start with broadcast */
}

/*
 * Initialise PPPoE by going through Discovery state and
 * enter Session state
 */
int pppoe_discovery (void)
{
  int loop;

  if (!cfg.enable)
     return (1);

  for (loop = 1; loop <= cfg.retries; loop++)
  {
    if (!pppoe_send(PPPOE_CODE_PADI))  /* send Init packet */
       return (0);

    if (pppoe_wait(PPPOE_CODE_PADO))   /* wait for Offer */
       break;
  }
  if (loop > cfg.retries)
     return (0);

  for (loop = 1; loop <= cfg.retries; loop++)
  {
    if (!pppoe_send(PPPOE_CODE_PADR))  /* send Request */
       return (0);

    if (pppoe_wait(PPPOE_CODE_PADS))   /* wait for Session-confirm */
       break;
  }
  if (loop > cfg.retries)
     return (0);

  state = StateSession;
  return (1);
}

/*
 * Close down PPPoE by sending PADT
 */
void pppoe_exit (void)
{
  if (!cfg.enable || state != StateSession)
     return;

  pppoe_send (PPPOE_CODE_PADT);
  session = 0;
  state = 0;
  memset (&macAddr, 0xFF, sizeof(macAddr));
}

/*
 * pppoe_handler() - handle incoming PPPoE packets
 */
int pppoe_handler (struct pppoe_Packet *pkt)
{
  WORD proto, code;

  if (pkt->head.type != 1 || pkt->head.ver != 1)
     return (0);

  proto = pkt->head.proto;
  code  = pkt->head.code;

  if (proto == PPPOE_SESS_TYPE && state == StateSession)
  {
    if (code == PPPOE_CODE_SESS && pkt->head.session == session &&
        !memcmp(pkt->head.source, &macAddr, sizeof(macAddr)))
    {
      int   len = intel16 (pkt->head.length);
      BYTE *ppp = &pkt->data[0] - 2; /* overwrite pkt->head.length */

      PPP_ADDRESS (ppp) = PPP_ALLSTATIONS;
      PPP_CONTROL (ppp) = PPP_UI;
   // ppp_input (ppp, len);
    }
  }
  else if (proto == PPPOE_DISC_TYPE && state == StateDiscovery)
  {
    if (code == PPPOE_CODE_PADO)
    {
      got_PADO = TRUE;
      memcpy (&macAddr, &pkt->head.source, sizeof(macAddr));
    }
    else if (code == PPPOE_CODE_PADT && pkt->head.session == session)
    {
      got_PADT = TRUE;
      outsnl ("PPPoE: session terminated");
    }
    else if (code == PPPOE_CODE_PADS)
    {
      got_PADS = TRUE;
      session = pkt->head.session;
    }
  }
  return (1);
}

/*
 * Return true if we have a session
 */
BOOL pppoe_is_up (void)
{
  return (session != 0 && state == StateSession && !got_PADT);
}

/*
 * Build a PADx packet
 */
static int build_pad (struct pppoe_Packet *pkt, WORD code)
{
  BYTE *tags = &pkt->data[0];
  int   len  = 0;

  pkt->head.ver     = 1;
  pkt->head.type    = 1;
  pkt->head.code    = code;
  pkt->head.session = session;

  if (code == PPPOE_CODE_PADI || code == PPPOE_CODE_PADR)
  {
    len = PPPOE_TAG_HDR_SIZE;
    *(WORD*) tags = PPPOE_TAG_SERVICE_NAME;
    if (cfg.serviceName)
    {
      int slen = strlen (cfg.serviceName);
      len += slen;
      memcpy (tags, cfg.serviceName, slen);
      *(WORD*) (tags+2) = intel16 (slen);
    }
    else
      *(WORD*) (tags+2) = 0;  /* No service-selection */
  }
  pkt->head.length = intel16 (len);
  return (len + PPPOE_HDR_SIZE);
}

/*
 * Build and send a PADx (PPPoE Active Discovery) packet as
 * link-layer broadcast or unicast
 */
static int pppoe_send (int code)
{
  void         *eth = _eth_formatpacket (&macAddr[0], PPPOE_DISC_TYPE);
  pppoe_Packet *pkt = (struct pppoe_Packet*) MAC_HDR (eth);
  int           len = build_pad (pkt, code);

#if defined(USE_DEBUG)
  if (_dbugxmit)
    (*_dbugxmit) (NULL, eth, __FILE__, __LINE__);
  if (debug_on)
  {
    outs ("PPPOE: sending code 0x");
    outhex (code);
  }
#endif
  return _eth_send (len);
}

/*
 * Loop waiting for timeout or PAD response
 */
static int pppoe_wait (int wait_code)
{
  DWORD timer = set_timeout (1000 * cfg.timeout);

  while (1)
  {
    tcp_tick (NULL);

    if (chk_timeout(timer))
       return (0);

    if ((wait_code == PPPOE_CODE_PADO && got_PADO) ||
        (wait_code == PPPOE_CODE_PADS && got_PADS))
    {
      if (debug_on)
      {
        outs ("PPPoE: got code 0x");
        outhex (wait_code);
      }
      return (1);
    }
  }
  return (0);
}

#endif /* (USE_PPPOE) */

