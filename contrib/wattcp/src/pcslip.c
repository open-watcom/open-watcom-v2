/*
 *  This stuff will probably never take off --gv
 *  Not called during init.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "wattcp.h"
#include "pcqueue.h"
#include "pcsed.h"
#include "pcpkt.h"
#include "pcconfig.h"
#include "strings.h"
#include "misc.h"
#include "language.h"
#include "ioport.h"
#include "pcslip.h"

static void (*prev_hook) (const char*, const char*);

static WORD  slipBase    = 0x3F8;   /* COM1 */
static int   slipTimeout = 45;
static char *slipUser    = NULL;
static char *slipPasswd  = NULL;

static int  SlipDial (const char *str);
static void DoDial   (const char *val);

/*****************************************************************/

static void SlipConfig (const char *name, const char *value)
{
  static struct config_table slip_cfg[] = {
                { "DIAL",    ARG_FUNC,   (void*)DoDial       },
                { "USER",    ARG_STRDUP, (void*)&slipUser    },
                { "PASSWD",  ARG_STRDUP, (void*)&slipPasswd  },
                { "TIMEOUT", ARG_ATOI,   (void*)&slipTimeout },
                { "BASE",    ARG_ATOX_W, (void*)&slipBase    },
                { NULL }
              };

  if ((_pktdevclass != PD_SLIP ||
       !parse_config_table(&slip_cfg[0], "SLIP.", name, value)) &&
      prev_hook)
     (*prev_hook) (name, value);
}

/*****************************************************************/

int slip_init (void)
{
  prev_hook = usr_init;
  usr_init  = SlipConfig;
  return (0);
}

/*****************************************************************/

static int ModemCommand (const char *str, const char *resp, BYTE timeout)
{
  DWORD timer;
  int   len = str ? strlen (str) : strlen (resp);

  if (str)
     pkt_send (str, len);

  timer = set_timeout (1000 * timeout);

  while (!chk_timeout(timer))
  {
    WORD  type = 0;
    char *pkt  = (char*) _eth_arrived (&type, NULL);

    if (!pkt || type != IP_TYPE)
       continue;

    outsn (pkt, len);                             /* print the modem echo */
    _eth_free (pkt, type);
    return (strncmp(pkt,resp,strlen(resp)) == 0); /* got modem response */
  }
  return (0);
}

/*****************************************************************/

static int SlipDial (const char *str)
{
  char dialStr[80];
  WORD mcr = slipBase + 4;
  WORD lcr = slipBase + 3;

  _outportb (lcr, _inportb(lcr) & 0x43);  /* 8N1       */
  _outportb (mcr, _inportb(mcr) | 1);     /* raise DTR */

  if (!ModemCommand("ATZ\r","OK",5))
     return (0);

  strcpy (dialStr, str);
  strcat (dialStr, "\r");
  outs (_LANG("SLIP dialing.."));
  if (!ModemCommand(dialStr,"OK",2))
     return (0);

  if (!ModemCommand(NULL,"CONNECT",40))
     return (0);
  return (1);
}

static void DoDial (const char *value)
{
  if (!SlipDial(value))
     outsnl (_LANG("Modem not responding\7"));
}

