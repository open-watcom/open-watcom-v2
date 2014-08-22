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


#include "vi.h"
#include "win.h"

/*
 * GetCurrentLine - copy current line into work buffer
 */
void GetCurrentLine( void )
{
    memcpy( WorkLine->data, CurrentLine->data, CurrentLine->len + 1 );
    WorkLine->len = CurrentLine->len;
    CurrentLine->u.ld.nolinedata = true;

} /* GetCurrentLine */

/*
 * ReplaceCurrentLine - replace current line with work line
 */
vi_rc ReplaceCurrentLine( void )
{
    int         extra;
    line        *tmp;

    /*
     * add extra space to fcb
     */
    FetchFcb( CurrentFcb );
    extra = WorkLine->len - CurrentLine->len;
    CurrentFcb->byte_cnt += extra;
    CurrentFcb->nullfcb = false;

    /*
     * copy new data in
     */
    tmp = LineAlloc( WorkLine->data, WorkLine->len );
    tmp->u.ld.mark = CurrentLine->u.ld.mark;
    ReplaceLLItem( (ss **)&CurrentFcb->lines.head, (ss **)&CurrentFcb->lines.tail,
                   (ss *)CurrentLine, (ss *)tmp );
    MemFree( CurrentLine );
    CurrentLine = tmp;

    WorkLine->len = -1;

    return( CheckCurrentFcbCapacity() );

} /* ReplaceCurrentLine */

/*
 * DisplayWorkLine - display the working copy of the current line
 */
void DisplayWorkLine( bool killsFlags )
{
    int i;

    if( killsFlags ) {
        DCDisplayAllLines();
    } else {
        // could speed up a little by calling directly
        i = (int)( CurrentPos.line - LeftTopPos.line );
        DCDisplaySomeLines( i, i );
    }

} /* DisplayWorkLine */
