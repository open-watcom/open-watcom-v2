/****************************************************************************
*
*                            Open Watcom Project
*
*    Portions Copyright (c) 1983-2002 Sybase, Inc. All Rights Reserved.
*
*  ========================================================================
*
*    This file contains Original Code and/or Modifications of Original
*    Code as defined in and that are subject to the Sybase Open Watcom
*    Public License version 1.0 (the 'License'). You may not use this file
*    except in compliance with the License. BY USING THIS FILE YOU AGREE TO
*    ALL TERMS AND CONDITIONS OF THE LICENSE. A copy of the License is
*    provided with the Original Code and Modifications, and is also
*    available at www.sybase.com/developer/opensource.
*
*    The Original Code and all software distributed under the License are
*    distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
*    EXPRESS OR IMPLIED, AND SYBASE AND ALL CONTRIBUTORS HEREBY DISCLAIM
*    ALL SUCH WARRANTIES, INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF
*    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR
*    NON-INFRINGEMENT. Please see the License for the specific language
*    governing rights and limitations under the License.
*
*  ========================================================================
*
* Description:  Test of basic debugger functionality.
*
****************************************************************************/


/* This is a simple but not entirely trivial test designed to exercise basic
 * debugger functionality, as well as implied test of debugging information
 * representation as produced by compiler and linker.
 *
 * Tested areas:
 *
 * - Representation of source file and line information
 * - Single stepping through code
 * - Representation of basic integer types
 * - Representation of floating point types
 * - Representation of structures
 * - Representation of unions
 * - Representation of bitfields
 * - Representation of pointers
 * - Access to global and local variables
 * - Stepping into and out of procedures
 * - Stack unwinding
 * - Dynamic stack frames (alloca())
 *
 */

/* Too bad alloca() isn't standardized */
#if defined( __WATCOMC__ ) || defined( _MSC_VER ) || defined( __IBMC__ ) || defined( __BORLANDC__ )
    #include <malloc.h>
#else
    #include <alloca.h>
#endif

struct s1_t {
    int     int_fld;
    double  dbl_fld;    /* intentionally misaligned (on most platforms) */
    float   flt_fld;
    short   sht_fld;
};

struct bf1_t {
    int     bit_fld_1   : 1;
    int     bit_fld_2   : 2;
    int     bit_fld_3   : 3;
    int     bit_fld_10  : 10;
    int     bit_fld_16  : 16;
};

typedef struct s2_t {
    struct s1_t     struc_fld;
    struct bf1_t    bit_fld;
} s2_t;

union u1_t {
    s2_t            struct_fld_1;
    struct bf1_t    struct_fld_2;
};

/* A few pre-initialized globals */

union u1_t      U_glob = { { { 34, 2.734, 1.256f, 0x1234 } } };
struct s2_t     S_glob = { { 0x7654321, 5.57, 0.25f, 12 }, { 1, 2, 3, 4, 5 } };

int bar( int arg )
{
    long    *l_p;

    /* Test dynamic stack frame */
    l_p = alloca( arg * sizeof( long ) );
    l_p[0] = 21;
    l_p[1] = 4;
    return( l_p[0] );
}

int foo( int *arg )
{
    int     temp;

    temp = bar( *arg );
    return( temp + 1 );
}

int main( int argc, char **argv )
{
    struct s1_t     s1_var;
    float           flt_var = 3.14;
    double          dbl_var = 3.14;
    int             i, j;
    int             *p_i = &i;
    signed char     sc = -1;
    unsigned char   uc = 255;

    i = 42;
    s1_var = S_glob.struc_fld;
    s1_var.sht_fld += argc;
    j = foo( p_i );
    dbl_var += flt_var;
    j += flt_var;
    {   /* Test variable scoping */
        int     i = 3;

        j += i;
    }
    sc += uc;
    return( j );
}
