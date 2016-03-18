#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "xfloat.h"

#ifdef __SW_BW
    #include <wdefwin.h>
    #define PROG_ABORT( num )   { printf( "Line: %d\n"                      \
                                          "Abnormal termination.\n", num ); \
                                  exit( -1 ); }
#else
    #define PROG_ABORT( num )   { printf( "Line: %d\n", num ); exit(-1); }
#endif

typedef union
{
    float f;
    char  bits[sizeof(float)];
    unsigned long lbits;
} F4;

typedef union
{
    double d;
    char  bits[sizeof(double)];
    unsigned __int64 llbits;
} F8;

#ifdef _LONG_DOUBLE_
typedef union
{
    long_double d;
    char  bits[sizeof(long_double)];
    unsigned __int64 llbits;
} F10;

static void OutputF10(F10, char *);
static void OutputLongDoubleSeries(void);
#endif

static void OutputF4(F4, char *);
static void OutputF8(F8, char *);
static void OutputDoubleSeries(void);
static void OutputFloatSeries(void);

#ifdef _LONG_DOUBLE_

static void OutputF10(F10 f, char *desc)
{
    int i;

    printf("%13.13s : ", desc);
    for (i = 10 - 1; i >= 0; i--)
        printf("%2.2X ", (f.bits[i] & 0xFF));
    printf("[%+21.21LE]\n", f.d);
}

static void OutputLongDoubleSeries(void)
{
    F10 f3;

    f3.llbits = 0xFFFFFFFFFFFFFFFFUI64;
    f3.d.exponent = 0xFFFF;
    OutputF10(f3, "-Greatest NAN");

    f3.llbits = 0x8000000000000001UI64;
    f3.d.exponent = 0xFFFF;
    OutputF10(f3, "-Smallest NAN");

    f3.llbits = 0x8000000000000000UI64;
    f3.d.exponent = 0xFFFF;
    OutputF10(f3, "-Infinity");

    f3.llbits = 0xFFFFFFFFFFFFFFFFUI64;
    f3.d.exponent = 0xFFFE;
    OutputF10(f3, "-Greatest Mag");

    f3.llbits = 0x8000000000000000UI64;
    f3.d.exponent = 0xBFFF;
    OutputF10(f3, "-1.0");

    f3.llbits = 0x8000000000000000UI64;
    f3.d.exponent = 0x8001;
    OutputF10(f3, "-Smallest Mag");

    f3.llbits = 0x7FFFFFFFFFFFFFFFUI64;
    f3.d.exponent = 0x8000;
    OutputF10(f3, "-Largest Den");

    f3.llbits = 0x0000000000000001UI64;
    f3.d.exponent = 0x8000;
    OutputF10(f3, "-Smallest Den");

    f3.llbits = 0x0000000000000000UI64;
    f3.d.exponent = 0x8000;
    OutputF10(f3, "-0.0");

    f3.llbits = 0x0000000000000000UI64;
    f3.d.exponent = 0x0000;
    OutputF10(f3, "+0.0");

    f3.llbits = 0x0000000000000001UI64;
    f3.d.exponent = 0x0000;
    OutputF10(f3, "Smallest Den");

    f3.llbits = 0x7FFFFFFFFFFFFFFFUI64;
    f3.d.exponent = 0x0000;
    OutputF10(f3, "Largest Den");

    f3.llbits = 0x8000000000000000UI64;
    f3.d.exponent = 0x0001;
    OutputF10(f3, "Smallest Mag");

    f3.llbits = 0x8000000000000000UI64;
    f3.d.exponent = 0x3FFF;
    OutputF10(f3, "1.0");

    f3.llbits = 0xFFFFFFFFFFFFFFFFUI64;
    f3.d.exponent = 0x7FFE;
    OutputF10(f3, "Greatest Mag");

    f3.llbits = 0x8000000000000000UI64;
    f3.d.exponent = 0x7FFF;
    OutputF10(f3, "Infinity");

    f3.llbits = 0x8000000000000001UI64;
    f3.d.exponent = 0x7FFF;
    OutputF10(f3, "Smallest NAN");

    f3.llbits = 0xFFFFFFFFFFFFFFFFUI64;
    f3.d.exponent = 0x7FFF;
    OutputF10(f3, "Greatest NAN");
}

#endif

