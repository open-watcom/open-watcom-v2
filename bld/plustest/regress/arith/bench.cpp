#ifndef __WATCOM_INT64__
#include <stdio.h>

int main() {
    FILE *fp = fopen( "bench.chk", "r" );
    int c;
    for(;;) {
        c = fgetc( fp );
        if( c == EOF ) break;
        putchar( c );
    }
    fclose( fp );
    return 0;
}
#else
/*
                This program can be used to check how well
        gcc optimizes some 64-bit arithmetic constructs.

                Procedure NR_MUL does modular multiplication of
        multiple precision operands, as used (for example)
        by the RSA cryptosystem.
        In my integer factorization program, 50-90% of the
        time is spent here.  An argument to the procedure
        specifies one of three algorithms for arithmetic:

                MULT_DBLEINT -  Use long long data type.
                                This algorithm takes the
                                64-bit product of 32-bit
                                arguments, and manipulates
                                such products.
                                It needs addition, multiplication,
                                shifting, masking.

                MULT_HALFINT -  To multiply single-precision
                                arguments, each is broken
                                into half-sized pieces, which
                                are individually multiplied.

                MULT_DOUBLE_PRECISION - Floating point arithmetic
                                is used for intermediate calculations.

                This version of NR_MUL is a simplified version of the
        real one, which includes checks for squaring a number
        and whose choice of algorithm is made at compilation
        time rather than execution time.
        More importantly, the real one allows radix 2^30 for
        MULT_DBLEINT, 2^28 for MULT_HALFINT, but only 2^26
        for MULT_DOUBLE_PRECISION (assuming 53-bit
        double precision floating mantissa, as in IEEE).
        So a 100-digit (= 332-bit) number will need
        length 12 for MULT_DBLEINT, 13 for MULT_HALFINT,
        but 14 for MULT_DOUBLE_PRECISION.

                On a DEC 5000 (MIPS R3000 architecture), the last output
        line shows times of 11249, 937, and 703 microseconds
        when lng = 14, for MULT_DBLEINT, MULT_HALFINT,
        and MULT_DOUBLE_PRECISION, respectively.
        The pixstats output shows 59% of the time in bmul and __muldi3,
        18% in badd and __adddi3, 17% in NR_MUL, 4% in bzero, 2% in __lshrdi3

                Presently 20% of the time is being spent in badd,
        __adddi3, and __lshrdi3; inlining these routines
        (as you have done for gcc 2) will cause a 15%-25% speed-up.
        When bmul, bzero, and __muldi3 are similarly inlined,
        the overall speed-up should be 2-fold to 3-fold,
        on machines with hardware integer multiply,
        and the MULT_DBLEINT time will hopefully
        be comparable with the others, if not better.

                Thank you for your consideration.

                        Peter L. Montgomery
                        Department of Mathematics
                        University of California
                        Los Angeles, CA 90024-1555 USA
                        pmontgom@math.ucla.edu

*/

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
/*
        Typedefs for 32-bit and 64-bit integers.
        A leading "s" identifies signed data;
        a trailing "c" identifies const data.
*/
#define LNGMAX 15

typedef unsigned long       mp_digit_t;
typedef   signed long      smp_digit_t;
typedef unsigned __int64    mp_digit2_t;
typedef   signed int        mp_lng_t;      /* For subscripts, lengths, etc. */

typedef const  mp_digit_t   mp_digit_tc;
typedef const  mp_digit2_t  mp_digit2_tc;
typedef const  mp_lng_t     mp_lng_tc;

#define CONSTP(type) type*const
                        /* Constant pointer to (possibly non-const) data */

#define LG2MPB  26
#define MP_BASE (1L << LG2MPB)
                /* Radix for multiple precision arithmetic */
                /* This assumes double precision floating is */
                /* at least 2*26 + 1 = 53 bits wide. */
#define MP_MASK (MP_BASE - 1)
#define LG2MPB_CHUNK (LG2MPB/2)
#if LG2MPB != 2*LG2MPB_CHUNK
    #error "LG2MPB must be even"
#endif

static const double MP_BASE_D = (double)MP_BASE;
static const double MP_BASE_DINV = 1.0/(double)MP_BASE;


#define MULT_DBLEINT 0
                /* Use long long data type for intermediate results */
#define MULT_HALFINT 1
                /* Partition operands into two half-size integers */
#define MULT_DOUBLE_PRECISION 2
                /* Use double precision floating point arithmetic */

#define MAX_MULT_ALG 2
static char *desc[1+MAX_MULT_ALG] = {"Long long", "Half ints", "Floating"};

