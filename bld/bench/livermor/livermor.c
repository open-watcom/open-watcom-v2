/* Livermore Loops coded in C        Latest File Modification  20 Oct 92,
 *  by Tim Peters, Kendall Square Res. Corp. tim@ksr.com, ksr!tim@uunet.uu.net
 *     SUBROUTINE KERNEL( TK)  replaces the Fortran routine in LFK Test program.
 ************************************************************************
 *                                                                      *
 *            KERNEL     executes 24 samples of "C" computation         *
 *                                                                      *
 *                TK(1) - total cpu time to execute only the 24 kernels.*
 *                TK(2) - total Flops executed by the 24 Kernels        *
 *                                                                      *
 ************************************************************************
 *                                                                      *
 *     L. L. N. L.   " C "   K E R N E L S:   M F L O P S               *
 *                                                                      *
 *     These kernels measure   " C "   numerical computation            *
 *     rates for  a  spectrum  of  cpu-limited computational            *
 *     structures or benchmarks.   Mathematical  through-put            *
 *     is measured  in  units  of millions of floating-point            *
 *     operations executed per second, called Megaflops/sec.            *
 *                                                                      *
 *     Fonzi's Law: There is not now and there never will be a language *
 *                  in which it is the least bit difficult to write     *
 *                  bad programs.                                       *
 *                                                    F.H.MCMAHON  1972 *
 ************************************************************************
 *Originally from  Greg Astfalk, AT&T, P.O.Box 900, Princeton, NJ. 08540*
 *               by way of Frank McMahon (LLNL).                        *
 *                                                                      *
 *                               REFERENCE                              *
 *                                                                      *
 *              F.H.McMahon,   The Livermore Fortran Kernels:           *
 *              A Computer Test Of The Numerical Performance Range,     *
 *              Lawrence Livermore National Laboratory,                 *
 *              Livermore, California, UCRL-53745, December 1986.       *
 *                                                                      *
 *       from:  National Technical Information Service                  *
 *              U.S. Department of Commerce                             *
 *              5285 Port Royal Road                                    *
 *              Springfield, VA.  22161                                 *
 *                                                                      *
 *    Changes made to correct many array subscripting problems,         *
 *      make more readable (added #define's), include the original      *
 *      FORTRAN versions of the runs as comments, and make more         *
 *      portable by Kelly O'Hair (LLNL) and Chuck Rasbold (LLNL).       *
 *                                                                      *
 ************************************************************************
 */

#include <stdio.h>
#include <math.h>

/* Type-specifiers for function declarations */
#ifdef CRAY
#define CALLED_FROM_FORTRAN fortran
#define FORTRAN_FUNCTION    fortran
#else
#define CALLED_FROM_FORTRAN
#define FORTRAN_FUNCTION    extern
#endif

/* These are the names to be used for the functions */
#ifdef CRAY
#define KERNEL kernel
#define TRACE  trace
#define SPACE  space
#define TEST   test
#define TRACK  track
#else
#define KERNEL kernel_
#define TRACE  trace_
#define SPACE  space_
#define TEST   test_
#define TRACK  track_
#endif

/* Type-specifiers for the structs that map to common */
#ifdef CRAY
#define COMMON_BLOCK fortran
#else
#define COMMON_BLOCK extern
#endif

/* Names of structs (or common blocks) */
#ifdef CRAY
#define ALPHA  alpha
#define BETA   beta
#define SPACES spaces
#define SPACER spacer
#define SPACE0 space0
#define SPACEI spacei
#define ISPACE ispace
#define SPACE1 space1
#define SPACE2 space2
#else
#define ALPHA  alpha_
#define BETA   beta_
#define SPACES spaces_
#define SPACER spacer_
#define SPACE0 space0_
#define SPACEI spacei_
#define ISPACE ispace_
#define SPACE1 space1_
#define SPACE2 space2_
#endif

/* Declare the four fortran functions called */
FORTRAN_FUNCTION TRACE();
FORTRAN_FUNCTION SPACE();
FORTRAN_FUNCTION TEST();
FORTRAN_FUNCTION TRACK();

/* Define the structs or COMMON BLOCKS */

COMMON_BLOCK struct {
    long Mk;
    long Ik;
    long Im;
    long Ml;
    long Il;
    long Mruns;
    long Nruns;
    long Jr;
    long Npfs[47][3][8];
} ALPHA ;
#define mk     ALPHA.Mk
#define ik     ALPHA.Ik
#define im     ALPHA.Im
#define ml     ALPHA.Ml
#define il     ALPHA.Il
#define mruns  ALPHA.Mruns;
#define nruns  ALPHA.Nruns;
#define jr     ALPHA.Jr
#define npfs   ALPHA.Npfs

COMMON_BLOCK struct {
    double Tic;
    double Times[47][3][8];
    double See[3][8][3][5];
    double Terrs[47][3][8];
    double Csums[47][3][8];
    double Fopn[47][3][8];
    double Dos[47][3][8];
} BETA ;
#define tic     BETA.Tic
#define times   BETA.Times
#define see     BETA.See
#define terrs   BETA.Terrs
#define csums   BETA.Csums
#define fopn    BETA.Fopn
#define dos     BETA.Dos

COMMON_BLOCK struct {
    long Ion;
    long J5;
    long K2;
    long K3;
    long MULTI;
    long Laps;
    long Loop;
    long M;
    long Kr;
    long It;
    long N13h;
    long Ibuf;
    long Npass;
    long Nfail;
    long N;
    long N1;
    long N2;
    long N13;
    long N213;
    long N813;
    long N14;
    long N16;
    long N416;
    long N21;
    long Nt1;
    long Nt2;
} SPACES ;
#define  ion    SPACES.Ion
#define  j5     SPACES.J5
#define  k2     SPACES.K2
#define  k3     SPACES.K3
#define  multi  SPACES.MULTI
#define  laps   SPACES.Laps
#define  loop   SPACES.Loop
#define  m      SPACES.M
#define  kr     SPACES.Kr
#define  it     SPACES.It
#define  n13h   SPACES.N13h
#define  ibuf   SPACES.Ibuf
#define  npass  SPACES.Npass
#define  nfail  SPACES.Nfail
#define  n      SPACES.N
#define  n1     SPACES.N1
#define  n2     SPACES.N2
#define  n13    SPACES.N13
#define  n213   SPACES.N213
#define  n813   SPACES.N813
#define  n14    SPACES.N14
#define  n16    SPACES.N16
#define  n416   SPACES.N416
#define  n21    SPACES.N21
#define  nt1    SPACES.Nt1
#define  nt2    SPACES.Nt2