static void OutputDoubleSeries(void)
{
    F8 f2;

    f2.llbits = 0xFFFFFFFFFFFFFFFFUI64;
    OutputF8(f2, "-Greatest NAN");

    f2.llbits = 0xFFF0000000000001UI64;
    OutputF8(f2, "-Smallest NAN");

    f2.llbits = 0xFFF0000000000000UI64;
    OutputF8(f2, "-Infinity");

    f2.llbits = 0xFFEFFFFFFFFFFFFFUI64;
    OutputF8(f2, "-Greatest Mag");

    f2.llbits = 0xBFF0000000000000UI64;
    OutputF8(f2, "-1.0");

    f2.llbits = 0x8010000000000000UI64;
    OutputF8(f2, "-Smallest Mag");

    f2.llbits = 0x800FFFFFFFFFFFFFUI64;
    OutputF8(f2, "-Largest Den");

    f2.llbits = 0x8000000000000001UI64;
    OutputF8(f2, "-Smallest Den");

    f2.llbits = 0x8000000000000000UI64;
    OutputF8(f2, "-0.0");

    f2.llbits = 0x0000000000000000UI64;
    OutputF8(f2, "+0.0");

    f2.llbits = 0x0000000000000001UI64;
    OutputF8(f2, "Smallest Den");

    f2.llbits = 0x000FFFFFFFFFFFFFUI64;
    OutputF8(f2, "Largest Den");

    f2.llbits = 0x0010000000000000UI64;
    OutputF8(f2, "Smallest Mag");

    f2.llbits = 0x3FF0000000000000UI64;
    OutputF8(f2, "1.0");

    f2.llbits = 0x7FEFFFFFFFFFFFFFUI64;
    OutputF8(f2, "Greatest Mag");

    f2.llbits = 0x7FF0000000000000UI64;
    OutputF8(f2, "Infinity");

    f2.llbits = 0x7FF0000000000001UI64;
    OutputF8(f2, "Smallest NAN");

    f2.llbits = 0x7FFFFFFFFFFFFFFFUI64;
    OutputF8(f2, "Greatest NAN");
}

static void OutputFloatSeries(void)
{
    F4 f1;

    f1.lbits = 0xFFFFFFFF;
    OutputF4(f1, "-Greatest NAN");

    f1.lbits = 0xFF800001;
    OutputF4(f1, "-Smallest NAN");

    f1.lbits = 0xFF800000;
    OutputF4(f1, "-Infinity");

    f1.lbits = 0xFF7FFFFF;
    OutputF4(f1, "-Greatest Mag");

    f1.lbits = 0xBF800000;
    OutputF4(f1, "-1.0");

    f1.lbits = 0x80800000;
    OutputF4(f1, "-Smallest Mag");

    f1.lbits = 0x807FFFFF;
    OutputF4(f1, "-Largest Den");

    f1.lbits = 0x80000001;
    OutputF4(f1, "-Smallest Den");

    f1.lbits = 0x80000000;
    OutputF4(f1, "-0.0");

    f1.lbits = 0x00000000;
    OutputF4(f1, "+0.0");

    f1.lbits = 0x00000001;
    OutputF4(f1, "Smallest Den");

    f1.lbits = 0x007FFFFF;
    OutputF4(f1, "Largest Den");

    f1.lbits = 0x00800000;
    OutputF4(f1, "Smallest Mag");

    f1.lbits = 0x3F800000;
    OutputF4(f1, "1.0");

    f1.lbits = 0x7F7FFFFF;
    OutputF4(f1, "Greatest Mag");

    f1.lbits = 0x7F800000;
    OutputF4(f1, "Infinity");

    f1.lbits = 0x7F800001;
    OutputF4(f1, "Smallest NAN");

    f1.lbits = 0x7FFFFFFF;
    OutputF4(f1, "Greatest NAN");
}

static void OutputF4(F4 f, char *desc)
{
    int i;

    printf("%13.13s : ", desc);
    for (i = sizeof(f) - 1; i >= 0; i--)
        printf("%2.2X ", (f.bits[i] & 0xFF));
    printf("[%+7.7E]\n", f.f);
}

static void OutputF8(F8 f, char *desc)
{
    int i;

    printf("%13.13s : ", desc);
    for (i = sizeof(f) - 1; i >= 0; i--)
        printf("%2.2X ", (f.bits[i] & 0xFF));
    printf("[%+16.16E]\n", f.d);
}

int main( int argc, char **argv )
{
#ifdef __SW_BW
    FILE *my_stdout;
    my_stdout = freopen( "tmp.log", "a", stdout );
    if( my_stdout == NULL ) {
        fprintf( stderr, "Unable to redirect stdout\n" );
        exit( -1 );
    }
#endif
    OutputFloatSeries();
    putchar('\n');
    OutputDoubleSeries();
#ifdef _LONG_DOUBLE_
    putchar('\n');
    OutputLongDoubleSeries();
#endif

    printf( "Tests completed (%s).\n", strlwr( argv[0] ) );
#ifdef __SW_BW
    fprintf( stderr, "Tests completed (%s).\n", strlwr( argv[0] ) );
    fclose( my_stdout );
    _dwShutDown();
#endif
    return 0;
}