void NR_MUL(CONSTP(mp_digit_tc) ar1,
            CONSTP(mp_digit_t ) ar2,
            CONSTP(mp_digit_tc) ar3,            /* Modulus */
            CONSTP(mp_digit_t ) ar4,            /* Product */
                   mp_digit_tc  inv3,
                   mp_lng_tc    lng,
                        int     algorithm)
{
/*
C               ar4 = (ar1*ar2)/MP_BASE**lng mod ar3.
C               We know inv3*ar3(0) == -1 mod MP_BASE.

C               See "Modular Multiplication Without Trial Division", by
C               Peter Montgomery, Mathematics of Computation, April, 1985.


C       General algorithm:
C
C           tmparr(0:lng-1) = 0
C           do j = 0, lng-1     ! j = 0 iteration is coded separately
C               mul2 = ar1(j)
C               quad = mul2*ar2(0) + tmparr(0)
C               mul3 = IAND(quad * inv3, MP_MASK)
C               carry = (quad + mul3*ar3(0))/MP_BASE   (exact)
C               do i = 1, lng-1
C
C                               0 <= mul2       <= MP_BASE-1
C                               0 <= mul3       <= MP_BASE-1
C                               0 <= ar2(i)     <= MP_BASE-1
C                               0 <= ar3(i)     <= MP_BASE-1
C                               0 <= tmparr(i)  <= MP_BASE-1 if i < lng-1
C                               0 <= carry      <= 2*(MP_BASE-1)
C
C                               0 <= quad       <= 2*MP_BASE**2-MP_BASE-1
C                                                            if i < lng-1
C                               0 <= tmparr(lng-1)<= 2*MP_BASE-1
C                               0 <= quad        <= 2*MP_BASE**2-1
C                                                            if i = lng-1
C                   quad = mul2*ar2(i) + mul3*ar3(i) + tmparr(i) + carry
C                   tmparr(i-1) = IAND(quad, MP_MASK)
C                   carry = RSHIFT(quad, LG2MPB)
C               end do
C               tmparr(lng-1) = carry
C Note tmparr(lng-1) may be as large as 2*MP_BASE-1.
C           end do
C
C           if (tmparr >= ar3) then
C               ar4 = tmparr - ar3      (multiple precision subtract)
C           else
C               ar4 = tmparr
C           end if
C
C - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
*/
                   mp_digit_t   tmparr[LNGMAX]; /* variable length */
            CONSTP(mp_digit_tc) par1    = ar1;
   register CONSTP(mp_digit_tc) par2    = ar2;
   register CONSTP(mp_digit_tc) par3    = ar3;
   register          mp_lng_t   i;
                     mp_lng_t   j;

    switch(algorithm) {
default: fprintf(stderr, "Bad algorithm value - %d\n", algorithm);
         exit(1);
         break;
case MULT_DBLEINT:      /* Use long long type */
#define NN_DPRODUU(nnm1, nnm2) (((mp_digit2_t)(nnm1))*((mp_digit2_t)(nnm2)))
    {
        register mp_digit_t  mul2, mul3;
        register mp_digit2_t quad, carry;

        mul2 = par1[0];
        quad = NN_DPRODUU(mul2, par2[0]);
        mul3 = (mp_digit_t)(quad & MP_MASK);
        mul3 = (mp_digit_t)(NN_DPRODUU(mul3, inv3) & MP_MASK);
        carry = (quad + NN_DPRODUU(mul3, par3[0])) >> LG2MPB;
        if (lng == 1) {
            tmparr[0] = (mp_digit_t)carry;
        } else {
            i = 1;
            do {
                quad =   NN_DPRODUU(mul2, par2[i])
                       + NN_DPRODUU(mul3, par3[i]) + carry;
                tmparr[i-1] = quad & MP_MASK;
                carry = quad >> LG2MPB;
            } while (++i < lng);
            tmparr[lng-1] = (mp_digit_t)carry;

            j = 1;
            do {
                mul2 = par1[j];
                quad = NN_DPRODUU(mul2, par2[0]) + (mp_digit2_t)tmparr[0];
                mul3 = (mp_digit_t)(quad & MP_MASK);
                mul3 = (mp_digit_t)(NN_DPRODUU(mul3, inv3) & MP_MASK);
                carry = (quad + NN_DPRODUU(mul3, par3[0]) >> LG2MPB);
                i = 1;
                do {
                    quad = NN_DPRODUU(mul2, par2[i]) + NN_DPRODUU(mul3, par3[i])
                           + (mp_digit2_t)(tmparr[i]) + carry;
                    tmparr[i-1] = quad & MP_MASK;
                    carry = quad >> LG2MPB;
                } while (++i < lng);
                tmparr[lng-1] = (mp_digit_t)carry;
            } while (++j < lng);
        } /* lng > 1 */
    }
    break;      /* MULT_DBLEINT */
#undef NN_DPRODUU

case MULT_HALFINT:              /* Partition everything into half-integers */

#define NN_LCHUNKU(nnm1) ((nnm1) & ((1L << LG2MPB_CHUNK)-1))
                /* Lower half of 32-bit integer */
#define NN_HCHUNKU(nnm1) ((nnm1) >> (LG2MPB_CHUNK))
                /* Upper half of 32-bit integer */
    {
        mp_digit_tc inv3h  = NN_HCHUNKU(inv3);
        mp_digit_tc inv3l  = NN_LCHUNKU(inv3);
        mp_digit_tc ar20h  = NN_HCHUNKU(par2[0]);
        mp_digit_tc ar20l  = NN_LCHUNKU(par2[0]);
        mp_digit_tc ar30h  = NN_HCHUNKU(par3[0]);
        mp_digit_tc ar30l  = NN_LCHUNKU(par3[0]);

                 mp_digit_t mul2h  = NN_HCHUNKU(par1[0]);
                 mp_digit_t mul2l  = NN_LCHUNKU(par1[0]);
        register mp_digit_t quadh  = mul2h*ar20h;
        register mp_digit_t quadm  = mul2h*ar20l + mul2l*ar20h;
        register mp_digit_t quadl  = mul2l*ar20l;
                 mp_digit_t quadll = NN_LCHUNKU(quadl);
                 mp_digit_t quadlh = NN_LCHUNKU(quadm + NN_HCHUNKU(quadl));
                 mp_digit_t mul3l  = quadll*inv3l;
                 mp_digit_t mul3h  = quadll*inv3h + quadlh*inv3l;
        register mp_digit_t carry;

        mul3h  = NN_LCHUNKU(mul3h + NN_HCHUNKU(mul3l));
        mul3l  = NN_LCHUNKU(mul3l);
        quadh += mul3h*ar30h;
        quadm += mul3h*ar30l + mul3l*ar30h;
        quadl += mul3l*ar30l;
        quadm += NN_HCHUNKU(quadl);
        carry  = quadh + NN_HCHUNKU(quadm);

        if (lng == 1) {
            tmparr[0] = carry;
        } else {
            i = 1;
            do {
                mp_digit_tc ar2h  = NN_HCHUNKU(ar2[i]);
                mp_digit_tc ar2l  = NN_LCHUNKU(ar2[i]);
                mp_digit_tc ar3h  = NN_HCHUNKU(ar3[i]);
                mp_digit_tc ar3l  = NN_LCHUNKU(ar3[i]);

                quadh = mul2h*ar2h + mul3h*ar3h;
                quadl = mul2l*ar2l + mul3l*ar3l;
                quadm = quadh - (mul2h-mul2l)*(ar2h-ar2l)
                              - (mul3h-mul3l)*(ar3h-ar3l) + quadl;

                quadm += NN_HCHUNKU(carry);
                quadl += NN_LCHUNKU(carry);

                quadh += NN_HCHUNKU(quadm);
                quadl += NN_LCHUNKU(quadm) << LG2MPB_CHUNK;
                tmparr[i-1] = quadl & MP_MASK;
                carry = quadh + (quadl >> LG2MPB);
            } while (++i < lng);
            tmparr[lng-1] = carry;

            j = 1;
            do {
                mul2h  = NN_HCHUNKU(par1[j]);
                mul2l  = NN_LCHUNKU(par1[j]);
                quadh  = mul2h*ar20h;
                quadm  = mul2h*ar20l + mul2l*ar20h;
                quadl  = mul2l*ar20l + tmparr[0];
                quadll = NN_LCHUNKU(quadl);
                quadlh = NN_LCHUNKU(quadm + NN_HCHUNKU(quadl));
                mul3l  = quadll*inv3l;
                mul3h  = quadll*inv3h + quadlh*inv3l;
                mul3h  = NN_LCHUNKU(mul3h + NN_HCHUNKU(mul3l));
                mul3l  = NN_LCHUNKU(mul3l);
                quadh += mul3h*ar30h;
                quadm += mul3h*ar30l + mul3l*ar30h;
                quadl += mul3l*ar30l;
                quadm += NN_HCHUNKU(quadl);
                carry  = quadh + NN_HCHUNKU(quadm);

                i = 1;
                do {
                    mp_digit_tc ar2h = NN_HCHUNKU(ar2[i]);
                    mp_digit_tc ar2l = NN_LCHUNKU(ar2[i]);
                    mp_digit_tc ar3h = NN_HCHUNKU(ar3[i]);
                    mp_digit_tc ar3l = NN_LCHUNKU(ar3[i]);

                    quadh = mul2h*ar2h + mul3h*ar3h;
                    quadl = mul2l*ar2l + mul3l*ar3l;
                    quadm = quadh + quadl
                       - (mul2h-mul2l)*(ar2h-ar2l)
                       - (mul3h-mul3l)*(ar3h-ar3l);

                    quadm += NN_HCHUNKU(tmparr[i] + carry);
                    quadl += NN_LCHUNKU(tmparr[i] + carry);

                    quadh += NN_HCHUNKU(quadm);
                    quadl += NN_LCHUNKU(quadm) << LG2MPB_CHUNK;

                    tmparr[i-1] = quadl & MP_MASK;
                    carry = quadh + (quadl >> LG2MPB);
                } while (++i < lng);
                tmparr[lng-1] = carry;
            } while (++j < lng);
        }       /* lng > 1 */
    }
    break;      /* MULT_HALFINT */
#undef NN_HCHUNKU
#undef NN_LCHUNKU

case MULT_DOUBLE_PRECISION:     /* Use double precision floating */

#define DBLTOP(dblarg) ((double)((smp_digit_t)((dblarg)*MP_BASE_DINV)))
                        /* Upper half of double precision floating arg */
#define DBLBOT(dblarg) ((dblarg) - MP_BASE_D*DBLTOP(dblarg))
                        /* Lower half of double precision floating arg */
#define MPDVAL(dblarg) ((smp_digit_t)(dblarg))
                        /* Convert floating to integer */
#define DBLVAL(mpdarg) ((double)((smp_digit_t)(mpdarg)))
                        /* Convert floating to integer */
    {
           const double dinv3 = DBLVAL(inv3);
        register double mul2  = DBLVAL(par1[0]);
        register double quad  = mul2*DBLVAL(par2[0]);
        register double mul3  = DBLBOT(DBLBOT(quad)*dinv3);
        register double carry = (quad + mul3*DBLVAL(par3[0]))*MP_BASE_DINV;

        if (lng == 1) {
            tmparr[0] = MPDVAL(carry);
        } else  {
            i = 1;
            do {
                register double q;

                quad = mul2*DBLVAL(par2[i]) + mul3*DBLVAL(par3[i]) + carry;
                q = DBLTOP(quad);
                tmparr[i-1] = MPDVAL(quad - MP_BASE_D*q);
                carry = q;
            } while (++i < lng);
            tmparr[lng-1] = MPDVAL(carry);

            j = 1;
            do {
                mul2  = DBLVAL(par1[j]);
                quad  = mul2*DBLVAL(par2[0]) + DBLVAL(tmparr[0]);
                mul3  = DBLBOT(DBLBOT(quad)*dinv3);
                carry = (quad + mul3*DBLVAL(par3[0]))*MP_BASE_DINV;

                i = 1;
                do {
                    register double q;

                    quad =    mul2*DBLVAL(par2[i]) + mul3*DBLVAL(par3[i])
                            + DBLVAL(tmparr[i]) + carry;
                    q = DBLTOP(quad);
                    tmparr[i-1] = MPDVAL(quad - MP_BASE_D*q);
                    carry = q;
                } while (++i < lng);
                tmparr[lng-1] = MPDVAL(carry);
            } while (++j < lng);
        }       /* lng > 1 */
    }
#undef DBLTOP
#undef DBLBOT
#undef DBLVAL
#undef MPDVAL
    break;      /* MULT_DOUBLE_PRECISION */
    }   /* switch */

                /* Common code (independent of multiplication algorithm) */

    {
        smp_digit_t diff;

        j = lng - 1;
        do {
            diff = (smp_digit_t)tmparr[j] - (smp_digit_t)par3[j];
            j--;
        } while (diff == 0 && j >= 0);

        if (diff < 0) {
            (void)memcpy(ar4, tmparr, lng*sizeof(mp_digit_t));
        } else {                /* Multiple precision subtract */
            register smp_digit_t c1 = 1;        /* carry + 1 */
            for (j = 0; j < lng; j++) {
                c1 += tmparr[j] - par3[j] + (MP_BASE-1);
                ar4[j] = c1 & MP_MASK;
                c1 >>= LG2MPB;
            }
            assert (c1 == 1);
        }
    }
}

