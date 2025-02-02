// pp28.c -- preprocessor test # 28
//
//
#include <limits.h>
#include "fail.h"

main() {

#if -3 > 3U
#else
  fail( __LINE__ );
#endif

#if -3 > 3
  fail( __LINE__ );
#endif

#if ULONGLONG_MAX - 3i64 != ~3ui64
  fail( __LINE__ );
#endif

#if ULONGLONG_MAX - 3i64 != ~3ui64
  fail( __LINE__ );
#endif

#if UINT_MAX + 1 - 1  != UINT_MAX
  fail( __LINE__ );
#endif


  _PASS;

}
