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


#include "dbgdefn.h"
#include "dbglit.h"

#include "doserr.h"

extern char *Format(char *,char *,... );
extern char *StrCopy(char *,char *);

char **DosErrMsgs[] = {
    LITREF( Empty ),
    LITREF( DOS_invalid_function ),
    LITREF( DOS_file_not_found ),
    LITREF( DOS_path_not_found ),
    LITREF( DOS_too_many_open_files ),
    LITREF( DOS_access_denied ),
    LITREF( DOS_invalid_handle ),
    LITREF( DOS_memory_control ),
    LITREF( DOS_insufficient_memory ),
    LITREF( DOS_invalid_address ),
    LITREF( DOS_invalid_environment ),
    LITREF( DOS_invalid_format ),
    LITREF( DOS_invalid_access_code ),
    LITREF( DOS_invalid_data ),
    LITREF( Empty ),
    LITREF( DOS_invalid_drive ),
    LITREF( DOS_remove_cd ),
    LITREF( DOS_not_same_device ),
};

#define MAX_CODE (sizeof( DosErrMsgs ) / sizeof( char * ) - 1)

void GetDOSErrMsg( sys_error code, char *buff )
{
    if( code > MAX_CODE ) {
        Format( buff, "error #%u", code );
    } else {
        StrCopy( *DosErrMsgs[ code ], buff );
    }
}
