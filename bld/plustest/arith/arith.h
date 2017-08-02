// CONFIGURATION FOR ARITH TESTS

#ifndef __ARITH_H__
#   define __ARITH_H__

#   ifdef __WATCOM_INT64__
        typedef signed  __int64 SBIG;
        typedef unsigned __int64 UBIG;
#   else
        typedef signed   long SBIG;
        typedef unsigned long UBIG;
#   endif

#endif
