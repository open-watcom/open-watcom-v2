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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#include "variety.h"
#include <stdio.h>
#include <stdarg.h>
#include <memory.h>
#include "printf.h"
#include "myvalist.h"
#include "xfloat.h"
#include "farsupp.h"

static void SetZeroPad( SPECS __SLIB *specs )
{
    int         n;

    if( !(specs->_flags & SPF_LEFT_ADJUST) ) {
        if( specs->_o._pad_char == '0' ) {
            n = specs->_o._fld_width - specs->_n0 - specs->_nz0 -
                         specs->_n1 - specs->_nz1 - specs->_n2 - specs->_nz2;
            if( n > 0 ) {
                specs->_nz0 += n;
            }
        }
    }
}


#define BUF_SIZ   40


/*
 * char *
 * InsertSpace(char *buffer, int spaces, char filler);
 *
 * Purpose: (pre-11.0 prtf() support function)
 *          Inserts `spaces` at the head of `buffer` and will optionally
 *          fill them with `filler`.
 *
 * Return : Pointer to first position AFTER the inserted spaces.
 */
static char *InsertSpace(char *buffer, int spaces, char filler)
{
    char *src = &buffer[strlen(buffer)];
    char *dst = src + spaces;

    if (spaces <= 0)
        return buffer;

    while (src >= buffer)
        *dst-- = *src--;

    for ( ; spaces > 0; --spaces, ++buffer)
        *buffer = filler;

    return buffer;
} /* InsertSpace() */


/*
 * void
 * HandleFormat(SPECS __SLIB *specs, char *buffer);
 *
 * Purpose: (pre-11.0 prtf() support function)
 *          Will simulate the pre-11.0 efgfmt() when formatting buffer.
 *
 * Return : void
 */
static void HandleFormat(SPECS __SLIB *specs, char *buffer)
{
    char *ptr;
    int   width;

    SetZeroPad(specs);

    width = specs->_n0 + specs->_nz0 + specs->_n1 + specs->_nz1 + specs->_n2
                + specs->_nz2;

    if (width >= BUF_SIZ)
    {
        memset(buffer, '*', (BUF_SIZ - 1));
        buffer[BUF_SIZ-1] = 0x00;
        return;
    }

    /*
     * If specs->_n0 > 0 then the first character in the buffer is in use.
     * Otherwise it is a garbage character and we want to skip over it.
     */
    if (!specs->_n0)
        strcpy(buffer, &buffer[1]);

    ptr = buffer;

    specs->_o._fld_width -= width;

    if (!(specs->_flags & SPF_LEFT_ADJUST))
    {
        if (specs->_o._pad_char == ' ')
            ptr = InsertSpace(ptr, specs->_o._fld_width, ' ');
    }

    ptr += specs->_n0;
    ptr  = InsertSpace(ptr, specs->_nz0, '0');
    ptr += specs->_n1;
    ptr  = InsertSpace(ptr, specs->_nz1, '0');
    ptr += specs->_n2;
    ptr  = InsertSpace(ptr, specs->_nz2, '0');

    if (specs->_flags & SPF_LEFT_ADJUST)
        ptr = InsertSpace(ptr, specs->_o._fld_width, ' ');

    *ptr = 0x00;
} /* HandleFormat() */


_WMRTLINK FAR_STRING _EFG_Format( char *buffer, my_va_list *args, SPECS __SLIB *specs )
{
    int         digits;
    int         fmt;
    CVT_INFO    cvt;
    auto double double_value;
    long_double ld;

    SPECS __SLIB work105;
    SPECS105 __SLIB *old105;

    /*
     * Test for pre-11.0 caller.
     */
    if (!specs->_o._alt_prefix[0])
    {
        old105 = (SPECS105 __SLIB *)specs;
        memset(&work105, 0, sizeof(work105));
        memcpy(&work105, &(specs->_o), sizeof(SPECS105));
        work105._flags = work105._o._flags;

        specs = (SPECS __SLIB *)&work105;
    }

    cvt.expchar = specs->_o._character; /* 'e', 'g' exponent character */
    digits = specs->_o._prec;
    fmt = specs->_o._character & 0x5F;
    if( fmt == 'G' ) {
        if( digits == 0 )  digits = 1;  /* 27-oct-88 */
        cvt.expchar -= 2;               /* change exponent to 'e' or 'E' */
        cvt.flags = G_FMT;
        cvt.scale = 1;
    } else if( fmt == 'E' ) {
        cvt.flags = E_FMT;
        cvt.scale = 1;
    } else {
        cvt.flags = F_FMT;
        cvt.scale = 0;
    }
    if( specs->_flags & SPF_ALT )  cvt.flags |= F_DOT;
    if( (specs->_flags & SPF_LONG_DOUBLE) &&
        sizeof(long double) > sizeof(double) ) {        /* 24-jun-94 */
        ld = va_arg( args->v, long_double );
    } else {
        double_value = va_arg( args->v, double );
#ifdef _LONG_DOUBLE_
        /* convert this double into a long double */
        __iFDLD( (double _WCNEAR *)&double_value, (long_double _WCNEAR *)&ld );
#else
        ld.value = double_value;
#endif
    }
    if( digits == -1 ) digits = 6;
    cvt.ndigits = digits;
    cvt.expwidth = 0;
    __LDcvt( &ld, &cvt, buffer + 1 );
    specs->_n1  = cvt.n1;
    specs->_nz1 = cvt.nz1;
    specs->_n2  = cvt.n2;
    specs->_nz2 = cvt.nz2;
    if( cvt.sign < 0 ) {
        buffer[specs->_n0++] = '-';
    } else if( specs->_flags & SPF_FORCE_SIGN ) {
        buffer[specs->_n0++] = '+';
    } else if( specs->_flags & SPF_BLANK ) {
        buffer[specs->_n0++] = ' ';
    }

    /*
     * If we were called from a pre-11.0 printf() then we need to copy
     * the data from the modified `work105' struct back into the SPECS105
     * struct passed in by the caller.
     */
    if (!specs->_o._alt_prefix[0])
    {
        HandleFormat(specs, buffer);
        work105._o._flags = work105._flags;
        memcpy(old105, &(work105._o), sizeof(SPECS105));

        /*
         * Some cludges to make sure that the pre-11.0 prtf() doesn't do
         * any additional formatting on the buffer.
         */
        old105->_flags = 0;
        old105->_prec  = 0;
    }
    else
    {
        /*
         * Signal the calling prtf() that this is an 11.0-style efgfmt().
         */
        specs->_o._alt_prefix[0] = 0x00;
    }

    return buffer;
} /* _EFG_Format() */

#ifdef __MAKE_DLL_MATHLIB
_WMRTLINK FAR_STRING (*__get_EFG_Format())() {
    return &_EFG_Format;
}
#endif
