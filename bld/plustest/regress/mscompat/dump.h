#include <stdio.h>
#include <stdlib.h>

static char  bad_format_string[] = { "FAIL line %d, file " };
static char good_format_string[] = { "PASS line %d\n" };
static int  good_checksum_value = 0;

static unsigned errors;

#define BAD  { printf( bad_format_string, __LINE__ ); puts( __FILE__ ); ++errors; }
#define GOOD { printf( good_format_string, __LINE__ ); good_checksum_value += __LINE__; }
#define FORCE_GOOD(line) { printf( good_format_string, line ); good_checksum_value += line; }
#define CHECK_GOOD( __cgv__ ) \
    if( __cgv__ != good_checksum_value ) { \
	printf( "FAILURE: checksum mismatch, wanted=%d, got=%d\n", \
	        __cgv__, good_checksum_value ); \
    ++errors; \
    } else { \
	puts( "SUCCESS" ); \
    }
