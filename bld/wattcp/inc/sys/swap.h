#ifndef __SYS_SWAP_BYTES_H
#define __SYS_SWAP_BYTES_H

#ifndef __SYS_CDEFS_H
#include <sys/cdefs.h>
#endif

#ifndef W32_NAMESPACE
#define W32_NAMESPACE(x) __CONCAT(_w32_,x)
#endif

__BEGIN_DECLS

#undef  ntohs
#undef  htons
#undef  ntohl
#undef  htonl

#define ntohs(x)    __ntohs(x)
#define htons(x)    __ntohs(x)
#define ntohl(x)    __ntohl(x)
#define htonl(x)    __ntohl(x)

#define intel(x)    __ntohl(x)
#define intel16(x)  __ntohs(x)

extern unsigned long __ntohl (unsigned long x);
#if defined(_M_I86) /* Watcom 16-bit */
  #pragma aux  __ntohl =     \
              "xchg al, dh"  \
              "xchg ah, dl"  \
              __parm   [__dx __ax] \
              __modify [__dx __ax];
#else /* Watcom 32-bit */
  #pragma aux  __ntohl =     \
              "xchg al, ah"  \
              "ror  eax, 16" \
              "xchg al, ah"  \
              __parm   [__eax]   \
              __modify [__eax];
#endif

extern unsigned short __ntohs (unsigned short x);
#pragma aux __ntohs =     \
              "xchg al, ah" \
              __parm   [__ax]   \
              __modify [__ax];

__END_DECLS

#endif /* __SYS_SWAP_BYTES_H */
