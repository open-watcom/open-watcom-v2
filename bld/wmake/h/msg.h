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
* Description:  WMAKE message output related defines.
*
****************************************************************************/


#include "banner.h"

#define YES_CHAR    'Y' /* must be upper case */

enum MsgClass {
    NUM_MSK     = 0x03ff,   /* these are valid msg numbers          */

    INF         = 0x0000,   /* an inform message - always printed   */
    WRN         = 0x0400,   /* Warning(Wnn): msg                    */
    ERR         = 0x0800,   /* Error(Enn): msg                      */
    FTL         = 0x0C00,   /* Error(Fnn): msg - aborts execution   */
    CLASS_MSK   = 0x0C00,


    PRNTSTR     = 0x1000,   /* print first arg as a string. used for*/
                                /* printing ANYTHING greater than 256   */
    LOC         = 0x2000,   /* print file and line info if possible */
    NEOL        = 0x4000,   /* suppress the automatic end of line   */

    DBG         = 0x8000,   /* only if Glob.debug                   */
};


enum {
    #define pick( name, string ) name,
    #include "_msg.h"
    #undef pick
};

extern unsigned FmtStr( char *buf, const char *fmt, ... );
extern void     PrtMsg( enum MsgClass num, ... );
extern void     Usage( void );
extern BOOLEAN  GetYes( enum MsgClass querymsg );
extern void     LogInit( const char *logname );
extern void     LogFini( void );
