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
* Description:  Errors messages only English
*
****************************************************************************/

#include "doserr.h"

extern char *Format(char *,char *,... );
extern char *StrCopy(char *,char *);

char *DosErrMsgs[] = {
        "",
        "invalid function number",
        "file not found",
        "path not found",
        "too many open files",
        "access denied",
        "invalid handle",
        "memory control blocks destroyed",
        "insufficient memory",
        "invalid memory block address",
        "invalid environment",
        "invalid format",
        "invalid access code",
        "invalid data",
        "", /* reserved */
        "invalid drive was specified",
        "attempt to remove current directory",
        "not same device",
        "no more files"
};

#define MAX_CODE (sizeof( DosErrMsgs ) / sizeof( char * ) - 1)

void GetDOSErrMsg( sys_error code, char *buff )
{
    if( code > MAX_CODE ) {
        Format( buff, "error #%u", code );
    } else {
        StrCopy( DosErrMsgs[ code ], buff );
    }
}
