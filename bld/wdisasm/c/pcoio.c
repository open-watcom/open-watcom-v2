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


#include "disasm.h"
#include "wdismsg.h"

rec_type                RecType;
bool                    EndOfRecord;
unsigned                RecNumber;
uint_32                 Offset;
unsigned                RecLen;

static  char            CheckSum;
static  uint_32         ByteNo;
static  char            *Buff;      /* FIXME - should be HUGE */
static  char            *PrevBuff;
static  uint_32         begData;

/*
 * Static function prototypes
 */
extern  void            GetObjRec( void );
static  void            GetHeader( void );


void  InitObj()
/*************/

{
    RecNumber = 0;
    Buff = AllocMem( MAX_RECLEN );
    PrevBuff = AllocMem( MAX_RECLEN );
}


void MarkBegData( void )
/**********************/
{
    begData = ByteNo;
}


void  SkipPcoRec()
/****************/

{
    while( EndOfRecord == FALSE ) {
        GetByte();
    }
}


void  GetObjRec()
/***************/

{
    GetHeader();
    ++RecNumber;
    if( feof( ObjFile ) ) {
        Is32Record = 0;
        RecType = NULL;
    } else if( RecLen > MAX_RECLEN ) {
        Error( ERR_REC_TOO_LONG, TRUE );
    } else {
        FGetObj( Buff, RecLen );
        ByteNo = 0;
        if( RecLen - sizeof( char ) == 0 ) {
            EndOfRecord = TRUE;
        } else {
            EndOfRecord = FALSE;
        }
    }
}


static  void  GetHeader()
/***********************/

{
    char                header[ HEADER_LEN ];

    FGetObj( header, HEADER_LEN );
    RecType = header[ 0 ] & ~1;
    Is32Record = header[ 0 ] & 1;
    if( Is32Record ) {
        Is32BitObj = TRUE;
    }
    RecLen = header[ 2 ];
    RecLen = ( RecLen << 8 ) | header[ 1 ];
    CheckSum = RecType + header[ 1 ] + header[ 2 ];
}


void  ExchangeBuffers()
/*********************/

{
    char                *temp;

    temp = Buff;
    Buff = PrevBuff;
    PrevBuff = temp;
}


uint_32 Addr( uint_32 data_offset )
/*********************************/
{
    uint_32     address;
    uint_32     loc;

    loc = data_offset + begData;
    address = PrevBuff[ loc + 1 ];
    address = ( address << 8 ) | PrevBuff[ loc ];
    return( address );
}


uint_32 Addr32( uint_32 data_offset )
/************************************
 * This behaves like Addr does, but returns the 32-bit quantity at
 * data_offset
 */
{
    uint_32     address;

    address = Addr( data_offset + 2 );
    address = ( address << 16 ) + Addr( data_offset );

    return( address );
}


char  GetByte()
/*************/
{
    char                value;

    value = Buff[ ByteNo++ ];
    CheckSum += value;
    if( ByteNo == RecLen - sizeof( char ) ) {
        EndOfRecord = TRUE;
        CheckSum += Buff[ ByteNo ];
    }
    return( value );
}
