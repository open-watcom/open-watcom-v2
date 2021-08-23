#ifndef __CHECK_SUM_H
#define __CHECK_SUM_H

#define inchksum      NAMESPACE (inchksum)
#define inchksum_fast NAMESPACE (inchksum_fast)
#define do_checksum   NAMESPACE (do_checksum)

extern WORD inchksum    (const void *ptr, int len);
extern int  do_checksum (BYTE *buf, int proto, int len);

/* chksum0.asm
 */
#if (DOSX)
  extern WORD inchksum_fast(const void *ptr, int len);
  #pragma aux inchksum_fast "*" __parm [__esi] [__ecx]
  #define checksum(p, len) inchksum_fast (p, len)
#else
  #define checksum(p, len) inchksum (p, len)
#endif

#endif