COMMON_BLOCK struct {
    double A11;
    double A12;
    double A13;
    double A21;
    double A22;
    double A23;
    double A31;
    double A32;
    double A33;
    double Ar;
    double Br;
    double C0;
    double Cr;
    double Di;
    double Dk;
    double Dm22;
    double Dm23;
    double Dm24;
    double Dm25;
    double Dm26;
    double Dm27;
    double Dm28;
    double Dn;
    double E3;
    double E6;
    double Expmax;
    double Flx;
    double Q;
    double Qa;
    double R;
    double Ri;
    double S;
    double Scale;
    double Sig;
    double Stb5;
    double T;
    double Xnc;
    double Xnei;
    double Xnm;
} SPACER ;
#define  a11     SPACER.A11
#define  a12     SPACER.A12
#define  a13     SPACER.A13
#define  a21     SPACER.A21
#define  a22     SPACER.A22
#define  a23     SPACER.A23
#define  a31     SPACER.A31
#define  a32     SPACER.A32
#define  a33     SPACER.A33
#define  ar      SPACER.Ar
#define  br      SPACER.Br
#define  c0      SPACER.C0
#define  cr      SPACER.Cr
#define  di      SPACER.Di
#define  dk      SPACER.Dk
#define  dm22    SPACER.Dm22
#define  dm23    SPACER.Dm23
#define  dm24    SPACER.Dm24
#define  dm25    SPACER.Dm25
#define  dm26    SPACER.Dm26
#define  dm27    SPACER.Dm27
#define  dm28    SPACER.Dm28
#define  dn      SPACER.Dn
#define  e3      SPACER.E3
#define  e6      SPACER.E6
#define  expmax  SPACER.Expmax
#define  flx     SPACER.Flx
#define  q       SPACER.Q
#define  qa      SPACER.Qa
#define  r       SPACER.R
#define  ri      SPACER.Ri
#define  s       SPACER.S
#define  scale   SPACER.Scale
#define  sig     SPACER.Sig
#define  stb5    SPACER.Stb5
#define  t       SPACER.T
#define  xnc     SPACER.Xnc
#define  xnei    SPACER.Xnei
#define  xnm     SPACER.Xnm

COMMON_BLOCK struct {
    double Time[47];
    double Csum[47];
    double Ww[47];
    double Wt[47];
    double Ticks;
    double Fr[9];
    double Terr1[47];
    double Sumw[7];
    double Start;
    double Skale[47];
    double Bias[47];
    double Ws[95];
    double Total[47];
    double Flopn[47];
    long Iq[7];
    long Npf;
    long Npfs1[47];
} SPACE0 ;
#define  time    SPACE0.Time
#define  csum    SPACE0.Csum
#define  ww      SPACE0.Ww
#define  wt      SPACE0.Wt
#define  ticks   SPACE0.Ticks
#define  fr      SPACE0.Fr
#define  terr1   SPACE0.Terr1
#define  sumw    SPACE0.Sumw
#define  start   SPACE0.Start
#define  skale   SPACE0.Skale
#define  bias    SPACE0.Bias
#define  ws      SPACE0.Ws
#define  total   SPACE0.Total
#define  flopn   SPACE0.Flopn
#define  iq      SPACE0.Iq
#define  npf     SPACE0.Npf
#define  npfs1   SPACE0.Npfs1

COMMON_BLOCK struct {
    double Wtp[3];
    long Mult[3];
    long Ispan[3][47];
    long Ipass[3][47];
} SPACEI ;
#define wtp    SPACEI.Wtp
#define mult   SPACEI.Mult
#define ispan  SPACEI.Ispan
#define ipass  SPACEI.Ipass

COMMON_BLOCK struct {
    long E[96];
    long F[96];
    long Ix[1001];
    long Ir[1001];
    long Zone[300];
} ISPACE ;
#define e    ISPACE.E
#define f    ISPACE.F
#define ix   ISPACE.Ix
#define ir   ISPACE.Ir
#define zone ISPACE.Zone

COMMON_BLOCK struct {
    double U[1001];
    double V[1001];
    double W[1001];
    double X[1001];
    double Y[1001];
    double Z[1001];
    double G[1001];
    double Du1[101];
    double Du2[101];
    double Du3[101];
    double Grd[1001];
    double Dex[1001];
    double Xi[1001];
    double Ex[1001];
    double Ex1[1001];
    double Dex1[1001];
    double Vx[1001];
    double Xx[1001];
    double Rx[1001];
    double Rh[2048];
    double Vsp[101];
    double Vstp[101];
    double Vxne[101];
    double Vxnd[101];
    double Ve3[101];
    double Vlr[101];
    double Vlin[101];
    double B5[101];
    double Plan[300];
    double D[300];
    double Sa[101];
    double Sb[101];
} SPACE1 ;
#define  u    SPACE1.U
#define  v    SPACE1.V
#define  w    SPACE1.W
#define  x    SPACE1.X
#define  y    SPACE1.Y
#define  z    SPACE1.Z
#define  g    SPACE1.G
#define  du1  SPACE1.Du1
#define  du2  SPACE1.Du2
#define  du3  SPACE1.Du3
#define  grd  SPACE1.Grd
#define  dex  SPACE1.Dex
#define  xi   SPACE1.Xi
#define  ex   SPACE1.Ex
#define  ex1  SPACE1.Ex1
#define  dex1 SPACE1.Dex1
#define  vx   SPACE1.Vx
#define  xx   SPACE1.Xx
#define  rx   SPACE1.Rx
#define  rh   SPACE1.Rh
#define  vsp  SPACE1.Vsp
#define  vstp SPACE1.Vstp
#define  vxne SPACE1.Vxne
#define  vxnd SPACE1.Vxnd
#define  ve3  SPACE1.Ve3
#define  vlr  SPACE1.Vlr
#define  vlin SPACE1.Vlin
#define  b5   SPACE1.B5
#define  plan SPACE1.Plan
#define  d    SPACE1.D
#define  sa   SPACE1.Sa
#define  sb   SPACE1.Sb

