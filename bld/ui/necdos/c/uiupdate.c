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


#include <stdlib.h>
#include <dos.h>
#include <conio.h>
#include "uidos.h"
#include "uiattrs.h"
#include "biosui.h"
#include "uidef.h"
#include "uibox.h"

#ifdef __WINDOWS__
#include <windows.h>

extern WORD FAR *       WindowsKernelPointerTo_A000h;
extern WORD FAR *       WindowsKernelPointerTo_A200h;
#else
LP_ZEN_CODE     code_pages[ 2 ];
LP_ZEN_ATTR     attr_pages[ 2 ];
extern int      BIOSCurPage;
#endif

extern bool uiIsHighRes();

extern void uisetmouseoff();
extern void uisetmouseon( MOUSEORD row, MOUSEORD col );
extern MOUSEORD MouseRow, MouseCol;

/*
 * This array contains the appropriate right byte for all the ZEN_BOX
 * character codes.
 */
static const unsigned short int box_chars[] =
{
    0x0b20,     // 0x00
    0x0b24,     // 0x01
    0x0b25,     // 0x02
    0x0b26,     // 0x03 vertical line.
    0x0b27,     // 0x04
    0x0b30,     // 0x05
    0x0b32,     // 0x06
    0x0b33,     // 0x07
    0x0b34,     // 0x08
    0x0b37,     // 0x09
    0x0b38,     // 0x0a
    0x0b3a,     // 0x0b
    0x0b3b,     // 0x0c
    0x0b3c,     // 0x0d
    0x0b3d,     // 0x0e
    0x0b3f,     // 0x0f
    0x0b40,     // 0x10
    0x0b41,     // 0x11
    0x0b48,     // 0x12
    0x0b49,     // 0x13
    0x0b5c,     // 0x14
    0x0b6f      // 0x15
};

/*
 * Definitive Box character test.
 */
#define IS_BOX_CHAR( ch ) \
        ((ch)<=0x15)

/*
 * Definitive Double Byte character test.
 */
unsigned short iszen( unsigned short c )
{
    unsigned char u, l;

    l = c;
    u = c >> 8;

    if( (u < 0x81) || (u > 0xFC) ) return 0;
    if( (u > 0x9F) && (u < 0xE0) ) return 0;
    if( (l < 0x40) || (l > 0xFC) ) return 0;
    if( l == 0x7f ) return 0;
    return( c );
}

/*
 * Definitive SHF_JIS to JIS converter
 */
unsigned short jmstojis( unsigned short msc )
{
    unsigned char u, l;

    l = msc;
    u = msc >> 8;

    u = (u - ((u < 0xa0) ? 0x71 : 0xb1)) * 2 + ((l >= 0x9f) ? 2 : 1);
    l -= (l >= 0x7f) ? ((l < 0x9f) ? 0x20 : 0x7e) : 0x1f;

    return( ( (unsigned short)u << 8 ) + l );
}

#define MOUSE_SUPRESS \
 { if( mouse_check_col ) uisetmouseoff(); }
#define MOUSE_RETRIEVE \
 { if( mouse_check_col ) uisetmouseon( MouseRow, MouseCol ); }


/*
 * the mighty physupdate:
 * Take the information stored in the virtual screen, and write it to
 * physical screen memory.
 */
