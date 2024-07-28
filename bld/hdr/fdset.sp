#ifndef _FDSET_DEFINED
#define _FDSET_DEFINED

/* The fd_set member is required to be an array of longs.  */
typedef long fd_mask;

#define FD_SETSIZE          1024
#define __NFDBITS           (8 * sizeof (fd_mask))
#define __FDELT(__d)        ((__d) / __NFDBITS)
#define __FDMASK(__d)       ((fd_mask) 1 << ((__d) % __NFDBITS))
#define __FDS_BITS(__set)   ((__set)->fds_bits)

/* fd_set for select */
typedef struct {
    fd_mask fds_bits[FD_SETSIZE / __NFDBITS];
} fd_set;

:: We don't use `memset' because this would require a prototype and
:: the array isn't too big.
#define FD_ZERO(__set) \
    do { \
        unsigned int __i; \
        fd_set *__arr = (__set); \
        for(__i = 0; __i < sizeof(fd_set) / sizeof(fd_mask); ++__i) \
            __FDS_BITS(__arr)[__i] = 0; \
    } while( 0 )
#define FD_SET(__d,__set)   (__FDS_BITS(__set)[__FDELT(__d)] |= __FDMASK(__d))
#define FD_CLR(__d,__set)   (__FDS_BITS(__set)[__FDELT(__d)] &= ~__FDMASK(__d))
#define FD_ISSET(__d,__set) (__FDS_BITS(__set)[__FDELT(__d)] & __FDMASK(__d))

#endif  /* !_FDSET_DEFINED */
