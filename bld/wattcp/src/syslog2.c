/*
 *  Simple syslog handler for Watt-32 & DOS.
 *
 *  by Gisle Vanem <giva@bgnett.no>  Jun-99
 *
 *  This module contain data and config-parser only.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/syslog.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>

#include "wattcp.h"
#include "printk.h"
#include "pcconfig.h"
#include "syslog2.h"

#if defined(USE_BSD_FUNC)

static void (*prev_hook) (const char*, const char*) = NULL;

#define DEF_MASK LOG_MASK (LOG_EMERG) | \
                 LOG_MASK (LOG_ALERT) | \
                 LOG_MASK (LOG_ERR)

char *syslog_fileName = NULL;      /* name of logfile */
char *syslog_hostName = NULL;      /* name of loghost */
WORD  syslog_port     = 514;       /* udp port to use */
int   syslog_mask     = DEF_MASK;  /* log priorities  */

static int getlogmask (const char *value)
{
  int mask = 0;

  if (!strcmp(value, "all"))
     return (LOG_UPTO (LOG_DEBUG));

  if (strstr(value, "emerg"))  mask |= LOG_MASK (LOG_EMERG);
  if (strstr(value, "alert"))  mask |= LOG_MASK (LOG_ALERT);
  if (strstr(value, "crit"))   mask |= LOG_MASK (LOG_CRIT);
  if (strstr(value, "error"))  mask |= LOG_MASK (LOG_ERR);
  if (strstr(value, "warn"))   mask |= LOG_MASK (LOG_WARNING);
  if (strstr(value, "notice")) mask |= LOG_MASK (LOG_NOTICE);
  if (strstr(value, "info"))   mask |= LOG_MASK (LOG_INFO);
  if (strstr(value, "debug"))  mask |= LOG_MASK (LOG_DEBUG);
  return (mask);
}

static void set_syslog_mask (const char *value)
{
  syslog_mask = getlogmask (value);
}

static void ourinit (const char *name, const char *value)
{
  static struct config_table syslog_cfg[] = {
                { "FILE",  ARG_STRDUP, (void*)&syslog_fileName },
                { "HOST",  ARG_STRDUP, (void*)&syslog_hostName },
                { "PORT",  ARG_ATOI,   (void*)&syslog_port     },
                { "LEVEL", ARG_FUNC,   (void*)set_syslog_mask  },
                { NULL }
              };
  if (!parse_config_table(&syslog_cfg[0], "SYSLOG.", name, value) && prev_hook)
     (*prev_hook) (name, value);
}

void syslog_init (void)
{
  prev_hook = usr_init;
  usr_init  = ourinit;
}
#endif /* USE_BSD_FUNC */
