#ifndef __CHECK_SUM_H
#define __CHECK_SUM_H

#define inchksum      NAMESPACE (inchksum)
#define inchksum_fast NAMESPACE (inchksum_fast)
#define do_checksum   NAMESPACE (do_checksum)

extern WORD inchksum    (const void *ptr, int len);
extern int  do_checksum (BYTE *buf, int proto, int len);

#ifndef cdecl
#define cdecl  /* prevent register calls */
#endif

/* chksum0.asm / chksum0.s
 */
#if (DOSX)
  extern WORD cdecl inchksum_fast (const void *ptr, int len);
  #define checksum(p, len) inchksum_fast (p, len)
#else
  #define checksum(p, len) inchksum (p, len)
#endif

#endif
