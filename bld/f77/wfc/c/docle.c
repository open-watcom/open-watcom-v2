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


//
// DOCLE     : Compile
//

#include "ftnstd.h"
#include "global.h"
#include "cioconst.h"
#include "fcgbls.h"
#include "progsw.h"
#include "cpopt.h"
#include "errcod.h"
#include "comio.h"
#include "inout.h"
#include "cspawn.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "clibext.h"

extern  void            CompProg(void);
extern  void            MsgBuffer(uint,char *,...);
extern  uint            MakeName(char *,char *,char *);

extern  unsigned_32     CompTime;


void    DoCompile( void ) {
//===================

    CSpawn( &CompProg );
}


static  void    FmtU32( char *buff, unsigned_32 num, uint len ) {
//===============================================================

    int         offset;
    int         num_len;
    char        *num_ptr;
    char        num_buff[MAX_INT_SIZE+1];

    memset( buff, ' ', len );
    num_ptr = num_buff;
    ltoa( num, num_ptr, 10 );
    num_len = strlen( num_ptr );
    offset = len - num_len;
    if( offset < 0 ) {
        num_ptr -= offset;
        offset = 0;
    }
    strcpy( buff + offset, num_ptr );
}


#define COLUMN_WIDTH    39

void    StatProg( void ) {
//==================

    char        stats[_MAX_PATH + LIST_BUFF_SIZE];
    char        *ptr;
    uint        len;

    if( !(Options & OPT_QUIET) && !(Options & OPT_TYPE) ) {
        len = MakeName( SrcName, SrcExtn, stats );
        stats[len] = ':';
        ++len;
        MsgBuffer( MS_STATISTICS, &stats[len],
                   ISNNumber, CodeSize, NumExtens, NumWarns, NumErrors );
        TOutNL( stats );
    }
    LFSkip();

    MsgBuffer( MS_CODE_SIZE, stats );
    ptr = stats;
    if( stats[0] == ' ' ) {
        ++ptr; // don't include leading space
    }
    len = strlen( ptr );
    FmtU32( &ptr[len], CodeSize, COLUMN_WIDTH - len - 1 );
    ptr[COLUMN_WIDTH-1] = ' ';
    MsgBuffer( MS_NUMBER_OF_ERRORS, &ptr[COLUMN_WIDTH] );
    len = strlen( &ptr[COLUMN_WIDTH] );
    FmtInteger( &ptr[COLUMN_WIDTH+len], NumErrors, COLUMN_WIDTH - len );
    PrtLstNL( ptr );

    MsgBuffer( MS_COMPILE_TIME, stats );
    ptr = stats;
    if( stats[0] == ' ' ) {
        ++ptr; // don't include leading space
    }
    len = strlen( ptr );
    FmtU32( &ptr[len], CompTime, COLUMN_WIDTH - len - 1 );
    ptr[COLUMN_WIDTH-1] = ' ';
    MsgBuffer( MS_NUMBER_OF_WARNINGS, &ptr[COLUMN_WIDTH] );
    len = strlen( &ptr[COLUMN_WIDTH] );
    FmtInteger( &ptr[COLUMN_WIDTH+len], NumWarns, COLUMN_WIDTH - len );
    PrtLstNL( ptr );

    MsgBuffer( MS_NUMBER_OF_STATEMENTS, stats );
    ptr = stats;
    if( stats[0] == ' ' ) {
        ++ptr; // don't include leading space
    }
    len = strlen( ptr );
    FmtInteger( &ptr[len], ISNNumber, COLUMN_WIDTH - len - 1 );
    ptr[COLUMN_WIDTH-1] = ' ';
    MsgBuffer( MS_NUMBER_OF_EXTENSIONS, &ptr[COLUMN_WIDTH] );
    len = strlen( &ptr[COLUMN_WIDTH] );
    FmtInteger( &ptr[COLUMN_WIDTH+len], NumExtens, COLUMN_WIDTH - len );
    PrtLstNL( ptr );
    LFNewPage();       // force a form feed
    PrtLstNL( "" );    // ...
}
