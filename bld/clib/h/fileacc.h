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


#define _ValidFile( fp, retval )        /* make sure fp is a valid pointer */
#define SetupTGCSandNCS( x )            /* initialization for Netware/386 */

#if defined( __MT__ )
    #define _AccessFile( fp )       _AccessFileH( (fp)->_handle )
    #define _ReleaseFile( fp )      _ReleaseFileH( (fp)->_handle )
  #if defined( _M_I86 )
    // 16bit OS/2 multi-thread is different
    extern void __AccessFileH( int );
    extern void __ReleaseFileH( int );
    extern void __AccessIOB( void );
    extern void __ReleaseIOB( void );

    // define macros to call the access routines directly for OS/2 1.x
    #define _AccessFileH( hdl )     __AccessFileH( hdl )
    #define _ReleaseFileH( hdl )    __ReleaseFileH( hdl )
    #define _AccessIOB()            __AccessIOB()
    #define _ReleaseIOB()           __ReleaseIOB()
  #else
    #if !defined(_THIN_LIB)
        extern void (*_AccessFileH)( int );
        extern void (*_ReleaseFileH)( int );
        extern void (*_AccessIOB)( void );
        extern void (*_ReleaseIOB)( void );
    #else
        #define _AccessFileH( hdl )
        #define _ReleaseFileH( hdl )
        #define _AccessIOB()
        #define _ReleaseIOB()
    #endif
    #if defined(__NT__)
        extern void (*_AccessFList)( void );
        extern void (*_ReleaseFList)( void );
    #endif
  #endif
#else
    /* these are for multi thread support */
    /* they are not required if not building multi-thread library */
    /* note: 32 bit NETWARE, OS/2, NT, QNX libraries are always multi-thread */
    #define _AccessFile( fp )           /* gain access to the FILE* pointer */
    #define _ReleaseFile( fp )          /* release access */

    #define _AccessFileH( hdl )         /* gain access to the file handle */
    #define _ReleaseFileH( hdl )        /* release access */

    #define _AccessIOB()                /* gain access to array of __iob's */
    #define _ReleaseIOB()               /* release access */

  #if defined(__NT__)
    #define _AccessFList()
    #define _ReleaseFList()
  #endif
#endif
