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
* Description:  Internal debugger trap file OS specific configuration.
*
****************************************************************************/


#ifndef TRPCONF_H
#define TRPCONF_H

#if defined(__DOS__) || defined(__DSX__)
    #if defined(DOSXHELP)
        /* protected mode helper */
        #undef          WANT_FILE_INFO
        #undef          WANT_ENV
        #undef          WANT_ASYNC
        #undef          WANT_FILE
        #undef          WANT_OVL
        #undef          WANT_THREAD
        #undef          WANT_RUN_THREAD
        #undef          WANT_RFX
    #elif defined(DOSXTRAP)
        /* real mode trap file talking to protected mode helper */
        #undef          WANT_FILE_INFO
        #undef          WANT_ENV
        #undef          WANT_ASYNC
        #define         WANT_FILE
        #undef          WANT_OVL
        #undef          WANT_THREAD
        #undef          WANT_RUN_THREAD
        #define         WANT_RFX
    #else
        /* straight dos */
        #undef          WANT_FILE_INFO
        #undef          WANT_ENV
        #undef          WANT_ASYNC
        #define         WANT_FILE
        #define         WANT_OVL
        #undef          WANT_THREAD
        #undef          WANT_RUN_THREAD
        #define         WANT_RFX
    #endif
#elif defined(__OS2__) && defined(__I86__)
    #undef          WANT_FILE_INFO
    #undef          WANT_ENV
    #undef          WANT_ASYNC
    #define         WANT_FILE
    #undef          WANT_OVL
    #define         WANT_THREAD
    #undef          WANT_RUN_THREAD
    #define         WANT_RFX
#elif defined(__OS2V2__) || defined(__OS2__) && !defined(__I86__)
    #if defined(ELFCORE)
        #undef      WANT_FILE_INFO
        #undef      WANT_ENV
        #undef      WANT_ASYNC
        #define     WANT_FILE
        #undef      WANT_OVL
        #undef      WANT_THREAD
        #undef      WANT_RUN_THREAD
        #undef      WANT_RFX
    #else
        #undef      WANT_FILE_INFO
        #undef      WANT_ENV
        #undef      WANT_ASYNC
        #define     WANT_FILE
        #undef      WANT_OVL
        #define     WANT_THREAD
        #undef      WANT_RUN_THREAD
        #define     WANT_RFX
    #endif
#elif defined(__NT__)
    #if defined(JVMXHELP) || defined(MSJXHELP)
        #undef  WANT_FILE_INFO
        #undef  WANT_ENV
        #undef  WANT_ASYNC
        #define WANT_FILE
        #undef  WANT_OVL
        #define WANT_THREAD
        #undef  WANT_RUN_THREAD
        #undef  WANT_RFX
    #elif defined(MSJXTRAP)
        #define WANT_FILE_INFO
        #define WANT_ENV
        #undef  WANT_ASYNC
        #define WANT_FILE
        #undef  WANT_OVL
        #define WANT_THREAD
        #undef  WANT_RUN_THREAD
        #undef  WANT_RFX
    #elif defined(ELFCORE)
        #undef  WANT_FILE_INFO
        #undef  WANT_ENV
        #undef  WANT_ASYNC
        #define WANT_FILE
        #undef  WANT_OVL
        #undef  WANT_THREAD
        #undef  WANT_RUN_THREAD
        #undef  WANT_RFX
    #else
        #define WANT_FILE_INFO
        #define WANT_ENV
        #undef  WANT_ASYNC
        #define WANT_FILE
        #undef  WANT_OVL
        #define WANT_THREAD
        #undef  WANT_RUN_THREAD
        #undef  WANT_RFX
        #define WANT_CAPABILITIES
    #endif
#elif defined(__WINDOWS__)
    #undef      WANT_FILE_INFO
    #undef      WANT_ENV
    #undef      WANT_ASYNC
    #define     WANT_FILE
    #undef      WANT_OVL
    #undef      WANT_THREAD
    #undef      WANT_RUN_THREAD
    #define     WANT_RFX
#elif defined(__QNX__)
    #undef      WANT_FILE_INFO
    #undef      WANT_ENV
    #undef      WANT_ASYNC
    #define     WANT_FILE
    #undef      WANT_OVL
    #define     WANT_THREAD
    #undef      WANT_RUN_THREAD
    #undef      WANT_RFX
#elif defined(__LINUX__)
    #undef      WANT_FILE_INFO  // TODO: Want this later for Linux!
    #undef      WANT_ENV        // TODO: Want this later for Linux!
    #undef      WANT_ASYNC      // TODO: Want this later for Linux!
    #define     WANT_FILE
    #undef      WANT_OVL
    #undef      WANT_THREAD     // TODO: Want this later for Linux!
    #undef      WANT_RUN_THREAD
    #undef      WANT_RFX        // TODO: Want this later for Linux!
#elif defined(__UNIX__)
    #undef      WANT_FILE_INFO
    #undef      WANT_ENV
    #undef      WANT_ASYNC
    #define     WANT_FILE
    #undef      WANT_OVL
    #undef      WANT_THREAD
    #undef      WANT_RUN_THREAD
    #undef      WANT_RFX
#elif defined(__NETWARE__)
    #undef      WANT_FILE_INFO
    #undef      WANT_ENV
    #undef      WANT_ASYNC
    #define     WANT_FILE
    #undef      WANT_OVL
    #define     WANT_THREAD
    #undef      WANT_RUN_THREAD
    #undef      WANT_RFX
#elif defined(__RDOS__)
    #define     WANT_FILE_INFO
    #define     WANT_ENV
    #define     WANT_ASYNC
    #define     WANT_FILE
    #undef      WANT_OVL
    #undef      WANT_THREAD
    #define     WANT_RUN_THREAD
    #undef      WANT_RFX
    #define     WANT_CAPABILITIES
#else
    #error Unknown operating system
#endif

#endif
