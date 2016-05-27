/*
 *  get/put short/long functions for little-endian platforms
 */

#include "wattcp.h"
#include "chksum.h"
#include "misc.h"
#include "getput.h"

/*
 * Functions for get/put short/long. Pointer is _NOT_ advanced.
 */

#define GETSHORT(s, cp) {            \
        BYTE *t_cp = (BYTE*)(cp);    \
        (s) = ((WORD)t_cp[0] << 8)   \
            | ((WORD)t_cp[1]);       \
      }

#define GETLONG(l, cp) {             \
        BYTE *t_cp = (BYTE*)(cp);    \
        (l) = ((DWORD)t_cp[0] << 24) \
            | ((DWORD)t_cp[1] << 16) \
            | ((DWORD)t_cp[2] << 8)  \
            | ((DWORD)t_cp[3]);      \
      }

#define PUTSHORT(s, cp) {            \
        WORD  t_s  = (WORD)(s);      \
        BYTE *t_cp = (BYTE*)(cp);    \
        *t_cp++ = t_s >> 8;          \
        *t_cp   = t_s;               \
      }

#define PUTLONG(l, cp) {             \
        DWORD t_l  = (DWORD)(l);     \
        BYTE *t_cp = (BYTE*)(cp);    \
        *t_cp++ = (BYTE)(t_l >> 24); \
        *t_cp++ = (BYTE)(t_l >> 16); \
        *t_cp++ = (BYTE)(t_l >> 8);  \
        *t_cp   = (BYTE)t_l;         \
      }

WORD _getshort (const BYTE *x)   /* in <arpa/nameserv.h> */
{
  WORD res;
  GETSHORT (res, x);
  return (res);
}

DWORD _getlong (const BYTE *x)   /* in <arpa/nameserv.h> */
{
  DWORD res;
  GETLONG (res, x);
  return (res);
}

void __putshort (WORD var, BYTE *ptr)   /* in <resolv.h> */
{
  PUTSHORT (var, ptr);
}

void __putlong (DWORD var, BYTE *ptr)   /* in <resolv.h> */
{
  PUTLONG (var, ptr);
}

DWORD htonl (DWORD val) { return intel(val); }
DWORD ntohl (DWORD val) { return intel(val); }
WORD  htons (WORD  val) { return intel16(val); }
WORD  ntohs (WORD  val) { return intel16(val); }

WORD inchksum (const void *ptr, int len)
{
#if (DOSX)
  return _w32_inchksum_fast (ptr, len);
#else
  return _w32_inchksum (ptr, len);
#endif
}

/*
 * For old compatibility (linking old .o-files with new libwatt.a)
 */

DWORD intel  (DWORD val) { return htonl(val); }
WORD  intel16(WORD  val) { return htons(val); }