COMMON_BLOCK struct {
    double P[512][4];
    double Px[101][25];
    double Cx[101][25];
    double Vy[25][101];
    double Vh[7][101];
    double Vf[7][101];
    double Vg[7][101];
    double Vs[7][101];
    double Za[7][101];
    double Zp[7][101];
    double Zq[7][101];
    double Zr[7][101];
    double Zm[7][101];
    double Zb[7][101];
    double Zu[7][101];
    double Zv[7][101];
    double Zz[7][101];
    double B[64][64];
    double C[64][64];
    double H[64][64];
    double U1[2][101][5];
    double U2[2][101][5];
    double U3[2][101][5];
} SPACE2 ;
#define  p       SPACE2.P
#define  px      SPACE2.Px
#define  cx      SPACE2.Cx
#define  vy      SPACE2.Vy
#define  vh      SPACE2.Vh
#define  vf      SPACE2.Vf
#define  vg      SPACE2.Vg
#define  vs      SPACE2.Vs
#define  za      SPACE2.Za
#define  zp      SPACE2.Zp
#define  zq      SPACE2.Zq
#define  zr      SPACE2.Zr
#define  zm      SPACE2.Zm
#define  zb      SPACE2.Zb
#define  zu      SPACE2.Zu
#define  zv      SPACE2.Zv
#define  zz      SPACE2.Zz
#define  b       SPACE2.B
#define  c       SPACE2.C
#define  h       SPACE2.H
#define  u1      SPACE2.U1
#define  u2      SPACE2.U2
#define  u3      SPACE2.U3

/* KERNEL routine */

