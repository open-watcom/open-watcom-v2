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


//
// SYSINQ       : system dependent INQUIRE support
//

#include "ftnstd.h"
#include "ftnio.h"

extern  bool                    IsDevice(ftnfile *);


int     InqSeq( ftnfile *fcb ) {
//==============================

// Return ACCM_SEQUENTIAL if sequential is in the allowed set of access
//                        modes for the given file,
//        ACCM_DIRECT     if not,
//        ACCM_DEFAULT    if we can't decide.

    if( fcb->accmode == ACCM_DIRECT ) {
        if( fcb->formatted == FORMATTED_IO ) {
            return( ACCM_SEQUENTIAL );
        }
        if( fcb->recfm == RECFM_VARIABLE ) {
            return( ACCM_SEQUENTIAL );
        }
        return( ACCM_DIRECT );
    }
    return( ACCM_SEQUENTIAL );
}


int     InqDir( ftnfile *fcb ) {
//==============================

// Return ACCM_DIRECT     if direct is in the allowed set of access modes
//                        for the given file,
//        ACCM_SEQUENTIAL if not,
//        ACCM_DEFAULT    if we can't decide.
//

    if( IsDevice( fcb ) ) {
        return( ACCM_SEQUENTIAL );
    }
    if( fcb->accmode == ACCM_DIRECT ) {
        return( ACCM_DIRECT );
    }
    if( fcb->recfm == RECFM_FIXED ) {
        return( ACCM_DIRECT );
    }
    return( ACCM_SEQUENTIAL );
}


int     InqFmtd( ftnfile *fcb ) {
//===============================

// Return FORMATTED_IO   if formatted is "in the allowed set of format
//                       modes for the given file",
//        UNFORMATTED_IO if not,
//        FORMATTED_DFLT if we can't decide.
//

    return( fcb->formatted );
}


int     InqUnFmtd( ftnfile *fcb ) {
//=================================

// Return UNFORMATTED_IO if unformatted is "in the allowed set of format
//                       modes for the given file",
//        FORMATTED_IO   if not,
//        FORMATTED_DFLT if we can't decide.
//

    return( fcb->formatted );
}
