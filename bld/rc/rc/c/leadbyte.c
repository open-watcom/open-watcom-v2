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


#include <stdlib.h>
#include "watcom.h"
#include "leadbyte.h"
#include "clibext.h"
#include "rccore.h"

#ifdef __NT__

#include <windows.h>

void SetNativeLeadBytes( void ) {
/*********************************/
    CPINFO      info;
    unsigned    i;

    GetCPInfo( CP_ACP, &info );
    i = 0;
    while( info.LeadByte[ i ] != 0 && info.LeadByte[i+1] != 0 ) {
        SetMBRange( info.LeadByte[i], info.LeadByte[i+1], 1 );
        i += 2;
    }
}

int NativeDBStringToUnicode( int len, const char *str, char *buf ) {
/*******************************************************************/
    int         ret;
    unsigned    outlen;

    if( len > 0 ) {
        if( buf == NULL ) {
            outlen = 0;
        } else {
            outlen = len * 2;
        }
        ret = MultiByteToWideChar( CP_ACP, 0, str, len, (LPWSTR)buf, outlen );
    } else {
        ret = 0;
    }
    return( ret * 2 );
}
#endif
