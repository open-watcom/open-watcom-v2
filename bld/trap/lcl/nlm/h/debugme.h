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


#ifdef DEBUG_ME

    typedef enum {
        D_THREAD        = 1,
        D_DR            = 2,
        D_EVENT         = 4,
        D_MISC          = 8,
        D_IO            = 16,
        D_IPX           = 32,
        D_NET           = 32,
        D_REQ           = 64,
        D_ERROR  = 128
    } debug_classes;

    extern debug_classes        DebugClasses;

    #ifndef static
        #define static
    #endif

    #define _DBG_THREAD( x ) if( DebugClasses & D_THREAD ) { ConsolePrintf x; }
    #define _DBG_DR( x ) if( DebugClasses & D_DR ) { ConsolePrintf x; }
    #define _DBG_EVENT( x ) if( DebugClasses & D_EVENT ) { ConsolePrintf x; }
    #define _DBG_IO( x ) if( DebugClasses & D_IO ) { ConsolePrintf x; }
    #define _DBG_MISC( x ) if( DebugClasses & D_MISC ) { ConsolePrintf x; }
    #define _DBG_IPX( x ) if( DebugClasses & D_IPX ) { ConsolePrintf x; }
    #define _DBG_NET( x ) if( DebugClasses & D_NET ) { ConsolePrintf x; }
    #define _DBG_REQ( x ) if( DebugClasses & D_REQ ) { ConsolePrintf x; }
    #define _DBG_ERROR( x ) if( DebugClasses & D_ERROR ) { ConsolePrintf x; }

    extern void ConsolePrintf(char *,...);

#else

    #define _DBG_THREAD( x )
    #define _DBG_DR( x )
    #define _DBG_EVENT( x )
    #define _DBG_IO( x )
    #define _DBG_MISC( x )
    #define _DBG_IPX( x )
    #define _DBG_NET( x )
    #define _DBG_REQ( x )
    #define _DBG_ERROR( x )

#endif
