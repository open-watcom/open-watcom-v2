/***************************************************************************
*
*                            Open Watcom Project
*
*  Copyright (c) 2004-2007 The Open Watcom Contributors. All Rights Reserved.
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
*  Description: Implementation of the rtlver.lib library.
*
**************************************************************************/

#include <ntddk.h>

typedef NTSTATUS (NTAPI *PRTLGETVERSION)( PRTL_OSVERSIONINFOW );

BOOLEAN NTAPI WdmlibRtlIsNtDdiVersionAvailable( ULONG ulVersion )
/***************************************************************/
{
    UNICODE_STRING      RtlGetVersionName;
    PRTLGETVERSION      pRtlGetVersion;
    RTL_OSVERSIONINFOW  VersionInfo;
    ULONG               ulMajorVersion;
    ULONG               ulMinorVersion;
    
    RtlInitUnicodeString( &RtlGetVersionName, L"RtlGetVersion" );
    pRtlGetVersion = (PRTLGETVERSION)MmGetSystemRoutineAddress( &RtlGetVersionName );
    if( pRtlGetVersion != NULL ) {
        VersionInfo.dwOSVersionInfoSize = sizeof( RTL_OSVERSIONINFOW );
        pRtlGetVersion( &VersionInfo );
        ulMajorVersion = VersionInfo.dwMajorVersion;
        ulMinorVersion = VersionInfo.dwMinorVersion;
    } else {
        PsGetVersion( &ulMajorVersion, &ulMinorVersion, NULL, NULL );
    }
    return( ulVersion <= ((ulMajorVersion << 24) | (ulMinorVersion << 16)) );
}

BOOLEAN NTAPI WdmlibRtlIsServicePackVersionInstalled( ULONG ulVersion )
/*********************************************************************/
{
    UNICODE_STRING          RtlGetVersionName;
    PRTLGETVERSION          pRtlGetVersion;
    RTL_OSVERSIONINFOEXW    VersionInfo;
    ULONG                   ulMajorVersion;
    ULONG                   ulMinorVersion;
    USHORT                  usServicePack;
    
    RtlInitUnicodeString( &RtlGetVersionName, L"RtlGetVersion" );
    pRtlGetVersion = (PRTLGETVERSION)MmGetSystemRoutineAddress( &RtlGetVersionName );
    if( pRtlGetVersion != NULL ) {
        VersionInfo.dwOSVersionInfoSize = sizeof( RTL_OSVERSIONINFOEXW );
        pRtlGetVersion( (PRTL_OSVERSIONINFOW)&VersionInfo );
        ulMajorVersion = VersionInfo.dwMajorVersion;
        ulMinorVersion = VersionInfo.dwMinorVersion;
        usServicePack = VersionInfo.wServicePackMajor;
    } else {
        PsGetVersion( &ulMajorVersion, &ulMinorVersion, NULL, NULL );
        usServicePack = 0xFF;
    }
    return( ulVersion <= ((ulMajorVersion << 24) | (ulMinorVersion << 16) | 
        (usServicePack << 8)) );
}
