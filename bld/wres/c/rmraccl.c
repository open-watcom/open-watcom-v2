/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2022 The Open Watcom Contributors. All Rights Reserved.
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


#include "layer0.h"
#include "filefmt.h"
#include "read.h"
#include "resaccel.h"
#include "reserr.h"
#include "wresrtns.h"
#include "rmmacs.h"


bool IsAccelTableLastEntry( const char *data )
{
    return( (VALU8( data ) & ACCEL_LAST) != 0 );
}

bool IsAccelTableLastEntry32( const char *data )
{
    return( (VALU16( data ) & ACCEL_LAST) != 0 );
}

char *ResReadDataAccelTableEntry( const char *data, AccelTableEntry *entry )
{
    entry->Flags = VALU8( data ) & ~ACCEL_LAST;
    INCU8( data );
    entry->Ascii = VALU16( data );
    INCU16( data );
    entry->Id = VALU16( data );
    INCU16( data );
    return( (char *)data );
}

char *ResReadDataAccelTableEntry32( const char *data, AccelTableEntry32 *entry )
{
    entry->Flags = VALU16( data ) & ~ACCEL_LAST;
    INCU16( data );
    entry->Ascii = VALU16( data );
    INCU16( data );
    entry->Id = VALU16( data );
    INCU16( data );
    entry->Unknown = VALU16( data );
    INCU16( data );
    return( (char *)data );
}
