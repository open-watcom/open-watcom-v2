#ifndef __SYS_SWAP_BYTES_H
#define __SYS_SWAP_BYTES_H

#ifndef __SYS_CDEFS_H
#include <sys/cdefs.h>
#endif

#ifndef W32_NAMESPACE
#define W32_NAMESPACE(x) __CONCAT(_w32_,x)
#endif

__BEGIN_DECLS

#if !defined(__dj_include_netinet_in_h_)
  #undef  ntohs
  #undef  htons
  #undef  ntohl
  #undef  htonl
  #define ntohs(x)  intel16(x)
  #define htons(x)  intel16(x)
  #define ntohl(x)  intel(x)
  #define htonl(x)  intel(x)
#endif

 /*
  * Hard to believe, but someone uses Watt-32 on a
  * Motorola/PowerPC embedded target.
  */
#if defined(BIG_ENDIAN_MACHINE) || defined(USE_BIGENDIAN)
  #define intel(x)    x
  #define intel16(x)  x

#elif defined(__GNUC__)
  #define intel(x)   __ntohl(x)
  #define intel16(x) __ntohs(x)

  /*
   * Ripped (and adapted) from <linux/include/asm-386/byteorder.h>
   */
  /*@unused@*/ static __inline__ unsigned long __ntohl (unsigned long x)
  {
    __asm__ ("xchgb %b0, %h0\n\t"   /* swap lower bytes  */
             "rorl  $16,%0\n\t"     /* swap words        */
             "xchgb %b0,%h0"        /* swap higher bytes */
            : "=q" (x) : "0" (x));
    return (x);
  }

  /*@unused@*/ static __inline__ unsigned short __ntohs (unsigned short x)
  {
    __asm__ ("xchgb %b0, %h0"       /* swap bytes */
            : "=q" (x) : "0" (x));
    return (x);
  }

#elif defined(__WATCOMC__) && defined(__FLAT__) /* Watcom 32-bit */
  #define intel(x)   __ntohl(x)
  #define intel16(x) __ntohs(x)

  extern unsigned long __ntohl (unsigned long x);
  #pragma aux  __ntohl =     \
              "xchg al, ah"  \
              "ror  eax, 16" \
              "xchg al, ah"  \
              parm   [eax]   \
              modify [eax];

  extern unsigned short __ntohs (unsigned short x);
  #pragma aux __ntohs = \
              "xchg al, ah" \
              parm   [ax]   \
              modify [ax];

#elif defined(__WATCOMC__) && !defined(__FLAT__) /* Watcom 16-bit */
  #define intel(x)   __ntohl(x)
  #define intel16(x) __ntohs(x)

  extern unsigned long __ntohl (unsigned long x);
  #pragma aux  __ntohl =     \
              "xchg al, dh"  \
              "xchg ah, dl"  \
              parm   [dx ax] \
              modify [dx ax];

  extern unsigned short __ntohs (unsigned short x);
  #pragma aux __ntohs =     \
              "xchg al, ah" \
              parm   [ax]   \
              modify [ax];  

#else  /* no inlining possible */
  #define intel   W32_NAMESPACE (intel)
  #define intel16 W32_NAMESPACE (intel16)

  extern unsigned long  intel   (unsigned long x);
  extern unsigned short intel16 (unsigned short x);
#endif

__END_DECLS

#endif /* __SYS_SWAP_BYTES_H */
