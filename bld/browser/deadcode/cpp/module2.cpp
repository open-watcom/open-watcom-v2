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


#include <wfilenam.hpp>

#include "busyntc.h"
#include "dwmerger.h"
#include "module.h"
#include "util.h"

void Module::mergeComponents( void )
//----------------------------------
{
    #if 0
    int i;
    BusyNotice busy( "Merging components..." );

    /* make sure _dataFile contains the correct components */

    _dataFile->resetComponents();
    for( i = 0; i < _enabledFiles.count(); i += 1 ) {
        _dataFile->addComponentFile( ((WFileName *)_enabledFiles[ i ])->gets(), TRUE );
    }
    for( i = 0; i < _disabledFiles.count(); i += 1 ) {
        _dataFile->addComponentFile( ((WFileName *)_disabledFiles[ i ])->gets(), FALSE );
    }

    _dataFile->endRead();

    try {
        DwarfFileMerger merger( _dataFile );
        merger.doMerge();
    }
    catch ( ... ) {
        errMessage( "Unknown error while merging" ); // NYI -- fix this up!
    }

    _dataFile->initSections();
    _dbgInfo = DRDbgInit( this, _dataFile->getDRSizes() );
    DRSetDebug( _dbgInfo );
    #endif
}
