/*
  test for constraint checking for bit-fields
*/
typedef unsigned char UC;
typedef unsigned short US;
typedef unsigned long UL;
typedef signed char SC;
typedef signed short SS;
typedef signed long SL;

struct S1 {
    UC
    	bc0 : -1,
	bc1 : 8-1,
	bc2 : 8,
	bc3 : 8+1;
    SC
	bc4 : 8-1,
	bc5 : 8,
	bc6 : 8+1;
    US
    	bs0 : -1,
	bs1 : 16-1,
	bs2 : 16,
	bs3 : 16+1;
    SS
	bs4 : 16-1,
	bs5 : 16,
	bs6 : 16+1;
    UL
    	bl0 : -1,
	bl1 : 32-1,
	bl2 : 32,
	bl3 : 32+1;
    SL
	bl4 : 32-1,
	bl5 : 32,
	bl6 : 32+1;
} v1;

struct S2 {
    nb0 : 1,
    nb1 : 2;
    friend q1 : 1;
    virtual q2 : 1;
    inline q3 : 1;
    const c1 : 1;
    volatile v1 : 1;
    const volatile cv1 : 1;
    typedef int
	TI,
	t1 : 1,
	ZI;
    S1 bt : 1;
    int zb : 0;
} v2;

struct S3 {
    UC f1;
    int : 0;
    UC
    	bc0 : 1,
	    : 2,
	bc1 : 2,
	    : 0,
	bc2 : 3,
	bc3 : 4,
	bc4 : 5,
	bc5 : 6,
	bc6 : 7,
	bc7 : 8;
    SC
    	bc8 : 1,
	bc9 : 2,
	bca : 3,
	bcb : 4,
	bcc : 5,
	bcd : 6,
	bce : 7,
	bcf : 8;
    US
    	bs0 : 1,
	bs1 : 2,
	bs2 : 3,
	bs3 : 4,
	bs4 : 5,
	bs5 : 6,
	bs6 : 7,
	bs7 : 8;
    SS
    	bs8 : 1,
	bs9 : 2,
	bsa : 3,
	bsb : 4,
	bsc : 5,
	bsd : 6,
	bse : 7,
	bsf : 8;
    UL
    	bl0 : 1,
	bl1 : 2,
	bl2 : 3,
	bl3 : 4,
	bl4 : 5,
	bl5 : 6,
	bl6 : 7,
	bl7 : 8;
    SL
    	bl8 : 1,
	bl9 : 2,
	bla : 3,
	blb : 4,
	blc : 5,
	bld : 6,
	ble : 7,
	blf : 8;
} v3;