void intern physupdate( SAREA* area )
{
    LPPIXEL             src;
    LP_ZEN_CODE         chardst;
    LP_ZEN_ATTR         attrdst;
    unsigned short      shift_jis_code;
    unsigned short      jis_code;
    char                c1, c2;
    ATTR                a1, a2;
    unsigned short      idx = 0;
    unsigned short      idy = 0;
    unsigned short      offset;
    char                mouse_check_row;
    char                mouse_check_col;

    offset = ( area->row * UIData->width + area->col );

#ifdef __WINDOWS__
    chardst = ( LP_ZEN_CODE )MK_FP(
                        FP_SEG( WindowsKernelPointerTo_A000h ),
                        FP_OFF( WindowsKernelPointerTo_A000h ) + ( offset * 2 )
                              );
    attrdst = ( LP_ZEN_ATTR )MK_FP(
                        FP_SEG( WindowsKernelPointerTo_A200h ),
                        FP_OFF( WindowsKernelPointerTo_A200h ) + ( offset * 2 )
                              );
#else
    chardst = &((code_pages[ BIOSCurPage ] )[ offset ]);
    attrdst = &((attr_pages[ BIOSCurPage ] )[ offset ]);
#endif

    src     = &(UIData->screen.origin[ offset ]);

    for( ;; ) {
        if( idy >= area->height ) break;
        if( MouseRow == (idy+area->row) ) {
            mouse_check_row = TRUE;
        } else {
            mouse_check_row = FALSE;
        }
        for( ;; ) {
            if( idx >= area->width ) break;
            mouse_check_row = FALSE;
            if( mouse_check_row ) {
                if( MouseCol == (idy+area->col) ) {
                    mouse_check_col = TRUE;
                }
            }
            MOUSE_SUPRESS;
            c1 = src->ch;
            c2 = (src+1)->ch;
            shift_jis_code = (c1 << 8) + c2;
            /*
             * Now, remap the attribute to work on the NEC.
             */
            a1 = src->attr;
            a2 = 1;
            a2 |= (((a1 & IBM_BLUE     ) ? NEC_BLUE      : 0) |
                   ((a1 & IBM_RED      ) ? NEC_RED       : 0) |
                   ((a1 & IBM_GREEN    ) ? NEC_GREEN     : 0) |
                   ((a1 & BLINK        ) ? NEC_BLINKING  : 0) |
                   ((a1 & UNDERLINE    ) ? NEC_UNDERLINE : 0) |
                   ((a1 & REVERSE      ) ? NEC_REVERSE   : 0));
//          if( a1 & UNDERLINE ) a2 &= ~NEC_REVERSE;
            if( IS_BOX_CHAR( src->ch ) ) {
#if !defined( __WINDOWS__ )
                if( ((src->ch==0x03 )||(src->ch==0x04)) &&
                     ( uiIsHighRes() ) ) {
                    // kludge to fix the vertical line on a hires screen.
                    // can't do it in windows, tho'....
                    attrdst->left       = a2 | NEC_REVERSE;
                    chardst->left       = 0x20;
                    chardst->right      = 0;
                } else {
#endif
                    attrdst->left               = a2;
                    chardst->left               = box_chars[ src->ch ] >> 8;
                    chardst->right              = (char)box_chars[ src->ch ];
#if !defined( __WINDOWS__ )
                }
#endif
            } else if( iszen( shift_jis_code ) ) {
                /* first check whether this is appropriate place to start
                   writing ( i.e. whether this is at the end of screen
                   or not ) If so, write a space ( which doesn't seem
                   like the most elegant solution possible... )
                */
                if( ( idx + area->col ) == UIData->width ) {
                    attrdst->left = a2;
                    chardst->left = 0x20; /* space character */
                    chardst->right = 0x00;
                } else {
                    attrdst->left = a2;
                    (attrdst+1)->left = a2;

                    jis_code = jmstojis( shift_jis_code );

                    chardst->right = (char) jis_code;
                    chardst->left = ( (char) ( jis_code >> 8 ) ) - 0x20;
                    (chardst+1)->right = (char) jis_code;
                    (chardst+1)->left = ( (char) ( jis_code >> 8 ) ) + 0x80 - 0x20;

                    /*
                     * Since we already processed the next character in
                     * line, we skip an iteration of the loop.
                     */
                    ++chardst; ++attrdst; ++idx; ++src;
                }
            } else {
                attrdst->left = a2;
                chardst->left = src->ch;
                chardst->right = 0x00;
            }
            MOUSE_RETRIEVE;
            ++chardst; ++attrdst; ++idx; ++src;
        }
        src     += UIData->width - idx;
        chardst += UIData->width - idx;
        attrdst += UIData->width - idx;
        idx = 0;
        idy++;
    }
//    RECALL_COMP();
//    RECALL_FILL();
}
/*
void DebugOutput( char * s )
{
    int idx;

    LP_ZEN_ATTR attrdst;
    LP_ZEN_CODE codedst;

    attrdst = ATTR_LOOKUP( 0, 0 );
    codedst = CODE_LOOKUP( 0, 0 );

    for( idx = 0; s[ idx ] != 0; idx++ ) {
        codedst[ idx ].left = s[ idx ];
        codedst[ idx ].right = 0;
        attrdst[ idx ].left = 0xe1;
    }
    BIOSGetKeyboard( 0 ); // wait for key.
}*/
