/**
 * If the preprocessor works properly, this should fail epically.
 *
 * In another life, this was part of positive/pp19.c
 */

#define A2 0x02
#define A3 0xaa0e+A2
enum {
    A7 = 1,
};
#define A7	0xaa0e+A7

#define A16( a,b )	(b+a)
#define A17 0xaa0e+A16 (1,2)

int a[] = {
    A3,	0xaa10,
    A7,	0xaa0f,
    A17,0xaa11,
};

