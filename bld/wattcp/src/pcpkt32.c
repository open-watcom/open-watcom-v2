/*
 *  Skeleton (not finished) for probing for the presence of network cards.
 *
 *  All drivers should be put in a dynamically loaded module (DLL/DXE).
 *
 *  G. Vanem 1998  <giva@bgnett.no>
 */

#include <stdio.h>
#include <stdlib.h>

#include "copyrigh.h"
#include "wattcp.h"
#include "pcconfig.h"
#include "pcqueue.h"

#define DEFINE_IREGS
#include "wdpmi.h"
#include "pcsed.h"
#include "pcpkt.h"
#include "pcpkt32.h"

#if (DOSX)

int (*_pkt32_drvr)(IREGS*) = NULL;

struct PM_driver pm_driver_list[] = {
                 { PM_DRVR_3C501,  "3c501"  },
                 { PM_DRVR_3C503,  "3c503"  },
                 { PM_DRVR_3C505,  "3c505"  },
                 { PM_DRVR_3C507,  "3c507"  },
                 { PM_DRVR_3C5x9,  "3c5x9"  },
                 { PM_DRVR_3C59x,  "3c59x"  },
                 { PM_DRVR_NE2000, "NE2000" },
                 { PM_DRVR_EEXPR,  "EthExp" },
                 { 0,              NULL     }
               };

int pkt32_drvr_probe (const PM_driver *drivers)
{
  ARGSUSED (drivers);
  return (0);
}

int pkt32_drvr_init (int driver)
{
  ARGSUSED (driver);
  return (0);
}

const char *pkt32_drvr_name (int driver)
{
  int i;
  for (i = 0; pm_driver_list[i].type; i++)
      if (pm_driver_list[i].type == driver)
         return (pm_driver_list[i].name);

  return ("unknown");
}

#endif  /* DOSX */

