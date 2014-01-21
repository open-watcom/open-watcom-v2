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
* Description:  Implements the functions used to test width computations:
*                   cop_text_width()
*               plus these local functions:
*                   scale_basis_to_horizontal_base_units()
****************************************************************************/

//#define __STDC_WANT_LIB_EXT1__ 1
//#include <math.h>
//#include <stdlib.h>
//#include <time.h>

/* Allocate storage for global variables. */

//#define global
//#include "copfiles.h"
//#undef  global

/* copfiles.h is included by several of these headers. If it is not included
 * first, its guard will prevent the global variables from being allocated.
 */

//#include "copdev.h"
//#include "copdrv.h"
//#include "copfon.h"
//#include "cophdr.h"
//#include "devfuncs.h"
//#include "findfile.h"
//#include "gvars.h"
//#include "outbuff.h"
//#include "wgml.h"
#include "widthtst.h"

/* Local function definitions. */

/* Function scale_basis_to_horizontal_base_units().
 * Converts a length expressed in scale_basis units to the same length
 * expressed in horizontal_base_units.
 *
 * Parameter:
 *      in_units contains the length in scale_basis units.
 *      in_font points to the font to use.
 *
 * Returns:
 *      the same length in horizontal_base_units.
 */
 
static uint32_t scale_basis_to_horizontal_base_units( uint32_t in_units, wgml_font * in_font )
{
    uint32_t    divisor;
    uint64_t    units;
    uint32_t    width;

    /* The conversion is done using this formula: 
     * horizontal_base_units * font_height/100 * in_units
     * ------------------------------------------------
     *                     scale_basis
     * font_height is reduced from centipoints to points. This is done by
     * adjusting the divisor to avoid loss-of-precision problems with
     * integer arithmetic.
     */

    divisor = (in_font->bin_font->scale_basis * 100);
    units = (bin_device->horizontal_base_units * in_font->font_height);
    units *= in_units;
    width = units / divisor;

    /* This rounds width up if the division did not produce an integer.
     * This produces correct results with the test values, but may need
     * to be modified (or the entire algorithm reconsidered) when
     * side-by-side comparisons of wgml 4.0 and our wgml become possible.
     */

    if( (units % divisor) > 0 ) width++;

    return( width );
}

/* Extern function definitions */

/* Function cop_text_width().
 * Returns the width, in horizontal_base_units, of the text given.
 *
 * Parameters:
 *      text is a pointer to the first character.
 *      count is the number of characters.
 *      font is the font number of the available font to use.
 *
 * Returns:
 *      the sum of the widths of the count characters starting with *text.
 */
 
uint32_t cop_text_width( char *text, uint32_t count, font_number font )
{
    int             i;
    uint32_t        units;
    uint32_t    *   table;
    uint32_t        width;

    if( font > wgml_font_cnt )
        font = 0;

    /* Compute the number of units. This will be either horizontal base units
     * or scale basis units, depending on whether the font is scaled.
     */

    if( wgml_fonts[font].bin_font->width == NULL ) {
        units = count * wgml_fonts[font].default_width;
    } else {
        table = wgml_fonts[font].bin_font->width->table;
        units = 0;
        for( i = 0; i < count; i++ ) {
            units += table[(uint8_t)text[i]];
        }
    }

    /* Convert from units to width. */

    if( wgml_fonts[font].bin_font->scale_basis == 0 ) {

        /* If the font is not scaled, the value of units is the width. */

        width = units;
    } else {

        /* If the font is scaled, the width is the scaled value of units. */

        width = scale_basis_to_horizontal_base_units( units, &wgml_fonts[font] );
    }

    return( width );
}

