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


#define _FRENCH         // turn on French character support
#include "gdefn.h"


static short            Map( short, short, short, short );
static void             DrawChar( char, short, short, short, short, short, short );


#if defined( _FRENCH )
#include "french.h"


/* The three CodePage arrays contain indices into the
   MultiLingIndex array to avoid repetition of font data. */

static unsigned char CodePage437[ 99 ] = {
            98,                                         // number of entries
        C_CEDL, u_UMLT, e_AIGU, a_CIRC, a_UMLT, a_GRAV,   0xff, c_CEDL,
        e_CIRC, e_UMLT, e_GRAV, i_UMLT, i_CIRC, i_GRAV, A_UMLT,   0xff,
        E_AIGU,   0xff,   0xff, o_CIRC, o_UMLT, o_GRAV, u_CIRC, u_GRAV,
        y_UMLT, O_UMLT, U_UMLT,   0xff,   0xff,   0xff,   0xff,   0xff,
        a_AIGU, i_AIGU, o_AIGU, u_AIGU,   0xff,   0xff,   0xff,   0xff,
          0xff,   0xff,   0xff,   0xff,   0xff,   0xff, L_ARRO, R_ARRO,
          0xff,   0xff,   0xff,   0xff,   0xff,   0xff,   0xff,   0xff,
          0xff,   0xff,   0xff,   0xff,   0xff,   0xff,   0xff,   0xff,
          0xff,   0xff,   0xff,   0xff,   0xff,   0xff,   0xff,   0xff,
          0xff,   0xff,   0xff,   0xff,   0xff,   0xff,   0xff,   0xff,
          0xff,   0xff,   0xff,   0xff,   0xff,   0xff,   0xff,   0xff,
          0xff,   0xff,   0xff,   0xff,   0xff,   0xff,   0xff,   0xff,
          0xff, b_BETA
};

static unsigned char CodePage863[ 99 ] = {
            98,                                         // number of entries
        C_CEDL, u_UMLT, e_AIGU, a_CIRC, A_CIRC, a_GRAV,   0xff, c_CEDL,
        e_CIRC, e_UMLT, e_GRAV, i_UMLT, i_CIRC,   0xff, A_GRAV,   0xff,
        E_AIGU, E_GRAV, E_CIRC, o_CIRC, E_UMLT, I_UMLT, u_CIRC, u_GRAV,
          0xff, O_CIRC, U_UMLT,   0xff,   0xff, U_GRAV, U_CIRC,   0xff,
          0xff,   0xff, o_AIGU, u_AIGU,   0xff,   0xff,   0xff,   0xff,
        I_CIRC,   0xff,   0xff,   0xff,   0xff,   0xff, L_ARRO, R_ARRO,
          0xff,   0xff,   0xff,   0xff,   0xff,   0xff,   0xff,   0xff,
          0xff,   0xff,   0xff,   0xff,   0xff,   0xff,   0xff,   0xff,
          0xff,   0xff,   0xff,   0xff,   0xff,   0xff,   0xff,   0xff,
          0xff,   0xff,   0xff,   0xff,   0xff,   0xff,   0xff,   0xff,
          0xff,   0xff,   0xff,   0xff,   0xff,   0xff,   0xff,   0xff,
          0xff,   0xff,   0xff,   0xff,   0xff,   0xff,   0xff,   0xff,
          0xff, b_BETA
};

static unsigned char CodePage850[ 111 ] = {
           110,                                         // number of entries
        C_CEDL, u_UMLT, e_AIGU, a_CIRC, a_UMLT, a_GRAV,   0xff, c_CEDL,
        e_CIRC, e_UMLT, e_GRAV, i_UMLT, i_CIRC, i_GRAV, A_UMLT,   0xff,
        E_AIGU,   0xff,   0xff, o_CIRC, o_UMLT, o_GRAV, u_CIRC, u_GRAV,
        y_UMLT, O_UMLT, U_UMLT,   0xff,   0xff,   0xff,   0xff,   0xff,
        a_AIGU, i_AIGU, o_AIGU, u_AIGU,   0xff,   0xff,   0xff,   0xff,
          0xff,   0xff,   0xff,   0xff,   0xff,   0xff, L_ARRO, R_ARRO,
          0xff,   0xff,   0xff,   0xff,   0xff, A_AIGU, A_CIRC, A_GRAV,
          0xff,   0xff,   0xff,   0xff,   0xff,   0xff,   0xff,   0xff,
          0xff,   0xff,   0xff,   0xff,   0xff,   0xff,   0xff,   0xff,
          0xff,   0xff,   0xff,   0xff,   0xff,   0xff,   0xff,   0xff,
          0xff,   0xff, E_CIRC, E_UMLT, E_GRAV,   0xff, I_AIGU, I_CIRC,
        I_UMLT,   0xff,   0xff,   0xff,   0xff,   0xff, I_GRAV,   0xff,
        O_AIGU, b_BETA, O_CIRC, O_GRAV,   0xff,   0xff,   0xff,   0xff,
          0xff, U_AIGU, U_CIRC, U_GRAV, y_AIGU, Y_AIGU
};


// Multi-lingual characters are made up by drawing two characters,
// the regular character, followed by the accent.

