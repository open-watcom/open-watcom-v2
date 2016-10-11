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
* Description:  WLINK message output definitions.
*
****************************************************************************/


#include "banner.h"

enum msg_class {
    OUT_TERM    = 0x00100000,
    OUT_MAP     = 0x00200000,
    INF         = 0x00000000 + OUT_TERM,
    MAP         = 0x00000000 + OUT_MAP,
    YELL        = 0x00010000 + OUT_TERM,
    WRN         = 0x00020000 + OUT_TERM + OUT_MAP,
    MILD_ERR    = 0x00030000 + OUT_TERM + OUT_MAP,
    ERR         = 0x00040000 + OUT_TERM + OUT_MAP,
    FTL         = 0x00050000 + OUT_TERM + OUT_MAP,
    BANNER      = 0x00060000,
    LINE        = 0x00080000,
    LOC         = 0x00400000,
    LOC_REC     = 0x00800000 + LOC,
    CLASS_MSK   = 0x00070000,
    OUT_MSK     = 0x00300000,
    NUM_MSK     = 0x0000ffff,
    NUM_SHIFT   = 16
};

enum msg_num {
    #define pick( name, string ) name,
    #include "wlbanner.h"
    #undef pick
};

extern size_t   FmtStr( char *, size_t, const char *, ... );
extern size_t   DoFmtStr( char *, size_t, const char *, va_list * );
extern void     LnkMsg( unsigned, const char *, ... );
extern void     Locator( const char *, const char *, unsigned );
extern void     WLPrtBanner( void );
extern bool     SkipSymbol( symbol * );
extern int      SymAlphaCompare( const void *, const void * );
extern unsigned CalcMsgNum( unsigned );

#define MAX_MSG_SIZE 512

extern unsigned_32      MaxErrors;
extern byte             MsgFlags[];
extern bool             BannerPrinted;
extern const char       *MsgStrings[];