CALLED_FROM_FORTRAN KERNEL( TK )
double TK[6];
{

#pragma nodyneqv
#pragma o=i

    long argument , k , l , ipnt , ipntp , i;
    long lw , j , nl1 , nl2 , kx , ky , ip , kn;
    long i1 , j1 , i2 , j2 , nz , ink , jn , kb5i;
    long ii , lb , j4 , ng;
    double tmp , temp, sum, som;
    char name[8];

    /*
     *   Prologue
     */

    for(k=0; k<7; k++) name[k] = "kernel"[k];
    TRACE( &name );

    SPACE();
    argument = 0;
    TEST( &argument );

    /*
     *******************************************************************
     *   Kernel 1 -- hydro fragment
     *******************************************************************
     *       DO 1 L = 1,Loop
     *       DO 1 k = 1,n
     *  1       X(k)= Q + Y(k)*(R*ZX(k+10) + T*ZX(k+11))
     */

    for ( l=1 ; l<=loop ; l++ ) {
        for ( k=0 ; k<n ; k++ ) {
            x[k] = q + y[k]*( r*z[k+10] + t*z[k+11] );
        }
    }
    argument = 1;
    TEST( &argument );

    /*
     *******************************************************************
     *   Kernel 2 -- ICCG excerpt (Incomplete Cholesky Conjugate Gradient)
     *******************************************************************
     *    DO 200  L= 1,Loop
     *        II= n
     *     IPNTP= 0
     *222   IPNT= IPNTP
     *     IPNTP= IPNTP+II
     *        II= II/2
     *         i= IPNTP
     CDIR$ IVDEP
     *    DO 2 k= IPNT+2,IPNTP,2
     *         i= i+1
     *  2   X(i)= X(k) - V(k)*X(k-1) - V(k+1)*X(k+1)
     *        IF( II.GT.1) GO TO 222
     *200 CONTINUE
     */

    for ( l=1 ; l<=loop ; l++ ) {
        ii = n;
        ipntp = 0;
        do {
            ipnt = ipntp;
            ipntp += ii;
            ii /= 2;
            i = ipntp - 1;
#pragma nohazard
            for ( k=ipnt+1 ; k<ipntp ; k=k+2 ) {
                i++;
                x[i] = x[k] - v[k  ]*x[k-1] - v[k+1]*x[k+1];
            }
        } while ( ii>0 );
    }
    argument = 2;
    TEST( &argument );

    /*
     *******************************************************************
     *   Kernel 3 -- inner product
     *******************************************************************
     *    DO 3 L= 1,Loop
     *         Q= 0.0
     *    DO 3 k= 1,n
     *  3      Q= Q + Z(k)*X(k)
     */

    for ( l=1 ; l<=loop ; l++ ) {
        q = 0.0;
        for ( k=0 ; k<n ; k++ ) {
            q += z[k]*x[k];
        }
    }
    argument = 3;
    TEST( &argument );

    /*
     *******************************************************************
     *   Kernel 4 -- banded linear equations
     *******************************************************************
     *            m= (1001-7)/2
     *    DO 444  L= 1,Loop
     *    DO 444  k= 7,1001,m
     *           lw= k-6
     *         temp= X(k-1)
     CDIR$ IVDEP
     *    DO   4  j= 5,n,5
     *       temp  = temp   - XZ(lw)*Y(j)
     *  4        lw= lw+1
     *       X(k-1)= Y(5)*temp
     *444 CONTINUE
     */

    m = ( 1001-7 )/2;
    for ( l=1 ; l<=loop ; l++ ) {
        for ( k=6 ; k<1001 ; k=k+m ) {
            lw = k - 6;
            temp = x[k-1];
#pragma nohazard
            for ( j=4 ; j<n ; j=j+5 ) {
                temp -= x[lw]*y[j];
                lw++;
            }
            x[k-1] = y[4]*temp;
        }
    }
    argument = 4;
    TEST( &argument );

    /*
     *******************************************************************
     *   Kernel 5 -- tri-diagonal elimination, below diagonal
     *******************************************************************
     *    DO 5 L = 1,Loop
     *    DO 5 i = 2,n
     *  5    X(i)= Z(i)*(Y(i) - X(i-1))
     */

    for ( l=1 ; l<=loop ; l++ ) {
        for ( i=1 ; i<n ; i++ ) {
            x[i] = z[i]*( y[i] - x[i-1] );
        }
    }
    argument = 5;
    TEST( &argument );

    /*
     *******************************************************************
     *   Kernel 6 -- general linear recurrence equations
     *******************************************************************
     *    DO  6  L= 1,Loop
     *    DO  6  i= 2,n
     *    DO  6  k= 1,i-1
     *        W(i)= W(i)  + B(i,k) * W(i-k)
     *  6 CONTINUE
     */

    for ( l=1 ; l<=loop ; l++ ) {
        for ( i=1 ; i<n ; i++ ) {
            for ( k=0 ; k<i ; k++ ) {
                w[i] += b[k][i] * w[(i-k)-1];
            }
        }
    }
    argument = 6;
    TEST( &argument );

    /*
     *******************************************************************
     *   Kernel 7 -- equation of state fragment
     *******************************************************************
     *    DO 7 L= 1,Loop
     *    DO 7 k= 1,n
     *      X(k)=     U(k  ) + R*( Z(k  ) + R*Y(k  )) +
     *   .        T*( U(k+3) + R*( U(k+2) + R*U(k+1)) +
     *   .        T*( U(k+6) + R*( U(k+5) + R*U(k+4))))
     *  7 CONTINUE
     */

    for ( l=1 ; l<=loop ; l++ ) {
#pragma nohazard
        for ( k=0 ; k<n ; k++ ) {
            x[k] = u[k] + r*( z[k] + r*y[k] ) +
                   t*( u[k+3] + r*( u[k+2] + r*u[k+1] ) +
                      t*( u[k+6] + r*( u[k+5] + r*u[k+4] ) ) );
        }
    }
    argument = 7;
    TEST( &argument );

    /*
     *******************************************************************
     *   Kernel 8 -- ADI integration
     *******************************************************************
     *    DO  8      L = 1,Loop
     *             nl1 = 1
     *             nl2 = 2
     *    DO  8     kx = 2,3
     CDIR$ IVDEP
     *    DO  8     ky = 2,n
     *          DU1(ky)=U1(kx,ky+1,nl1)  -  U1(kx,ky-1,nl1)
     *          DU2(ky)=U2(kx,ky+1,nl1)  -  U2(kx,ky-1,nl1)
     *          DU3(ky)=U3(kx,ky+1,nl1)  -  U3(kx,ky-1,nl1)
     *    U1(kx,ky,nl2)=U1(kx,ky,nl1) +A11*DU1(ky) +A12*DU2(ky) +A13*DU3(ky)
     *   .       + SIG*(U1(kx+1,ky,nl1) -2.*U1(kx,ky,nl1) +U1(kx-1,ky,nl1))
     *    U2(kx,ky,nl2)=U2(kx,ky,nl1) +A21*DU1(ky) +A22*DU2(ky) +A23*DU3(ky)
     *   .       + SIG*(U2(kx+1,ky,nl1) -2.*U2(kx,ky,nl1) +U2(kx-1,ky,nl1))
     *    U3(kx,ky,nl2)=U3(kx,ky,nl1) +A31*DU1(ky) +A32*DU2(ky) +A33*DU3(ky)
     *   .       + SIG*(U3(kx+1,ky,nl1) -2.*U3(kx,ky,nl1) +U3(kx-1,ky,nl1))
     *  8 CONTINUE
     */

    for ( l=1 ; l<=loop ; l++ ) {
        nl1 = 0;
        nl2 = 1;
        for ( kx=1 ; kx<3 ; kx++ ){
#pragma nohazard
            for ( ky=1 ; ky<n ; ky++ ) {
               du1[ky] = u1[nl1][ky+1][kx] - u1[nl1][ky-1][kx];
               du2[ky] = u2[nl1][ky+1][kx] - u2[nl1][ky-1][kx];
               du3[ky] = u3[nl1][ky+1][kx] - u3[nl1][ky-1][kx];
               u1[nl2][ky][kx]=
                  u1[nl1][ky][kx]+a11*du1[ky]+a12*du2[ky]+a13*du3[ky] + sig*
                     (u1[nl1][ky][kx+1]-2.0*u1[nl1][ky][kx]+u1[nl1][ky][kx-1]);
               u2[nl2][ky][kx]=
                  u2[nl1][ky][kx]+a21*du1[ky]+a22*du2[ky]+a23*du3[ky] + sig*
                     (u2[nl1][ky][kx+1]-2.0*u2[nl1][ky][kx]+u2[nl1][ky][kx-1]);
               u3[nl2][ky][kx]=
                  u3[nl1][ky][kx]+a31*du1[ky]+a32*du2[ky]+a33*du3[ky] + sig*
                     (u3[nl1][ky][kx+1]-2.0*u3[nl1][ky][kx]+u3[nl1][ky][kx-1]);
            }
        }
    }
    argument = 8;
    TEST( &argument );

    /*
     *******************************************************************
     *   Kernel 9 -- integrate predictors
     *******************************************************************
     *    DO 9  L = 1,Loop
     *    DO 9  i = 1,n
     *    PX( 1,i)= DM28*PX(13,i) + DM27*PX(12,i) + DM26*PX(11,i) +
     *   .          DM25*PX(10,i) + DM24*PX( 9,i) + DM23*PX( 8,i) +
     *   .          DM22*PX( 7,i) +  C0*(PX( 5,i) +      PX( 6,i))+ PX( 3,i)
     *  9 CONTINUE
     */

    for ( l=1 ; l<=loop ; l++ ) {
        for ( i=0 ; i<n ; i++ ) {
            px[i][0] = dm28*px[i][12] + dm27*px[i][11] + dm26*px[i][10] +
                       dm25*px[i][ 9] + dm24*px[i][ 8] + dm23*px[i][ 7] +
                       dm22*px[i][ 6] + c0*( px[i][ 4] + px[i][ 5]) + px[i][ 2];
        }
    }
    argument = 9;
    TEST( &argument );

    /*
     *******************************************************************
     *   Kernel 10 -- difference predictors
     *******************************************************************
     *    DO 10  L= 1,Loop
     *    DO 10  i= 1,n
     *    AR      =      CX(5,i)
     *    BR      = AR - PX(5,i)
     *    PX(5,i) = AR
     *    CR      = BR - PX(6,i)
     *    PX(6,i) = BR
     *    AR      = CR - PX(7,i)
     *    PX(7,i) = CR
     *    BR      = AR - PX(8,i)
     *    PX(8,i) = AR
     *    CR      = BR - PX(9,i)
     *    PX(9,i) = BR
     *    AR      = CR - PX(10,i)
     *    PX(10,i)= CR
     *    BR      = AR - PX(11,i)
     *    PX(11,i)= AR
     *    CR      = BR - PX(12,i)
     *    PX(12,i)= BR
     *    PX(14,i)= CR - PX(13,i)
     *    PX(13,i)= CR
     * 10 CONTINUE
     */

    for ( l=1 ; l<=loop ; l++ ) {
        for ( i=0 ; i<n ; i++ ) {
            ar        =      cx[i][ 4];
            br        = ar - px[i][ 4];
            px[i][ 4] = ar;
            cr        = br - px[i][ 5];
            px[i][ 5] = br;
            ar        = cr - px[i][ 6];
            px[i][ 6] = cr;
            br        = ar - px[i][ 7];
            px[i][ 7] = ar;
            cr        = br - px[i][ 8];
            px[i][ 8] = br;
            ar        = cr - px[i][ 9];
            px[i][ 9] = cr;
            br        = ar - px[i][10];
            px[i][10] = ar;
            cr        = br - px[i][11];
            px[i][11] = br;
            px[i][13] = cr - px[i][12];
            px[i][12] = cr;
        }
    }
    argument = 10;
    TEST( &argument );

    /*
     *******************************************************************
     *   Kernel 11 -- first sum
     *******************************************************************
     *    DO 11 L = 1,Loop
     *        X(1)= Y(1)
     *    DO 11 k = 2,n
     * 11     X(k)= X(k-1) + Y(k)
     */

    for ( l=1 ; l<=loop ; l++ ) {
        x[0] = y[0];
        for ( k=1 ; k<n ; k++ ) {
            x[k] = x[k-1] + y[k];
        }
    }
    argument = 11;
    TEST( &argument );

    /*
     *******************************************************************
     *   Kernel 12 -- first difference
     *******************************************************************
     *    DO 12 L = 1,Loop
     *    DO 12 k = 1,n
     * 12     X(k)= Y(k+1) - Y(k)
     */

    for ( l=1 ; l<=loop ; l++ ) {
        for ( k=0 ; k<n ; k++ ) {
            x[k] = y[k+1] - y[k];
        }
    }
    argument = 12;
    TEST( &argument );

    /*
     *******************************************************************
     *   Kernel 13 -- 2-D PIC (Particle In Cell)
     *******************************************************************
     *    DO  13     L= 1,Loop
     *    DO  13    ip= 1,n
     *              i1= P(1,ip)
     *              j1= P(2,ip)
     *              i1=        1 + MOD2N(i1,64)
     *              j1=        1 + MOD2N(j1,64)
     *         P(3,ip)= P(3,ip)  + B(i1,j1)
     *         P(4,ip)= P(4,ip)  + C(i1,j1)
     *         P(1,ip)= P(1,ip)  + P(3,ip)
     *         P(2,ip)= P(2,ip)  + P(4,ip)
     *              i2= P(1,ip)
     *              j2= P(2,ip)
     *              i2=            MOD2N(i2,64)
     *              j2=            MOD2N(j2,64)
     *         P(1,ip)= P(1,ip)  + Y(i2+32)
     *         P(2,ip)= P(2,ip)  + Z(j2+32)
     *              i2= i2       + E(i2+32)
     *              j2= j2       + F(j2+32)
     *        H(i2,j2)= H(i2,j2) + 1.0
     * 13 CONTINUE
     */

    for ( l=1 ; l<=loop ; l++ ) {
        for ( ip=0 ; ip<n ; ip++ ) {
            i1 = p[ip][0];
            j1 = p[ip][1];
            i1 &= 64-1;
            j1 &= 64-1;
            p[ip][2] += b[j1][i1];
            p[ip][3] += c[j1][i1];
            p[ip][0] += p[ip][2];
            p[ip][1] += p[ip][3];
            i2 = p[ip][0];
            j2 = p[ip][1];
            i2 = ( i2 & 64-1 ) - 1 ;
            j2 = ( j2 & 64-1 ) - 1 ;
            p[ip][0] += y[i2+32];
            p[ip][1] += z[j2+32];
            i2 += e[i2+32];
            j2 += f[j2+32];
            h[j2][i2] += 1.0;
        }
    }
    argument = 13;
    TEST( &argument );

    /*
     *******************************************************************
     *   Kernel 14 -- 1-D PIC (Particle In Cell)
     *******************************************************************
     *    DO   14   L= 1,Loop
     *    DO   141  k= 1,n
     *          VX(k)= 0.0
     *          XX(k)= 0.0
     *          IX(k)= INT(  GRD(k))
     *          XI(k)= REAL( IX(k))
     *         EX1(k)= EX   ( IX(k))
     *        DEX1(k)= DEX  ( IX(k))
     *41  CONTINUE
     *    DO   142  k= 1,n
     *          VX(k)= VX(k) + EX1(k) + (XX(k) - XI(k))*DEX1(k)
     *          XX(k)= XX(k) + VX(k)  + FLX
     *          IR(k)= XX(k)
     *          RX(k)= XX(k) - IR(k)
     *          IR(k)= MOD2N(  IR(k),2048) + 1
     *          XX(k)= RX(k) + IR(k)
     *42  CONTINUE
     *    DO  14    k= 1,n
     *    RH(IR(k)  )= RH(IR(k)  ) + 1.0 - RX(k)
     *    RH(IR(k)+1)= RH(IR(k)+1) + RX(k)
     *14  CONTINUE
     */

    for ( l=1 ; l<=loop ; l++ ) {
        for ( k=0 ; k<n ; k++ ) {
            vx[k] = 0.0;
            xx[k] = 0.0;
            ix[k] = (long) grd[k];
            xi[k] = (double) ix[k];
            ex1[k] = ex[ ix[k] - 1 ];
            dex1[k] = dex[ ix[k] - 1 ];
        }
        for ( k=0 ; k<n ; k++ ) {
            vx[k] = vx[k] + ex1[k] + ( xx[k] - xi[k] )*dex1[k];
            xx[k] = xx[k] + vx[k]  + flx;
            ir[k] = xx[k];
            rx[k] = xx[k] - ir[k];
            ir[k] = ( ir[k] & 2048-1 ) + 1;
            xx[k] = rx[k] + ir[k];
        }
        for ( k=0 ; k<n ; k++ ) {
            rh[ ir[k]-1 ] += 1.0 - rx[k];
            rh[ ir[k]   ] += rx[k];
        }
    }
    argument = 14;
    TEST( &argument );

    /*
     *******************************************************************
     *   Kernel 15 -- Casual Fortran.  Development version
     *******************************************************************
     *      DO 45  L = 1,Loop
     *             NG= 7
     *             NZ= n
     *             AR= 0.053
     *             BR= 0.073
     * 15   DO 45  j = 2,NG
     *      DO 45  k = 2,NZ
     *             IF( j-NG) 31,30,30
     * 30     VY(k,j)= 0.0
     *                 GO TO 45
     * 31          IF( VH(k,j+1) -VH(k,j)) 33,33,32
     * 32           T= AR
     *                 GO TO 34
     * 33           T= BR
     * 34          IF( VF(k,j) -VF(k-1,j)) 35,36,36
     * 35           R= MAX( VH(k-1,j), VH(k-1,j+1))
     *              S= VF(k-1,j)
     *                 GO TO 37
     * 36           R= MAX( VH(k,j),   VH(k,j+1))
     *              S= VF(k,j)
     * 37     VY(k,j)= SQRT( VG(k,j)**2 +R*R)*T/S
     * 38          IF( k-NZ) 40,39,39
     * 39     VS(k,j)= 0.
     *                 GO TO 45
     * 40          IF( VF(k,j) -VF(k,j-1)) 41,42,42
     * 41           R= MAX( VG(k,j-1), VG(k+1,j-1))
     *              S= VF(k,j-1)
     *              T= BR
     *                 GO TO 43
     * 42           R= MAX( VG(k,j),   VG(k+1,j))
     *              S= VF(k,j)
     *              T= AR
     * 43     VS(k,j)= SQRT( VH(k,j)**2 +R*R)*T/S
     * 45    CONTINUE
     */

#pragma intrinsic sqrt

    for ( l=1 ; l<=loop ; l++ ) {
        ng = 7;
        nz = n;
        ar = 0.053;
        br = 0.073;
        for ( j=1 ; j<ng ; j++ ) {
            for ( k=1 ; k<nz ; k++ ) {
                if ( (j+1) >= ng ) {
                    vy[j][k] = 0.0;
                    continue;
                }
                if ( vh[j+1][k] > vh[j][k] ) {
                    t = ar;
                }
                else {
                    t = br;
                }
                if ( vf[j][k] < vf[j][k-1] ) {
                    if ( vh[j][k-1] > vh[j+1][k-1] )
                        r = vh[j][k-1];
                    else
                        r = vh[j+1][k-1];
                    s = vf[j][k-1];
                }
                else {
                    if ( vh[j][k] > vh[j+1][k] )
                        r = vh[j][k];
                    else
                        r = vh[j+1][k];
                    s = vf[j][k];
                }
                vy[j][k] = sqrt( vg[j][k]*vg[j][k] + r*r )* t/s;
                if ( (k+1) >= nz ) {
                    vs[j][k] = 0.0;
                    continue;
                }
                if ( vf[j][k] < vf[j-1][k] ) {
                    if ( vg[j-1][k] > vg[j-1][k+1] )
                        r = vg[j-1][k];
                    else
                        r = vg[j-1][k+1];
                    s = vf[j-1][k];
                    t = br;
                }
                else {
                    if ( vg[j][k] > vg[j][k+1] )
                        r = vg[j][k];
                    else
                        r = vg[j][k+1];
                    s = vf[j][k];
                    t = ar;
                }
                vs[j][k] = sqrt( vh[j][k]*vh[j][k] + r*r )* t / s;
            }
        }
    }
    argument = 15;
    TEST( &argument );

    /*
     *******************************************************************
     *   Kernel 16 -- Monte Carlo search loop
     *******************************************************************
     *          II= n/3
     *          LB= II+II
     *          k2= 0
     *          k3= 0
     *    DO 485 L= 1,Loop
     *           m= 1
     *405       i1= m
     *410       j2= (n+n)*(m-1)+1
     *    DO 470 k= 1,n
     *          k2= k2+1
     *          j4= j2+k+k
     *          j5= ZONE(j4)
     *          IF( j5-n      ) 420,475,450
     *415       IF( j5-n+II   ) 430,425,425
     *420       IF( j5-n+LB   ) 435,415,415
     *425       IF( PLAN(j5)-R) 445,480,440
     *430       IF( PLAN(j5)-S) 445,480,440
     *435       IF( PLAN(j5)-T) 445,480,440
     *440       IF( ZONE(j4-1)) 455,485,470
     *445       IF( ZONE(j4-1)) 470,485,455
     *450       k3= k3+1
     *          IF( D(j5)-(D(j5-1)*(T-D(j5-2))**2+(S-D(j5-3))**2
     *   .                        +(R-D(j5-4))**2)) 445,480,440
     *455        m= m+1
     *          IF( m-ZONE(1) ) 465,465,460
     *460        m= 1
     *465       IF( i1-m) 410,480,410
     *470 CONTINUE
     *475 CONTINUE
     *480 CONTINUE
     *485 CONTINUE
     */

    ii = n / 3;
    lb = ii + ii;
    k3 = k2 = 0;
    for ( l=1 ; l<=loop ; l++ ) {
        i1 = m = 1;
        label410:
        j2 = ( n + n )*( m - 1 ) + 1;
        for ( k=1 ; k<=n ; k++ ) {
            k2++;
            j4 = j2 + k + k;
            j5 = zone[j4-1];
            if ( j5 < n ) {
                if ( j5+lb < n ) {              /* 420 */
                    tmp = plan[j5-1] - t;       /* 435 */
                } else {
                    if ( j5+ii < n ) {          /* 415 */
                        tmp = plan[j5-1] - s;   /* 430 */
                    } else {
                        tmp = plan[j5-1] - r;   /* 425 */
                    }
                }
            } else if( j5 == n ) {
                break;                          /* 475 */
            } else {
                k3++;                           /* 450 */
                tmp=(d[j5-1]-(d[j5-2]*(t-d[j5-3])*(t-d[j5-3])+(s-d[j5-4])*
                              (s-d[j5-4])+(r-d[j5-5])*(r-d[j5-5])));
            }
            if ( tmp < 0.0 ) {
                if ( zone[j4-2] < 0 )           /* 445 */
                    continue;                   /* 470 */
                else if ( !zone[j4-2] )
                    break;                      /* 480 */
            } else if ( tmp ) {
                if ( zone[j4-2] > 0 )           /* 440 */
                    continue;                   /* 470 */
                else if ( !zone[j4-2] )
                    break;                      /* 480 */
            } else break;                       /* 485 */
            m++;                                /* 455 */
            if ( m > zone[0] )
                m = 1;                          /* 460 */
            if ( i1-m )                         /* 465 */
                goto label410;
            else
                break;
        }
    }
    argument = 16;
    TEST( &argument );

    /*
     *******************************************************************
     *   Kernel 17 -- implicit, conditional computation
     *******************************************************************
     *          DO 62 L= 1,Loop
     *                i= n
     *                j= 1
     *              INK= -1
     *            SCALE= 5./3.
     *              XNM= 1./3.
     *               E6= 1.03/3.07
     *                   GO TO 61
     *60             E6= XNM*VSP(i)+VSTP(i)
     *          VXNE(i)= E6
     *              XNM= E6
     *           VE3(i)= E6
     *                i= i+INK
     *               IF( i.EQ.j) GO TO  62
     *61             E3= XNM*VLR(i) +VLIN(i)
     *             XNEI= VXNE(i)
     *          VXND(i)= E6
     *              XNC= SCALE*E3
     *               IF( XNM .GT.XNC) GO TO  60
     *               IF( XNEI.GT.XNC) GO TO  60
     *           VE3(i)= E3
     *               E6= E3+E3-XNM
     *          VXNE(i)= E3+E3-XNEI
     *              XNM= E6
     *                i= i+INK
     *               IF( i.NE.j) GO TO 61
     * 62 CONTINUE
     */

    for ( l=1 ; l<=loop ; l++ ) {
        i = n-1;
        j = 0;
        ink = -1;
        scale = 5.0 / 3.0;
        xnm = 1.0 / 3.0;
        e6 = 1.03 / 3.07;
        goto l61;
l60:    e6 = xnm*vsp[i] + vstp[i];
        vxne[i] = e6;
        xnm = e6;
        ve3[i] = e6;
        i += ink;
        if ( i==j ) goto l62;
l61:    e3 = xnm*vlr[i] + vlin[i];
        xnei = vxne[i];
        vxnd[i] = e6;
        xnc = scale*e3;
        if ( xnm > xnc ) goto l60;
        if ( xnei > xnc ) goto l60;
        ve3[i] = e3;
        e6 = e3 + e3 - xnm;
        vxne[i] = e3 + e3 - xnei;
        xnm = e6;
        i += ink;
        if ( i != j ) goto l61;
l62:;
    }

    argument = 17;
    TEST( &argument );

    /*
     *******************************************************************
     *   Kernel 18 - 2-D explicit hydrodynamics fragment
     *******************************************************************
     *       DO 75  L= 1,Loop
     *              T= 0.0037
     *              S= 0.0041
     *             KN= 6
     *             JN= n
     *       DO 70  k= 2,KN
     *       DO 70  j= 2,JN
     *        ZA(j,k)= (ZP(j-1,k+1)+ZQ(j-1,k+1)-ZP(j-1,k)-ZQ(j-1,k))
     *   .            *(ZR(j,k)+ZR(j-1,k))/(ZM(j-1,k)+ZM(j-1,k+1))
     *        ZB(j,k)= (ZP(j-1,k)+ZQ(j-1,k)-ZP(j,k)-ZQ(j,k))
     *   .            *(ZR(j,k)+ZR(j,k-1))/(ZM(j,k)+ZM(j-1,k))
     * 70    CONTINUE
     *       DO 72  k= 2,KN
     *       DO 72  j= 2,JN
     *        ZU(j,k)= ZU(j,k)+S*(ZA(j,k)*(ZZ(j,k)-ZZ(j+1,k))
     *   .                    -ZA(j-1,k) *(ZZ(j,k)-ZZ(j-1,k))
     *   .                    -ZB(j,k)   *(ZZ(j,k)-ZZ(j,k-1))
     *   .                    +ZB(j,k+1) *(ZZ(j,k)-ZZ(j,k+1)))
     *        ZV(j,k)= ZV(j,k)+S*(ZA(j,k)*(ZR(j,k)-ZR(j+1,k))
     *   .                    -ZA(j-1,k) *(ZR(j,k)-ZR(j-1,k))
     *   .                    -ZB(j,k)   *(ZR(j,k)-ZR(j,k-1))
     *   .                    +ZB(j,k+1) *(ZR(j,k)-ZR(j,k+1)))
     * 72    CONTINUE
     *       DO 75  k= 2,KN
     *       DO 75  j= 2,JN
     *        ZR(j,k)= ZR(j,k)+T*ZU(j,k)
     *        ZZ(j,k)= ZZ(j,k)+T*ZV(j,k)
     * 75    CONTINUE
     */

    for ( l=1 ; l<=loop ; l++ ) {
        t = 0.0037;
        s = 0.0041;
        kn = 6;
        jn = n;
        for ( k=1 ; k<kn ; k++ ) {
#pragma nohazard
          for ( j=1 ; j<jn ; j++ ) {
              za[k][j] = ( zp[k+1][j-1] +zq[k+1][j-1] -zp[k][j-1] -zq[k][j-1] )*
                         ( zr[k][j] +zr[k][j-1] ) / ( zm[k][j-1] +zm[k+1][j-1]);
              zb[k][j] = ( zp[k][j-1] +zq[k][j-1] -zp[k][j] -zq[k][j] ) *
                         ( zr[k][j] +zr[k-1][j] ) / ( zm[k][j] +zm[k][j-1]);
          }
        }
        for ( k=1 ; k<kn ; k++ ) {
#pragma nohazard
            for ( j=1 ; j<jn ; j++ ) {
                zu[k][j] += s*( za[k][j]   *( zz[k][j] - zz[k][j+1] ) -
                                za[k][j-1] *( zz[k][j] - zz[k][j-1] ) -
                                zb[k][j]   *( zz[k][j] - zz[k-1][j] ) +
                                zb[k+1][j] *( zz[k][j] - zz[k+1][j] ) );
                zv[k][j] += s*( za[k][j]   *( zr[k][j] - zr[k][j+1] ) -
                                za[k][j-1] *( zr[k][j] - zr[k][j-1] ) -
                                zb[k][j]   *( zr[k][j] - zr[k-1][j] ) +
                                zb[k+1][j] *( zr[k][j] - zr[k+1][j] ) );
            }
        }
        for ( k=1 ; k<kn ; k++ ) {
#pragma nohazard
            for ( j=1 ; j<jn ; j++ ) {
                zr[k][j] = zr[k][j] + t*zu[k][j];
                zz[k][j] = zz[k][j] + t*zv[k][j];
            }
        }
    }
    argument = 18;
    TEST( &argument );

    /*
     *******************************************************************
     *   Kernel 19 -- general linear recurrence equations
     *******************************************************************
     *               KB5I= 0
     *           DO 194 L= 1,Loop
     *           DO 191 k= 1,n
     *         B5(k+KB5I)= SA(k) +STB5*SB(k)
     *               STB5= B5(k+KB5I) -STB5
     *191        CONTINUE
     *192        DO 193 i= 1,n
     *                  k= n-i+1
     *         B5(k+KB5I)= SA(k) +STB5*SB(k)
     *               STB5= B5(k+KB5I) -STB5
     *193        CONTINUE
     *194 CONTINUE
     */

    kb5i = 0;
    for ( l=1 ; l<=loop ; l++ ) {
        for ( k=0 ; k<n ; k++ ) {
            b5[k+kb5i] = sa[k] + stb5*sb[k];
            stb5 = b5[k+kb5i] - stb5;
        }
        for ( i=1 ; i<=n ; i++ ) {
            k = n - i ;
            b5[k+kb5i] = sa[k] + stb5*sb[k];
            stb5 = b5[k+kb5i] - stb5;
        }
    }
    argument = 19;
    TEST( &argument );

    /*
     *******************************************************************
     *   Kernel 20 -- Discrete ordinates transport, conditional recurrence on xx
     *******************************************************************
     *    DO 20 L= 1,Loop
     *    DO 20 k= 1,n
     *         DI= Y(k)-G(k)/( XX(k)+DK)
     *         DN= 0.2
     *         IF( DI.NE.0.0) DN= MAX( S,MIN( Z(k)/DI, T))
     *       X(k)= ((W(k)+V(k)*DN)* XX(k)+U(k))/(VX(k)+V(k)*DN)
     *    XX(k+1)= (X(k)- XX(k))*DN+ XX(k)
     * 20 CONTINUE
     */

    for ( l=1 ; l<=loop ; l++ ) {
        for ( k=0 ; k<n ; k++ ) {
            di = y[k] - g[k] / ( xx[k] + dk );
            dn = 0.2;
            if ( di ) {
                dn = z[k]/di ;
                if ( t < dn ) dn = t;
                if ( s > dn ) dn = s;
            }
            x[k] = ( ( w[k] + v[k]*dn )* xx[k] + u[k] ) / ( vx[k] + v[k]*dn );
            xx[k+1] = ( x[k] - xx[k] )* dn + xx[k];
        }
    }
    argument = 20;
    TEST( &argument );

    /*
     *******************************************************************
     *   Kernel 21 -- matrix*matrix product
     *******************************************************************
     *    DO 21 L= 1,Loop
     *    DO 21 k= 1,25
     *    DO 21 i= 1,25
     *    DO 21 j= 1,n
     *    PX(i,j)= PX(i,j) +VY(i,k) * CX(k,j)
     * 21 CONTINUE
     */

    for ( l=1 ; l<=loop ; l++ ) {
        for ( k=0 ; k<25 ; k++ ) {
            for ( i=0 ; i<25 ; i++ ) {
#pragma nohazard
                for ( j=0 ; j<n ; j++ ) {
                    px[j][i] += vy[k][i] * cx[j][k];
                }
            }
        }
    }
    argument = 21;
    TEST( &argument );

    /*
     *******************************************************************
     *   Kernel 22 -- Planckian distribution
     *******************************************************************
     *     EXPMAX= 20.0
     *       U(n)= 0.99*EXPMAX*V(n)
     *    DO 22 L= 1,Loop
     *    DO 22 k= 1,n
     *                                          Y(k)= U(k)/V(k)
     *       W(k)= X(k)/( EXP( Y(k)) -1.0)
     * 22 CONTINUE
     */

#pragma intrinsic exp
    expmax = 20.0;
    u[n-1] = 0.99*expmax*v[n-1];
    for ( l=1 ; l<=loop ; l++ ) {
        for ( k=0 ; k<n ; k++ ) {
            y[k] = u[k] / v[k];
            w[k] = x[k] / ( exp( y[k] ) -1.0 );
        }
    }
    argument = 22;
    TEST( &argument );

    /*
     *******************************************************************
     *   Kernel 23 -- 2-D implicit hydrodynamics fragment
     *******************************************************************
     *    DO 23  L= 1,Loop
     *    DO 23  j= 2,6
     *    DO 23  k= 2,n
     *          QA= ZA(k,j+1)*ZR(k,j) +ZA(k,j-1)*ZB(k,j) +
     *   .          ZA(k+1,j)*ZU(k,j) +ZA(k-1,j)*ZV(k,j) +ZZ(k,j)
     * 23  ZA(k,j)= ZA(k,j) +.175*(QA -ZA(k,j))
     */

    for ( l=1 ; l<=loop ; l++ ) {
        for ( j=1 ; j<6 ; j++ ) {
            for ( k=1 ; k<n ; k++ ) {
                qa = za[j+1][k]*zr[j][k] + za[j-1][k]*zb[j][k] +
                     za[j][k+1]*zu[j][k] + za[j][k-1]*zv[j][k] + zz[j][k];
                za[j][k] += 0.175*( qa - za[j][k] );
            }
        }
    }
    argument = 23;
    TEST( &argument );

    /*
     *******************************************************************
     *   Kernel 24 -- find location of first minimum in array
     *******************************************************************
     *     X( n/2)= -1.0E+10
     *    DO 24  L= 1,Loop
     *           m= 1
     *    DO 24  k= 2,n
     *          IF( X(k).LT.X(m))  m= k
     * 24 CONTINUE
     */

    x[n/2] = -1.0e+10;
    for ( l=1 ; l<=loop ; l++ ) {
        m = 0;
        for ( k=1 ; k<n ; k++ ) {
            if ( x[k] < x[m] ) m = k;
        }
    }
    argument = 24;
    TEST( &argument );

    /*
     *   Epilogue
     */

    for ( k=0 ; k<mk ; k++ ) {
        times[k][il-1][jr-1] = time[k];
        fopn[k][il-1][jr-1]  = flopn[k];
        terrs[k][il-1][jr-1] = terr1[k];
        npfs[k][il-1][jr-1]  = npfs1[k];
        csums[k][il-1][jr-1] = csum[k];
        dos[k][il-1][jr-1]   = total[k];
    }

    sum = 0.0;
    for ( k=0 ; k<mk ; k++ ) {
                        sum += time[k];
    }
    TK[0] += sum;

    som = 0.0;
    for ( k=0 ; k<mk ; k++ ) {
                        som += flopn[k]*total[k];
    }
    TK[1] += som;

    TRACK( &name );

    return;
}

/* End of File */

