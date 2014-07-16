// pp27.c -- preprocessor test # 27
//
//
#include "fail.h"

main() {


// defined

#if defined( me ) || defined( you )
  fail( __LINE__ );
#endif

#if defined me || defined( you )
  fail( __LINE__ );
#endif

#define me

#if defined( me ) && !defined( you )
#else
  fail( __LINE__ );
#endif

#if defined me || defined( you )
#else
  fail( __LINE__ );
#endif

#if defined (^)
  fail( __LINE__ );
#endif


// unary

#if !1 
  fail( __LINE__ );
#endif

#if -1 > 0
  fail( __LINE__ );
#endif

#if !0 
#else
  fail( __LINE__ );
#endif

#if a*-1 == -a
#else
  fail( __LINE__ );
#endif

#if a != +a
  fail( __LINE__ );
#endif

#if - -a != +a
  fail( __LINE__ );
#endif


#if __WATCOM_REVISION__ >= 8

#if (0x00000011 != ~0xffffffffffffffee) // int64
    fail( __LINE__ );
#endif

#else // __WATCOM_REIVION__ < 8

#if (0x00000011 != ~0xffffffee)
    fail( __LINE__ );
#endif

#endif

// multiplicitive

#define a 42
#define b 6

#if a == b*7
#else
  fail( __LINE__ );
#endif

#if a != b*7
  fail( __LINE__ );
#endif

#if b == a/7
#else
  fail( __LINE__ );
#endif

#if b != a/7
  fail( __LINE__ );
#endif

#if 4 % 3 != 1
  fail( __LINE__ );
#endif


// additive

#if 4+1 == 8-3
#else
  fail( __LINE__ );
#endif

// shift

#if 0x1 << 3 != 0x8
  fail( __LINE__ );
#endif

#if 0x8 >> 3 != 0x1
  fail( __LINE__ );
#endif

#if 0x8 >> 4 != 0
  fail( __LINE__ );
#endif

// relational
#define small -4
#define medium 2
#define large 12

#if !(small < large)
  fail( __LINE__ );
#endif

#if !(small <= large)
  fail( __LINE__ );
#endif

#if small > large
  fail( __LINE__ );
#endif

#if small >= large
  fail( __LINE__ );
#endif

#if -234234 > -12
  fail( __LINE__ );
#endif

#if -234234 > 4
  fail( __LINE__ );
#endif

#if 0x8 >> 4 != 0
  fail( __LINE__ );
#endif

#if !(small >= -4 )
  fail( __LINE__ );
#endif

#if !(5 <= +5 )
  fail( __LINE__ );
#endif

// bitwise and

#if !(0x101 & 0x011 )
  fail( __LINE__ );
#endif

#if 0xf0f & 0x0e0 
  fail( __LINE__ );
#endif

// bitwise XOR

#if 0xf0f ^ 0x0b0
#else
  fail( __LINE__ );
#endif

#if ( 0xf ^ 0x4 ) != 0xb
  fail( __LINE__ );
#endif

#if 0xf ^ 0x4 != 0xb
#else
  fail( __LINE__ );
#endif

// bitwise OR

#if ( 0xf0f | 0x0b0 ) != 0xfbf
  fail( __LINE__ );
#endif

#if 0xf0f | 0x0b0 != 0xfbf
#else
  fail( __LINE__ );
#endif

// logical and

#if ( 2==4/2 && 5!=3 && 2>-4 && 1-2 )
#else
  fail( __LINE__ );
#endif

#if ( 4 && 3>=2 && 3>6 )
  fail( __LINE__ );
#endif

// logical or


#if !defined( me ) || 3>4
  fail( __LINE__ );
#endif

#if 3>4 || defined( me )
#else
  fail( __LINE__ );
#endif

// conditional

#if 1 ? 0 : 1
  fail( __LINE__ );
#endif

#if ((((((1) ? (0) : ((1)))))))
  fail( __LINE__ );
#endif

#if 0 ? 0 : 1
#else
  fail( __LINE__ );
#endif


  _PASS;

}
