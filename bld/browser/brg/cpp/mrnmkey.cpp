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


#include "mrnmkey.h"

MergeNameKey::MergeNameKey()
                : _tag( 0 )
                , _extern( FALSE )
                , _name( NULL )
                , _unique( 0 )
//--------------------------------
{
}

MergeNameKey::MergeNameKey( uint_32 tag, bool ext,
                            const MergeStringHdl& name, uint_32 unique )
                : _tag( tag )
                , _extern( ext )
                , _name( name )
                , _unique( unique )
//---------------------------------------------------------------------
{
}

MergeNameKey::MergeNameKey( uint_32 tag, bool ext,
                            const char * name, uint_32 unique )
                : _tag( tag )
                , _extern( ext )
                , _name( name )
                , _unique( unique )
//------------------------------------------------------------
{
}

MergeNameKey::MergeNameKey( const MergeNameKey& other )
//-----------------------------------------------------
{
    this->operator= ( other );
}

#if INSTRUMENTS
const char * MergeNameKey::getString() const
//------------------------------------------
{
    const   int     BufSize = 512 * 4;  //Must be larger than mrfile.h's MERGEFILESTRBUF
    static  char    buffer[ BufSize ];
    const char *    nm;

    nm = _name.getString();

    sprintf( buffer, "<Tag: %#x, %s, Name: \"%s\", %lu>", _tag, (_extern) ? "extern" : "static",
                (nm == NULL) ? "<NULL>" : nm, _unique );
    return buffer;
}
#endif
