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


/*
 * DebuggingLog is a class intended to be used only while debugging
 * an application.  It would normally be used as a static member of a
 * class, and that class would write debugging information to the log

 * timing information is available throught the use of startTiming and
 * endTiming.  call startTiming, then endTiming returns the numbers of
 * seconds since startTiming (calls are stacked)
 */

#ifndef __DEBUG_LOG__
#define __DEBUG_LOG__

#include <wstd.h>
#include <stdio.h>

template <class Type> class              WCValSList;
template <class Type, class FType> class WCStack;

#if DEBUG

class DebuggingLog {
public:
            DebuggingLog( const char * name, bool append = false );
            ~DebuggingLog();

            enum LogException {
                OpenError,
                WriteError,
                CloseError
            };

                    // printf returns # chars written, as usual
            int     printf( const char * format, ... );
            void    write( const char * buffer, int len );
            void    puts( const char * buf );

            void    startTiming();
            double  endTiming();

                    // return the calendar date / time followed by '\n'
    static  char *  timeStamp();

private:
            FILE *                                      _fp;
            WCStack< uint_32, WCValSList< uint_32 > > * _times;
};

#else

class DebuggingLog {
public:
            DebuggingLog( const char *, bool= false ){};
            ~DebuggingLog(){};

            int     printf( const char *, ... ) { return 0; }
            void    write( const char *, int ) {}
            void    puts( const char * ) {}

            void    startTiming() {}
            double  endTiming() { return 0.0; }
    static  char *  timeStamp() { return NULL; }
};

#endif // DEBUG

#endif // __DEBUG_LOG__