static unsigned char MultiLingIndex[ NUM_LANG_CHARS ][ 2 ] = {
        { 'a',             LC_AIGU },       // a_AIGU
        { 'A',             UC_AIGU },       // A_AIGU
        { 'e',             LC_AIGU },       // e_AIGU
        { 'E',             UC_AIGU },       // E_AIGU
        { UNDOTTED_I,      LC_AIGU },       // i_AIGU
        { 'I',             UC_AIGU },       // I_AIGU
        { 'o',             LC_AIGU },       // o_AIGU
        { 'O',             UC_AIGU },       // O_AIGU
        { 'u',             LC_AIGU },       // u_AIGU
        { 'U',             UC_AIGU },       // U_AIGU
        { 'y',             LC_AIGU },       // y_AIGU
        { 'Y',             UC_AIGU },       // Y_AIGU
        { 'a',             LC_GRAVE },      // a_GRAV
        { 'A',             UC_GRAVE },      // A_GRAV
        { 'e',             LC_GRAVE },      // e_GRAV
        { 'E',             UC_GRAVE },      // E_GRAV
        { UNDOTTED_I,      LC_GRAVE },      // i_GRAV
        { 'I',             UC_GRAVE },      // I_GRAV
        { 'o',             LC_GRAVE },      // o_GRAV
        { 'O',             UC_GRAVE },      // O_GRAV
        { 'u',             LC_GRAVE },      // u_GRAV
        { 'U',             UC_GRAVE },      // U_GRAV
        { 'a',             LC_UMLAUT },     // a_UMLT
        { 'A',             UC_UMLAUT },     // A_UMLT
        { 'e',             LC_UMLAUT },     // e_UMLT
        { 'E',             UC_UMLAUT },     // E_UMLT
        { UNDOTTED_I,      LC_UMLAUT },     // i_UMLT
        { 'I',             UC_UMLAUT },     // I_UMLT
        { 'o',             LC_UMLAUT },     // o_UMLT
        { 'O',             UC_UMLAUT },     // O_UMLT
        { 'u',             LC_UMLAUT },     // u_UMLT
        { 'U',             UC_UMLAUT },     // U_UMLT
        { 'y',             LC_UMLAUT },     // y_UMLT
        { 'a',             LC_CIRC },       // a_CIRC
        { 'A',             UC_CIRC },       // A_CIRC
        { 'e',             LC_CIRC },       // e_CIRC
        { 'E',             UC_CIRC },       // E_CIRC
        { UNDOTTED_I,      LC_CIRC },       // i_CIRC
        { 'I',             UC_CIRC },       // I_CIRC
        { 'o',             LC_CIRC },       // o_CIRC
        { 'O',             UC_CIRC },       // O_CIRC
        { 'u',             LC_CIRC },       // u_CIRC
        { 'U',             UC_CIRC },       // U_CIRC
        { 'c',             CEDILLE },       // c_CEDL
        { 'C',             CEDILLE },       // C_CEDL
        { LFT_DBL_ARROW,   LFT_DBL_ARROW }, // L_ARRO
        { RGT_DBL_ARROW,   RGT_DBL_ARROW }, // R_ARRO
        { BETA,            BETA }           // b_BETA
};

#endif


void _HershDraw( char pc, short cx, short cy, short bx, short by,
/*=========================================*/ short px, short py )

{
    unsigned char   ch = pc;

    if( ch >= '!' && ch <= 0x7e ) {
        DrawChar( ch, cx, cy, bx, by, px, py );
#if defined( _FRENCH )
    } else if( ch >= 0x80 ) {
        short           codepage;
        unsigned char   *cp_array;

        codepage = GetCodePage();
        if( codepage == 850 ) {
            cp_array = CodePage850;
        } else if( codepage == 863 ) {
            cp_array = CodePage863;
        } else {        // assume that we use 437 for all others
            cp_array = CodePage437;
        }
        ch -= 0x80;
        if( ch > cp_array[ 0 ] ) {
            return;
        }
        ch = cp_array[ ch + 1 ];     // skip over size
        if( ch == 0xff ) {
            return;
        }
        DrawChar( MultiLingIndex[ ch ][ 0 ], cx, cy, bx, by, px, py );
        DrawChar( MultiLingIndex[ ch ][ 1 ], cx, cy, bx, by, px, py );
#endif
    }
}


static void DrawChar( char ch, short cx, short cy, short bx, short by,
/*==============================================*/ short px, short py )

{
    signed char _WCI86FAR *  data_ptr;
    unsigned short      offset;
    short               len;
    short               a;
    short               b;
    short               x0;
    short               y0;
    short               x1;
    short               y1;
    char                join_points;

    offset = _CurrFont->key[ ch - '!' ];
    len = _CurrFont->key[ ch - '!' + 1 ] - offset;
    data_ptr = (signed char _WCI86FAR *)&_CurrFont->data + offset;

    join_points = FALSE;
    while( len > 0 ) {
        a = *data_ptr;
        ++data_ptr;
        --len;
        if( a == -99 ) {                        /* Stroke is done   */
            join_points = FALSE;
        } else {
            b = *data_ptr;
            ++data_ptr;
            --len;
            x1 = px + Map( a, b, bx, cx );      /* map (a,b) into char  */
            y1 = py + Map( a, b, by, cy );      /* parallelogram        */
            if( join_points ) {
                _L1SLine( x0, y0, x1, y1 );     /* use solid line   */
            } else {
                join_points = TRUE;
            }
            x0 = x1;                            /* start of previous    */
            y0 = y1;                            /* point                */
        }
    }
}


static short Map( short a, short b, short v1, short v2 )
/*======================================================

    Map the point (a,b) inside the character definition grid to the
    character drawing parallelogram. Return one component at a time.

    Calculate: (x', y') = ( a * vx1 + b * vx2 , a * vy1 + b * vy2 ) / MAX
 */

{
    signed long         p1;
    signed long         p2;

    p1 = (long)a * v1;
    p2 = (long)b * v2;
    p1 += p2;
    if( p1 < 0 ) {
        p1 -= MAX/2;                    /* round point to nearest pixel */
    } else {
        p1 += MAX/2;                    /* round point to nearest pixel */
    }
    return( (long)p1 / MAX );
}
