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


#include "uidef.h"
#include "uimouse.h"
#include "stdui.h"
#include "biosui.h"

#if 0
global  void (intern *DrawCursor)(void) = NULL;
global  void (intern *EraseCursor)(void) = NULL;
#endif

#define OFF_SCREEN      200     /* not offscreen for > 320x200 resolutions */

extern MOUSEORD MouseRow, MouseCol;

/*
 * The next three variables are stolen out of uibios.c
 */
#ifdef __WINDOWS__
#include <windows.h>

extern WORD FAR *       WindowsKernelPointerTo_A000h;
extern WORD FAR *       WindowsKernelPointerTo_A200h;
#else
extern LP_ZEN_CODE      code_pages[];
extern LP_ZEN_ATTR      attr_pages[];
extern int              BIOSCurPage;
#endif

static MOUSEORD         OldMouseRow, OldMouseCol = OFF_SCREEN;
static bool             MouseOn = FALSE;
static char             OldCodeType = ZEN_NOTHING_IN_PARTICULAR;

static ATTR             OldAttr;

/*
 * BuddhaNature -
 * An important cursor-drawing function. It looks at a WORD handed over
 * from screen memory and determines its Buddha-nature.
 *
 * More specifically, it tells you whether the character you're looking at
 * is a box character, a double-byte prefix, a double-byte suffix, or
 * anything else.
 */
unsigned char BuddhaNature( char msb, char lsb )
{
    if( lsb == 0 ) {
        return( ZEN_NOTHING_IN_PARTICULAR );
    } else if( msb == ZEN_BOX_PREFIX ) {
        return( ZEN_BOX_CHAR );
    } else {
        /*
         * Definite Kanji character.  But is it left (yin) or right (yang)?
         */
        if( msb & 0x80 ) {
            /*
             * High bit on, Buddha-nature is yang
             */
            return( ZEN_DB_SUFFIX );
        } else {
            return( ZEN_DB_PREFIX );
        }
    }
}

void uisetmouseoff()
{
    LP_ZEN_ATTR         attrdst;

    if( MouseOn ) {

//        if( EraseCursor==NULL ) {
            /*
             * Write the new info the screen memory.
             */
            attrdst = ATTR_LOOKUP( OldMouseRow, OldMouseCol );
            attrdst->left = OldAttr;
            if( OldCodeType == ZEN_DB_PREFIX ) (attrdst+1)->left = OldAttr;
//        } else {
//           (*EraseCursor)();               /*  Hide text-graphics mouse    */
//        }
    }
}

void uisetmouseon( MOUSEORD row, MOUSEORD col )
{
    LP_ZEN_ATTR         attrdst;
    LP_ZEN_CODE         codedst;

    if( MouseOn ){
//        if( DrawCursor==NULL ) {
            attrdst = ATTR_LOOKUP( row, col );
            codedst = CODE_LOOKUP( row, col );

            OldAttr = attrdst->left;
            attrdst->left ^= 0x04; // Reverse the character.

            OldCodeType = BuddhaNature( codedst->left, codedst->right );
            if( OldCodeType == ZEN_DB_PREFIX ) {
                (attrdst+1)->left ^= 0x04;
            }
//      } else {
//            (*DrawCursor)();
//        }
        OldMouseRow = row;
        OldMouseCol = col;
    }
}


/*
 * This is the entry point for mouse drawing.  As such, it will be made
 * responsible for aligning the cursor on the beginning of double-byte
 * characters when the need arises.  All the remaining functions here can
 * then rely on the invariant that the coordinates they get passed point
 * either to a single byte character, or to the LEFT word of a kanji
 * character.
 */
void global uisetmouse( MOUSEORD row, MOUSEORD col )
{
    /*
     * First, perform any alignment necessary to put the cursor at the
     * start of the character in question.
     */
    LP_ZEN_CODE         codedst;
    char                nature;

    codedst = CODE_LOOKUP( row, col );
    nature = BuddhaNature( codedst->left, codedst->right );
    if( nature == ZEN_DB_SUFFIX ) {
        /*
         * This is the only necessary adjustment - shift left one column
         * if we're on the right half of a Kanji character.
         */
        col--;
    }

    /*
     * Then, get on with the standard part of the function.
     */
    if( OldMouseRow == row && OldMouseCol == col ) return;
    uisetmouseoff();
    uisetmouseon( row, col );
}


void global uimouse( func )
{
    if( func == MOUSE_ON ) {
        MouseOn = TRUE;
        uisetmouseon( OldMouseRow, OldMouseCol );
    } else {
        uisetmouseoff();
        MouseOn = FALSE;
    }
}
