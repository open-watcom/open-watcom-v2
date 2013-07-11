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
//  HCERRORS.H      --Functions to print error messages and
//            informational messages for the WATCOM .HLP compiler.
//

#ifndef HCERRORS_H
#define HCERRORS_H

#include "errnums.h"
#include <stddef.h> // for NULL

// Exception to throw in case of a call to HCError().
struct HCException {};

// The "Fatal Error" function.  This will terminate the program
// by throwing an exception.
extern void HCError( int err_num, ... );

// The "Non-fatal Error" function.  This will not terminate the program.
extern void HCWarning( int err_num, ... );

// Some informative messages.
extern void SetQuiet( int be_quiet );
extern void HCStartFile( char const name[] );
extern void HCTick();
extern void HCDoneTick();
extern void HCStartOutput();
extern void HCStartPhrase();
extern void HCPhraseLoop( int pass );

// A callback function for the memory tracker if it is in use.
extern "C" void HCMemerr( void );

#endif
