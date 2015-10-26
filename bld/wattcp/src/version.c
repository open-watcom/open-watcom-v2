#include <stdio.h>
#include <string.h>

#include "wattcp.h"       /* target, config.h */
#include "../inc/tcp.h"   /* WATTCP_MAJOR_VER, WATTCP_MINOR_VER */

const char *wattcpCopyright = "See COPYRIGHT.H for details";

const char *wattcpVersion (void)
{
  static char buf[80];
  char  *p = buf;

#if defined(WATTCP_DEVEL_REL) && (WATTCP_DEVEL_REL > 0)
  p += sprintf (p, "Watt-32 - (%d.%d.%d), ",
                WATTCP_MAJOR_VER, WATTCP_MINOR_VER, WATTCP_DEVEL_REL);
#else
  p += sprintf (p, "Watt-32 - (%d.%d), ",
                WATTCP_MAJOR_VER, WATTCP_MINOR_VER);
#endif

#if defined(__GNUC__)
  p += sprintf (p, "GNU C %d.%d", __GNUC__, __GNUC_MINOR__);

  /* The `__tune_xx' defines where introduced in gcc 2.95.1 (?)
   */
  #if defined (__tune_i386__)         /* -m386 */
    strcpy (p, " (386), ");
  #elif defined (__tune_i486__)       /* -m486 */
    strcpy (p, " (486), ");
  #elif defined (__tune_pentium__)    /* -mpentium */
    strcpy (p, " (Pent), ");
  #elif defined (__tune_pentiumpro__) /* -mpentiumpro */
    strcpy (p, " (PentPro), ");
  #else
    strcpy (p, ", ");
  #endif

#elif defined(__HIGHC__)
  strcpy (p, "Metaware HighC, ");     /* no way to get version */

#elif defined(__WATCOMC__)
  p += sprintf (p, "Watcom C %d.%d", __WATCOMC__/100, __WATCOMC__ % 100);

  #if defined(__SMALL__)
    strcpy (p, " (small model), ");
  #elif defined(__LARGE__)
    strcpy (p, " (large model), ");
  #else
    #if (_M_IX86 >= 600)              /* -6r/s */
      strcpy (p, " (686");
    #elif (_M_IX86 >= 500)            /* -5r/s */
      strcpy (p, " (586");
    #elif (_M_IX86 >= 400)            /* -4r/s */
      strcpy (p, " (486");
    #else                             /* -3r/s */
      strcpy (p, " (386");
    #endif

    p += 5;

    #if defined(__SW_3S)              /* -3/4/5/6s */
      strcpy (p, "S), ");
    #else
      strcpy (p, "R), ");    /* Register based call is default */
    #endif
 #endif

#elif defined(__BORLANDC__)
  p += sprintf (p, "Borland C %X.%X", __BORLANDC__ >> 8, __BORLANDC__ & 0xFF);

  #if defined(__SMALL__)
    strcpy (p, " (small model), ");
  #elif defined(__LARGE__)
    strcpy (p, " (large model), ");
  #else
    strcpy (p, ", ");
  #endif

#elif defined(__TURBOC__)
  p += sprintf (p, "Turbo C %X.%X", (__TURBOC__ >> 8) - 1, __TURBOC__ & 0xFF);

  #if defined(__SMALL__)
    strcpy (p, " (small model), ");
  #elif defined(__LARGE__)
    strcpy (p, " (large model), ");
  #else
    strcpy (p, ", ");
  #endif

#elif defined(_MSC_VER)
  p += sprintf (p, "Microsoft C %d.%d", _MSC_VER/100, _MSC_VER % 100);

  #if defined(__SMALL__)
    strcpy (p, " (small model), ");
  #elif defined(__LARGE__)
    strcpy (p, " (large model), ");
  #else
    strcpy (p, ", ");
  #endif
#endif

  p = strchr (buf, '\0');

#if (DOSX == DJGPP)
  sprintf (p, "djgpp %d.%02d, ", __DJGPP__, __DJGPP_MINOR__);

#elif (DOSX == DJGPP_DXE)
  sprintf (p, "djgpp %d.%02d (DXE), ", __DJGPP__, __DJGPP_MINOR__);

#elif (DOSX == DOS4GW)
  strcpy (p, "DOS4GW, ");

#elif (DOSX == DOS4GW_DLL)            /* possible ? */
  strcpy (p, "DOS4GW (DLL), ");

#elif (DOSX == PHARLAP)
  strcpy (p, "Pharlap, ");

#elif (DOSX == PHARLAP_DLL)
  strcpy (p, "Pharlap (DLL), ");

#elif (DOSX == WDOSX)
  strcpy (p, "WDOSX, ");

#elif (DOSX == POWERPAK)
  strcpy (p, "PowerPak, ");
#endif

  strcat (p, __DATE__);
  return (buf);
}


const char *wattcpCapabilities (void)
{
  static char buf[150];  /* !!watch this space if adding more features */

  buf[0] = 0;

#if defined(USE_DEBUG)
  strcat (buf, "/debug");
#endif

#if defined(USE_MULTICAST)
  strcat (buf, "/mcast");
#endif

#if defined(USE_BIND)
  strcat (buf, "/bind");
#endif

#if defined(USE_BSD_FUNC)
  strcat (buf, "/bsdsock");
#endif

#if defined(USE_BOOTP)
  strcat (buf, "/bootp");
#endif

#if defined(USE_DHCP)
  strcat (buf, "/dhcp");
#endif

#if defined(USE_RARP)
  strcat (buf, "/rarp");
#endif

#if defined(USE_LANGUAGE)
  strcat (buf, "/lang");
#endif

#if defined(USE_FRAGMENTS)
  strcat (buf, "/frag");
#endif

#if defined(USE_STATISTICS)
  strcat (buf, "/stat");
#endif

#if defined(USE_BSD_FATAL)
  strcat (buf, "/bsdfatal");
#endif

#if defined(USE_BSD_FORTIFY)
  strcat (buf, "/bsdfortify");
#endif

#if defined(USE_FORTIFY)
  strcat (buf, "/fortify");
#endif

#if defined(USE_FSEXT)
  strcat (buf, "/fsext");
#endif

#if defined(USE_ETHERS)
  strcat (buf, "/ethers");
#endif

#if defined(USE_LOOPBACK)
  strcat (buf, "/loopback");
#endif

#if defined(USE_EMBEDDED)
  strcat (buf, "/embedded");
#endif

#if defined(USE_TFTP)
  strcat (buf, "/tftp");
#endif

#if defined(USE_TCP_SACK)
  strcat (buf, "/sack");
#endif

#if defined(USE_UDP_ONLY)
  strcat (buf, "/udp-only");
#endif

#if defined(USE_LIBPCAP)
  strcat (buf, "/pcap");
#endif

#if defined(USE_ECHO_DISC)
  strcat (buf, "/echo");
#endif

#if defined(USE_PPPOE)
  strcat (buf, "/pppoe");
#endif
  return (buf);
}

