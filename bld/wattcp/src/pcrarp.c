/*
 *   RARP - Boot Protocol (RFC 903)
 *
 *   These extensions get called if MY_IP is set to RARP in wattcp.cfg
 *
 *   Version
 *
 *   0.0 : Sept 6, 1996 : Copied from E. Engelke's pcbootp.c by Dan Kegel
 *   0.1 : Febr 9, 1997 : Modified, removed debug printing,  G. Vanem
 */

#include <stdio.h>
#include <string.h>

#include "copyrigh.h"
#include "wattcp.h"
#include "strings.h"
#include "language.h"
#include "misc.h"
#include "pcsed.h"
#include "pctcp.h"
#include "pcdbug.h"
#include "pcrarp.h"

WORD _rarptimeout = 15;

static int _rarp_request (void)
{
  rarp_Header *rarp = (rarp_Header*) _eth_formatpacket (&_eth_brdcast[0],
                                                        RARP_TYPE);
  rarp->hwType       = intel16 (_eth_get_hwtype(NULL,NULL));
  rarp->protType     = IP_TYPE;
  rarp->hwAddrLen    = sizeof (mac_address);
  rarp->protoAddrLen = sizeof (DWORD);
  rarp->opcode       = RARP_REQUEST;
  rarp->srcIPAddr    = 0;
  rarp->dstIPAddr    = 0;
  memcpy (rarp->srcEthAddr, _eth_addr, sizeof(mac_address));
  memcpy (rarp->dstEthAddr, _eth_addr, sizeof(mac_address));

  DEBUG_TX (NULL, rarp);
  return _eth_send (sizeof(*rarp));
}

/*
 * _dorarpc - Checks global variable _rarptimeout
 *            returns 1 on success and sets ip address
 */
int _dorarp (void)
{
  struct rarp_Header *rarp = NULL;
  DWORD  rarptimeout       = set_timeout (1000 * _rarptimeout);
  WORD   protocol          = 0;
  WORD   magictimeout      = Random (7000, 14000);
  BOOL   link_broadcast    = FALSE;

  outs (_LANG("Configuring through RARP..."));

  while (1)
  {
    DWORD sendtimeout;

    if (!_rarp_request())
       return (0);

    sendtimeout = set_timeout (magictimeout);
    magictimeout += Random (1000, 7000);

    while (!chk_timeout(sendtimeout))
    {
      if (chk_timeout(rarptimeout))
         goto quit;

      kbhit();
      rarp = (rarp_Header*) _eth_arrived (&protocol, &link_broadcast);
      if (rarp && protocol == RARP_TYPE)
      {
        DEBUG_RX (NULL, rarp);
        if (protocol == RARP_TYPE && !link_broadcast &&
            rarp->opcode == RARP_REPLY)
        {
          my_ip_addr = intel (rarp->dstIPAddr);
          goto quit;
        }
      }
    }
  }

quit:
  if (rarp)
     _eth_free (rarp, protocol);

  return (my_ip_addr != 0);
}