double CP_TIME(void)            /* Return CP time used so far */
{
    return clock() / (double)CLK_TCK;
}

mp_digit_t DIFFER(CONSTP(mp_digit_tc) ar1,
                  CONSTP(mp_digit_tc) ar2,
                  mp_lng_tc lng)
        /* Return 0 if ar1 == ar2, nonzero if ar1 != ar2 */
{
    mp_digit_t or_xors = 0;
    mp_lng_t i;
    for (i = 0; i < lng; i++) {or_xors |= (ar1[i] ^ ar2[i]);}
    return or_xors;
}

mp_digit_t NR_INV(mp_digit_tc lowdig)
        /* Find neg_recip such that lowdig*neg_recip == -1 mod MP_BASE */
        /* Use Newton's method  (all modulo a power of 2) */
{
    mp_digit_t neg_recip = -lowdig;     /* 3 bit approximation */
    mp_lng_t prec = 3;
    while (prec < LG2MPB) {
        neg_recip *= (2 + neg_recip*lowdig);
        prec *= 2;
    }
    neg_recip &= MP_MASK;
    assert(((neg_recip*lowdig + 1) & MP_MASK) == 0);
    return neg_recip;
}

static unsigned long int next = 1;
int my_rand() {
    next = next * 1103515245 + 12345;
    return (unsigned)(next/65536) % 32768;
}

