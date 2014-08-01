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


#include "global.h"
#include "errors.h"
#include "semantic.h"
#include "clibext.h"
#include "rccore.h"

const FullAccelFlags DefaultAccelFlags = { 0, FALSE };

int SemWINStrToAccelEvent( char * string )
/****************************************/
{
    if (*string == '^') {
        /* control character requested */
        string++;
        if (isalpha( *string )) {
            /* assume we are using the ASCII charater set to get the */
            /* corresponding code for control-letter */
            return( toupper( *string ) - 0x40 );
        } else {
            return( 0 );
        }
    } else if (isprint( *string )) {
        /* only accept printable characters in this position */
        return( *string );
    } else {
        return( 0 );
    }
}

static void CheckAccelFlags( AccelFlags * flags, unsigned long idval )
/********************************************************************/
{
    if ( !( *flags & ACCEL_VIRTKEY ) ) {
        if (*flags & ACCEL_SHIFT) {
            *flags &= ~ACCEL_SHIFT;
            RcWarning( ERR_ACCEL_KEYWORD_IGNORED, "SHIFT", idval );
        }
        if (*flags & ACCEL_CONTROL) {
            *flags &= ~ACCEL_CONTROL;
            RcWarning( ERR_ACCEL_KEYWORD_IGNORED, "CONTROL", idval );
        }
    }
}

FullAccelEntry SemWINMakeAccItem( AccelEvent event, unsigned long idval,
                    FullAccelFlags flags )
/***********************************************************************/
{
    FullAccelEntry      entry;

    if (event.strevent || flags.typegiven) {
        CheckAccelFlags( &flags.flags, idval );
        if( CmdLineParms.TargetOS == RC_TARGET_OS_WIN16 ) {
            entry.Win32 = FALSE;
            entry.u.entry.Ascii = event.event;
            entry.u.entry.Flags = flags.flags;
            entry.u.entry.Id = idval;
        } else {
            entry.Win32 = TRUE;
            entry.u.entry32.Ascii = event.event;
            entry.u.entry32.Flags = flags.flags;
            entry.u.entry32.Id = idval;
            entry.u.entry32.Unknown = 0;
        }
    } else {
        RcError( ERR_ACCEL_NO_TYPE, idval );
        ErrorHasOccured = TRUE;
        entry.Win32 = FALSE;
        entry.u.entry.Ascii = 0;
        entry.u.entry.Flags = 0;
        entry.u.entry.Id = 0;
    }

    return( entry );
}

void SemWINWriteAccelEntry( FullAccelEntry entry )
/************************************************/
{
    int     error;

    if (!ErrorHasOccured) {
        if( entry.Win32 ) {
            error = ResWriteAccelEntry32( &entry.u.entry32, CurrResFile.handle );
        } else {
            error = ResWriteAccelEntry( &entry.u.entry, CurrResFile.handle );
        }
        if (error) {
            RcError( ERR_WRITTING_RES_FILE, CurrResFile.filename,
                     LastWresErrStr() );
            ErrorHasOccured = TRUE;
        }
    }
}

void SemWINWriteLastAccelEntry( FullAccelEntry entry )
/****************************************************/
{
    if( entry.Win32 ) {
        entry.u.entry32.Flags |= ACCEL_LAST;
    } else {
        entry.u.entry.Flags |= ACCEL_LAST;
    }
    SemWINWriteAccelEntry( entry );
}
