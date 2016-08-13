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


#include "jvmdip.h"

#define JAVA_SIG        "JAVA"

dip_status      DIGENTRY DIPImpLoadInfo( dig_fhandle dfh, imp_image_handle *ii )
{
    struct {
        char    sig[sizeof( JAVA_SIG ) - 1];
        ji_ptr  cc;
    }           jcf;
    unsigned    mb_size;
    dip_status  ds;

    if( DCCurrMAD() != MAD_JVM )
        return( DS_FAIL );
    DCSeek( dfh, 0, DIG_ORG );
    if( DCRead( dfh, &jcf, sizeof( jcf ) ) != sizeof( jcf ) ) return( DS_FAIL );
    if( memcmp( jcf.sig, JAVA_SIG, sizeof( jcf.sig ) ) != 0 ) return( DS_FAIL );
    DCClose( dfh );
    ii->cc = jcf.cc;
    if( GetU16( ii->cc + offsetof( ClassClass, major_version ) ) != JAVA_VERSION ) {
        return( DS_INFO_BAD_VERSION );
    }
    if( GetU16( ii->cc + offsetof( ClassClass, minor_version ) ) < JAVA_MINOR_VERSION ) {
        return( DS_INFO_BAD_VERSION );
    }
    ii->cp = GetPointer( ii->cc + offsetof( ClassClass, constantpool ) );
    ii->num_methods = GetU16( ii->cc + offsetof( ClassClass, methods_count ) );
    mb_size = ii->num_methods * sizeof( ii->methods[0] );
    ii->methods = DCAlloc( (mb_size != 0) ? mb_size : 1 );
    if( ii->methods == NULL ) {
        return( DS_NO_MEM );
    }
    ii->mb = GetPointer( ii->cc + offsetof( ClassClass, methods ) );
    if( mb_size != 0 ) {
        ds = GetData( ii->mb, ii->methods, mb_size );
        if( ds != DS_OK ) DCFree( ii->methods );
    }
    ii->last_method = 0;
    ii->object_class = 0;
    return( ds );
}

void            DIGENTRY DIPImpMapInfo( imp_image_handle *ii, void *d )
{
    DefCodeAddr = NilAddr;
    DefCodeAddr.mach.segment = MAP_FLAT_CODE_SELECTOR;
    DCMapAddr( &DefCodeAddr.mach, d );
    DefDataAddr = NilAddr;
    DefDataAddr.mach.segment = MAP_FLAT_DATA_SELECTOR;
    DCMapAddr( &DefDataAddr.mach, d );
}

void            DIGENTRY DIPImpUnloadInfo( imp_image_handle *ii )
{
    DCFree( ii->methods );
}
