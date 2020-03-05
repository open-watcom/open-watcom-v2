/*
 * The generated lang.c must be included below (and not compiled
 * alone). This is needed to work around different definitions
 * of e.g. INT8_MAX. In Watcom's <stdint.h>:
 *   \pre #define INT_MAX 127
 *
 * and in Flex generated lang.c:
 *   \pre #define INT8_MAX  (127)
 *
 * The Watcom preprossesor doesn't understand they're equal :-(
 *
 * Besides, the following YY_defines makes the code a bit smaller.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <setjmp.h>

#if defined(_MSC_VER) || defined(__TURBOC__) || defined(__BORLANDC__)
  #define YY_NO_UNISTD_H

#elif defined(__WATCOMC__)
  #include <stdint.h>

#elif defined(__BORLANDC__)
  /*
   * Refer Makefile.all for the '-DFLEXINT_H=1' etc. hacks.
   */
#endif

#define YY_NO_INPUT           1
#define YY_MAIN               0  /* no main() function */
#define YY_STACK_USED         0  /* only in some old Flex'er */
#define YY_ALWAYS_INTERACTIVE 0
#define YY_NEVER_INTERACTIVE  1

#include "wattcp.h"
#include "strings.h"
#include "sock_ini.h"
#include "misc.h"
#include "crc.h"

#if defined(USE_LANGUAGE)
  #include "language.h"
  #include "lang.c"
#endif
