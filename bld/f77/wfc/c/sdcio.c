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
* Description:  System dependent compiler I/O routines.
*
****************************************************************************/


#include "ftnstd.h"
#include "omodes.h"
#include "cpopt.h"
#include "global.h"
#include "fio.h"
#include "posio.h"

#include <string.h>

extern  file_handle             Openf(char *,int);
extern  int                     FGetRec(file_handle,char *,int);
extern  void                    FPutRec(file_handle,char *,int);
extern  void                    FSeekRec(file_handle,unsigned_32,int);
extern  void                    FRewind(file_handle);
extern  void                    Closef(file_handle);
extern  bool                    EOFile(file_handle);
extern  int                     Errorf(file_handle);
extern  char                    *ErrorMsg(file_handle);
extern  void                    Scratchf(char *);
extern  void                    CheckBlips(void);
extern  void                    InitStd(void);
extern  void                    SetIOBufferSize(uint);

extern  file_handle             FStdOut;

static  int             Modes[] = { RDONLY,
                                    WRONLY,
                                    APPEND,
                                    RDWR };

static  file_attr       CurrAttrs = { REC_TEXT };

file_attr       DskAttr = { REC_TEXT | CARRIAGE_CONTROL };
file_attr       PrtAttr = { REC_TEXT | CARRIAGE_CONTROL };
file_attr       TrmAttr = { REC_TEXT | CARRIAGE_CONTROL };
file_attr       ErrAttr = { REC_TEXT };
char            LstExtn[] = { "lst" };
char            ErrExtn[] = { "err" };
char            ForExtn[] = { "for" };
char            BrowseExtn[] = { "mbr" };
#if defined( __UNIX__ )
  char          ObjExtn[] = { "o" };
#else
  char          ObjExtn[] = { "obj" };
#endif


void    SDInitIO(void) {
//==================

    InitStd();
    SetIOBufferSize( 0 ); // minimum buffer size
}


void    SDInitAttr(void) {
//====================

    CurrAttrs = REC_TEXT;
}


void    SDSetAttr( file_attr attr ) {
//===================================

    CurrAttrs = attr;
}


void    SDScratch( char *name ) {
//===============================

    Scratchf( name );
}


file_handle     SDOpen( char *name, int mode ) {
//==============================================

    return( Openf( name, Modes[ mode ] | CurrAttrs ) );
}


void    SDClose( file_handle fp ) {
//=================================

    Closef( fp );
}


uint    SDRead( file_handle fp, byte *buff, uint len ) {
//======================================================

    return( FGetRec( fp, (char *)buff, len ) );
}


void    SDWrite( file_handle fp, byte *buff, int len ) {
//======================================================

    if( fp == FStdOut ) {
        CheckBlips();
    }
    FPutRec( fp, (char *)buff, len );
}


void    SDSeek( file_handle fp, int rec_num, int rec_size ) {
//===========================================================

    FSeekRec( fp, rec_num, rec_size );
}


void    SDRewind( file_handle fp ) {
//==================================

    FRewind( fp );
}


bool    SDEof( file_handle fp ) {
//===============================

    return( EOFile( fp ) );
}


bool    SDError( file_handle fp, char *buff ) {
//=============================================

    int         err;

    err = Errorf( fp );
    if( err != IO_OK ) {
        strcpy( buff, ErrorMsg( fp ) );
    }
    return( err != IO_OK );
}
