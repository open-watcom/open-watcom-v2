/*
 * The naming <sys/wxx.h> is required for those compilers that
 * have <sys/xx.h> in the usual place but doesn't define stuff
 * related to Waterloo's BSD-socket interface.
 */

#ifndef __SYS_WERRNO_H
#include <sys/werrno.h>
#endif

/*
 * This only work with gcc's pre-processor
 *
 * #ifdef __GNUC__
 * #include_next <sys/errno.h>
 * #endif
 *
 */