mp_digit_t RAND_DIGIT()
{
    mp_digit_tc n1 = my_rand(); /* At least 15-bits */
    mp_digit_tc n2 = my_rand(); /* At least 15-bits */
    mp_digit_tc n3 = n1 ^ (n2 >> LG2MPB_CHUNK) ^ (n2 << LG2MPB_CHUNK);
    return (n3 & MP_MASK);
}

void MP_PRINT(CONSTP(char       ) name,
              CONSTP(mp_digit_tc) ar1,
                     mp_lng_tc    lng)  /* Print number in internal form */
{
    mp_lng_t i;
    printf("%s =", name);
    for (i = 0; i < lng; i++) {
        printf(" %lx", ar1[i]);
    }
    printf("\n");


}

#define NTRIAL 10000
int main (const int argc, CONSTP(const char) argv[])
{
    mp_lng_t lng;
    for (lng = 1; lng <= LNGMAX; lng++) {
        mp_lng_t i, ialg;
        mp_digit_t modlus[LNGMAX], n1[LNGMAX], n2[LNGMAX];
        mp_digit_t invmod;
        mp_digit_t prods[1+MAX_MULT_ALG][LNGMAX];
        mp_digit_t dis_agree = 0;

        for (i = 0; i < lng; i++) {
            modlus[i] = RAND_DIGIT();
            n1[i] = RAND_DIGIT();
            n2[i] = RAND_DIGIT();
        }
/*
                Ensure modlus is odd, first digit positive,
                with 0 <= n1, n2 < modlus.
*/

        modlus[0] |= 1;
        invmod = NR_INV(modlus[0]);
        if (modlus[lng-1] == 0) modlus[lng-1] = 1;
        n1[lng-1] %= modlus[lng-1];
        n2[lng-1] %= modlus[lng-1];
        MP_PRINT("modulus", modlus, lng);
        MP_PRINT("n1", n1, lng);
        MP_PRINT("n2", n2, lng);
        for (ialg = 0; ialg <= MAX_MULT_ALG; ialg++)  {
            NR_MUL(n1, n2, modlus, prods[ialg], invmod, lng, ialg);
            if (ialg > 0) {
                dis_agree |= DIFFER(prods[0], prods[ialg], lng);
            }
        }
        MP_PRINT("product", prods[0], lng);
        if (dis_agree != 0) {
            printf("Disagreement for lng = %d:\n", lng);
            for (ialg = 0; ialg <= MAX_MULT_ALG; ialg++) {
                MP_PRINT(desc[ialg], prods[ialg], lng);
            }
        } else { /* disagreement */
/*
                Print average time per multiplication,
                by each algorithm.
*/
            mp_lng_t itrial;
            double times[1 + MAX_MULT_ALG];
            for (ialg = 0; ialg <= MAX_MULT_ALG; ialg++) {
                const double tbeg = CP_TIME();
                for (itrial = 0; itrial < NTRIAL; itrial++) {
                   NR_MUL(n1, n2, modlus, prods[ialg], invmod, lng, ialg);
                }
                times[ialg] = CP_TIME() - tbeg;
            }
            printf(" lng = %2d", lng);
#if 0
            for (ialg = 0; ialg <= MAX_MULT_ALG; ialg++) {
                printf(", %s %7.0f",
                        desc[ialg], 1.0e6*times[ialg]/(double)NTRIAL);
            }
#endif
            printf("\n");
        }       /* timing */
    } /* for lng */
    return(0);
}
#endif
