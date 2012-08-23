/* sys/param.h (wattcp) */

#ifndef __SYS_PARAM_H
#define __SYS_PARAM_H

#ifndef PAGE_SIZE
#define PAGE_SIZE  0x1000
#endif

#ifndef HZ
#define HZ         100
#endif

#ifndef MAXNAMLEN
#define MAXNAMLEN  260
#endif

#ifndef MAXPATHLEN
#define MAXPATHLEN 260
#endif

#ifndef __SYS_SWAP_BYTES_H
#include <sys/swap.h>
#endif

#endif
