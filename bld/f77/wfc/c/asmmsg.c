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
// ASMMSG       : error message interface for in-line assembler
//

#include "ftnstd.h"
#include "cioconst.h"
#include "errcod.h"
#include "ferror.h"
#include "asmstmt.h"
#include "errutil.h"

#if _INTEL_CPU

void    AsmError( int msg ) {
//===========================

    char        buff[LIST_BUFF_SIZE+1];

    MsgBuffer( msg, buff );
    Error( CP_ERROR, &buff[1] ); // skip leading space
}

#else

void    AsmError( const char *msg )
//=================================
{
    Error( CP_ERROR, msg );
}

void    AsmWarning( const char *msg )
//===================================
{
    Error( CP_ERROR, msg );
}

#endif
