:: "The Alpha NT version (the only one we have) has 32-bit long ints and
:: 64-bit long long ints. Essentially the system behaves just like a
:: 32-bit machine (pointers are only 32-bit, too), except long longs fit
:: in registers. The PPC port was only targeted for 32-bit PPC systems,
:: it's much like a 386."
/*          char | short | int | long | long long                        */
/* 16 bit : 8      16      16    32     64     All 16 bit targets.       */
/* 32 bit : 8      16      32    32     64     386, AXP, PPC, MIPS       */
/* 64 bit : 8      16      32    64     64     No targets implemented.   */

/* Exact-width types. */
:: Some of these types are also defined in sys/types.h. However, protection
:: with the _EXACT_WIDTH_INTS is not needed in cstdint because the types are
:: declared in namespace std (and thus can't conflict with the global types
:: in sys/types.h anyway). In fact, if both sys/types.h and cstdint are
:: included we want the exact width integer types declared in both namespaces.
:segment !CNAME
#ifndef _EXACT_WIDTH_INTS
#define _EXACT_WIDTH_INTS
:endsegment
typedef signed char         int8_t;
typedef unsigned char       uint8_t;
typedef short               int16_t;
typedef unsigned short      uint16_t;
#ifdef _M_I86
 typedef long               int32_t;
 typedef unsigned long      uint32_t;
#else
 typedef int                int32_t;
 typedef unsigned int       uint32_t;
#endif
typedef long long           int64_t;
typedef unsigned long long  uint64_t;
:segment !CNAME
#endif /* _EXACT_WIDTH_INTS */
:endsegment
