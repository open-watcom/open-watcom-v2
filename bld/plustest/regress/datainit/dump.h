#include <stdio.h>
#define DUMP( _p_ ) {\
    puts( __FILE__ ); \
    puts( # _p_ );\
    unsigned n = 0; \
    for( int i = 0 ; i < sizeof( (_p_) ) ; i++ ) {\
	if( *(((char *)(&(_p_)))+i) ) {\
	    ++n;\
	}\
    }\
    if( n ) {\
	puts( "non-zero" );\
    } else {\
	puts( "all-zero" );\
    }\
}

static char  bad_format_string[] = { "FAIL line %d, file " };
static char good_format_string[] = { "PASS line %d\n" };
static int  good_checksum_value = 0;

#define BAD  { printf( bad_format_string, __LINE__ ); puts( __FILE__ ); }
#if 1
#define GOOD { printf( good_format_string, __LINE__ ); good_checksum_value += __LINE__; }
#else
#define GOOD { good_checksum_value += __LINE__; }
#endif
#define CHECK_GOOD( __cgv__ ) \
    if( __cgv__ != good_checksum_value ) { \
	printf( "FAILURE: checksum mismatch, wanted=%d, got=%d\n", \
	        __cgv__, good_checksum_value ); \
    } else { \
	puts( "SUCCESS" ); \
    }

struct WMI {
    WMI( const char *p ) {
	puts( p );
    }
};
#define __wmi	WMI __w(__FILE__);
