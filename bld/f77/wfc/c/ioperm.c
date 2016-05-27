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
// IOPERM : routines to tell if a keyword is allowed in the control list
//

#include "ftnstd.h"
#include "errcod.h"
#include "iodefs.h"
#include "global.h"
#include "ferror.h"
#include "insert.h"
#include "ioperm.h"
#include "ioutls.h"


#define NO      0
#define YES     1

extern  char            *IOKeywords[];

// This table is used to determine if a control list item is permissible in
// a particular i/o statement.

#define TABLE_ENTRY     9

static  const byte __FAR        PermTable[] = {

//     READ WRT  OPEN CLS  BKSP ENDF REWD INQ  extension

       NO,  NO,  YES, NO,  NO,  NO,  NO,  YES, NO,  // "ACCESS"
       NO,  NO,  YES, NO,  NO,  NO,  NO,  YES, YES, // "ACTION"
       NO,  NO,  YES, NO,  NO,  NO,  NO,  YES, NO,  // "BLANK"
       NO,  NO,  YES, NO,  NO,  NO,  NO,  YES, YES, // "BLOCKSIZE"
       NO,  NO,  YES, NO,  NO,  NO,  NO,  YES, YES, // "CARRIAGECONTROL"
       NO,  NO,  NO,  NO,  NO,  NO,  NO,  YES, NO,  // "DIRECT"
       YES, NO,  NO,  NO,  NO,  NO,  NO,  NO,  NO,  // "END"
       YES, YES, YES, YES, YES, YES, YES, YES, NO,  // "ERR"
       NO,  NO,  NO,  NO,  NO,  NO,  NO,  YES, NO,  // "EXIST"
       NO,  NO,  YES, NO,  NO,  NO,  NO,  YES, NO,  // "FILE"
       YES, YES, NO,  NO,  NO,  NO,  NO,  NO,  NO,  // "FMT"
       NO,  NO,  YES, NO,  NO,  NO,  NO,  YES, NO,  // "FORM"
       NO,  NO,  NO,  NO,  NO,  NO,  NO,  YES, NO,  // "FORMATTED"
       YES, YES, YES, YES, YES, YES, YES, YES, NO,  // "IOSTAT"
       NO,  NO,  NO,  NO,  NO,  NO,  NO,  YES, NO,  // "NAME"
       NO,  NO,  NO,  NO,  NO,  NO,  NO,  YES, NO,  // "NAMED"
       NO,  NO,  NO,  NO,  NO,  NO,  NO,  YES, NO,  // "NEXTREC"
       NO,  NO,  NO,  NO,  NO,  NO,  NO,  YES, NO,  // "NUMBER"
       NO,  NO,  NO,  NO,  NO,  NO,  NO,  YES, NO,  // "OPENED"
       YES, YES, NO,  NO,  NO,  NO,  NO,  NO,  NO,  // "REC"
       NO,  NO,  YES, NO,  NO,  NO,  NO,  YES, NO,  // "RECL"
       NO,  NO,  YES, NO,  NO,  NO,  NO,  YES, YES, // "RECORDTYPE"
       NO,  NO,  NO,  NO,  NO,  NO,  NO,  YES, NO,  // "SEQUENTIAL"
       NO,  NO,  YES, NO,  NO,  NO,  NO,  YES, YES, // "SHARE"
       NO,  NO,  YES, YES, NO,  NO,  NO,  NO,  NO,  // "STATUS"
       NO,  NO,  NO,  NO,  NO,  NO,  NO,  YES, NO,  // "UNFORMATTED"
       YES, YES, YES, YES, YES, YES, YES, YES, NO   // "UNIT"
};


bool    Already( IOKW kw ) {
//==========================

    return( ( ( IOData >> ( kw - 1 ) ) & 1 ) != 0 );
}


static  byte    ExtnTest( IOKW kw ) {
//===================================

    return( PermTable[ TABLE_ENTRY * ( kw - 1 ) + 8 ] );
}


byte    PermTest( int kw ) {
//==========================

    return( PermTable[ TABLE_ENTRY * ( kw - 1 ) + IOIndex() ] );
}


void    KWRememb( IOKW kw ) {
//===========================

    unsigned_32 i;

    i = 1;
    i = i << ( kw - 1 );
    IOData |= i;
}


bool    Permission( IOKW kw ) {
//=============================

    bool        perm;

    perm = false;
    if( kw == 0 ) {
        OpndErr( IL_CTRL_LIST );
    } else if( Already( kw ) ) {
        OpndErr( IL_DUP_LIST );
    } else {
        perm = ( PermTest( kw ) != NO );
        if( perm ) {
            KWRememb( kw );
            if( ExtnTest( kw ) == YES ) {
                Extension( IL_SPECIFIER_NOT_STANDARD, IOKeywords[ kw ] );
            }
        } else {
            StmtPtrErr( IL_BAD_LIST, IOKeywords[ kw ] );
        }
    }
    return( perm );
}


void    CheckList( void ) {
//=========================

    bool        have_unit;

    have_unit = Already( IO_UNIT );
    if( StmtProc == PR_INQ ) {
        if( have_unit ) {
            if( Already( IO_FILE ) ) {
                Error( IL_UNIT_AND_FILE );
            }
        } else if( !Already( IO_FILE ) ) {
            Error( IL_NO_FILE_OR_UNIT );
        }
    } else if( !have_unit ) {
        Error( IL_NO_UNIT_ID );
    }
    if( Already( IO_INTERNAL ) && Already( IO_REC ) ) {
        Error( IL_AINTL );
    }
    if( Already( IO_END ) && Already( IO_REC ) ) {
        Extension( IL_END_REC );
    }
    if( Already( IO_LIST_DIR ) && Already( IO_INTERNAL ) ) {
        Extension( IL_ILST );
    }
    Remember.end_equals = Already( IO_END );
}
