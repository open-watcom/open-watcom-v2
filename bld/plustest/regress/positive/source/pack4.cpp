// test packing pragmas
//
// 95/04/20 -- J.W.Welch        -- conditionals for different systems

#if __WATCOMC__ > 1060
    #if defined( __ALPHA__ )
        #define DEF_PACK_AMT 8
    #elif defined( _M_I86 )
        #define DEF_PACK_AMT 2
    #elif defined( __386__ )
	#if __WATCOM_REVISION__ >= 8
	    #define DEF_PACK_AMT 8
	#else
	    #define DEF_PACK_AMT 4
	#endif
    #else
        #error Bad Target
    #endif
#else
    #define DEF_PACK_AMT 1
#endif

#include "fail.h"
#include <stddef.h>

#define __TEST(n,e) \
    struct X##n { \
        char c; \
        double d; \
        X##n() { \
            if( offsetof( X##n,d) != e ) fail(__LINE__); \
        } \
    } _X##n;
#define _TEST( n, e )   __TEST(n,e)
#define TEST( e )       _TEST(__LINE__,e)

// G:1 C:1 S:

#pragma pack(4)
// G:1 C:4 S:

TEST( 4 )

#pragma pack(8)
// G:1 C:8 S:

TEST(8)

#pragma pack()
// G:1 C:1 S:

TEST(DEF_PACK_AMT)

#pragma pack(4)
// G:1 C:4 S:

TEST( 4 )

#pragma pack(push)
// G:1 C:4 S:4

TEST( 4 )

#pragma pack(8)
// G:1 C:8 S:4

TEST(8)

#pragma pack(4)
// G:1 C:4 S:4

TEST( 4 )

#pragma pack(2)
// G:1 C:2 S:4

TEST( 2 )

#pragma pack()
// G:1 C:1 S:4

TEST( DEF_PACK_AMT )

#pragma pack(pop)
// G:1 C:4 S:

TEST( 4 )

#pragma pack()
// G:1 C:1 S:

TEST( DEF_PACK_AMT )

#pragma pack(2)
// G:1 C:2 S:

TEST( 2 )

#pragma pack(push,8)
// G:1 C:8 S:2

TEST( 8 )

#pragma pack(pop)
// G:1 C:2 S:

TEST( 2 )

#pragma pack()
// G:1 C:1 S:

TEST( DEF_PACK_AMT )

int main() {
    _PASS;
}
