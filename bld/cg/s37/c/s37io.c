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


#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include "cg.h"
#include "cgaux.h"
#include "targsys.h"
#include "model.h"
#include "zoiks.h"
/*
 *
 * START KLUDGE TO GET QNX WORKING FOR NOW.
 *
 */

#if _HOSTOS == _QNX
    #define O_RDWR      000002      /*  Read-Write mode */
    #define O_CREAT     000400      /*  Opens with file create              */
    #define O_TRUNC     001000      /*  Open with truncation                */
    #define O_BINARY    000000  /* binary file */
    #define S_IRUSR     000400  /*      Read permission, owner              */
    #define S_IWUSR     000200  /*      Write permission, owner             */
    #define S_IRGRP     000040  /*      Read permission, group              */
    #define S_IWGRP     000020  /*      Write permission, group             */
    #define S_IROTH     000004  /*      Read permission, other              */
    #define S_IWOTH     000002  /*      Write permission, other             */

    #define PMODE       S_IRUSR+S_IWUSR+S_IRGRP+S_IWGRP+S_IROTH+S_IWOTH

    int  open(char *__path,int __oflag,...);
#elif _HOSTOS == _CMS

    #define O_RDONLY        0x00    /* read only */
    #define O_WRONLY        0x01    /* write only */
    #define O_RDWR          0x02    /* read and write */
    #define O_BINARY        0x10    /* file contains - binary data */
    #define O_TEXT          0x20    /* - text (\n terminated records) */
    #define O_CREAT         0x200   /* replace, or create t */
    #define O_TRUNC         0x400   /* open with truncation */
#else
    #define O_WRONLY        0x0001  /* open for write only */
    #define O_RDONLY        0x0000  /* open for read only */
    #define O_RDWR              0x0002  /* open for read and write */
    #define O_CREAT     0x0020  /* create new file */
    #define O_TRUNC     0x0040  /* truncate existing file */
    #define O_TEXT          0x0100  /* text file */
    #define O_BINARY    0x0200  /* binary file */

    #define PMODE       0
    int  open(char *__path,int __oflag,...);
#endif

/*
 *
 * END KLUDGE TO GET QNX WORKING FOR NOW
 *
 */

extern  char    *       CopyStr(char*,char*);
int  close(int __handle);
int  creat(char *__path,int __pmode);
long lseek(int __handle,long __offset,int __origin);
int  read(int __handle,void *__buf,unsigned int __len);
int  write(int __handle,void *__buf,unsigned int __len);
char *strerror( int __errnum );
int   remove(const char *__filename);
typedef int handle;
enum stdfd { HStdIn=0, HStdOut =1, HStdErr = 2 };

extern  void            FatalError(char *);
extern  pointer         FEAuxInfo(pointer*,aux_class);
extern    handle        ObjFile;
static  char            ObjName[128];
#define DUMP_SIZE       80
static  char            DumpBuff[DUMP_SIZE];
static  int             DumpUsed = { 0 };

extern  bool    CGOpenf() {
/*************************/

    CopyStr( FEAuxInfo( NULL, OBJECT_FILE_NAME ), ObjName );
    if( (int)FEAuxInfo( NULL, REVISION_NUMBER ) != II_REVISION ) {
        FatalError( "Incompatible code generator" );
    }
    ObjFile = OpenObj( ObjName );
    if( ObjFile == 0 ) return( FALSE );
    return( TRUE );
}

extern  int  OpenObj( char *name ) {
/*************************/
    int         retc;
#if _HOSTOS & _CMS
    char        filename[ 256 ];

    strcpy( filename, name );
    strcat( filename, "(lrecl 80 recfm f" );
    name = filename;
    errno = 0;
    retc = open( name, O_CREAT+O_TRUNC+O_WRONLY+O_BINARY );
#else
    errno = 0;
    retc = open( name, O_CREAT+O_TRUNC+O_WRONLY+O_BINARY, PMODE );
#endif
    if( retc == -1 ) {
        ObjError( errno );
    }
    return( retc );

}

extern void    PutObjRec( int fd, void *buff, uint len ) {
/*************************************************************/
//  char out[200];
//  ToHex( buff, out, len );
//  out[len*2] = '\r';
//  out[len*2+1] = '\v';
    PutStream( fd, buff, len );
}


extern  void    ScratchObj() {
/****************************/

    if( ObjFile != 0 ) {
        CloseStream( ObjFile );
        EraseStream( ObjName );
        ObjFile = 0;
    }
}


extern  void    CloseObj() {
/**************************/


    if( ObjFile != 0 ) {
        CloseStream( ObjFile );
        ObjFile = 0;
    }
}

extern  handle  OpenDbg( char *name ) {
/*************************/
    int         retc;
#if _HOSTOS & _CMS
    char        filename[ 256 ];

    strcpy( filename, name );
    strcat( filename, "(lrecl 80 recfm v" );
    name = filename;
    errno = 0;
    retc = open( name, O_CREAT+O_TRUNC+O_WRONLY+O_BINARY );
#else
    errno = 0;
    retc = open( name, O_CREAT+O_TRUNC+O_WRONLY+O_BINARY, PMODE );
#endif
    if( retc == -1 ) {
        ObjError( errno );
    }
    return( retc );

}

typedef struct asm_fi {
    handle fi;
    int    size;
    bool   lst;
} asm_fi;

extern  asm_fi *AsmOpen( char *name, int size, bool lst  ) {
/*************************/
    int         fi;
    asm_fi     *asm;
#if _HOSTOS & _CMS
    char        filename[ 256 ];
    char        num[20];
    strcpy( filename, name );
    itoa( size, num, 10 );
    strcat( filename, "(lrecl " );
    strcat( filename, num );
    strcat( filename, " recfm f" );
    name = filename;
    errno = 0;
    fi = open( name, O_CREAT+O_TRUNC+O_WRONLY+O_TEXT );
#else
    size = 0;
    errno = 0;
    fi = open( name, O_CREAT+O_TRUNC+O_WRONLY+O_TEXT, PMODE );
#endif
    if( fi == -1 ) {
        ObjError( errno );
    }
    asm = malloc( sizeof( *asm ) );
    asm->fi = fi;
    asm->size = size;
    asm->lst = lst;
    return( asm );

}

void PutLine( asm_fi *asm, char *buff, int len ){
/*** Write out a line of output***************/
    char *trim;
#if _HOSTOS & _CMS  /* not quite right */
    PutStream( asm->fi, buff, len );
#else  /* assumes buff is long enough to hold \n  */
    trim = &buff[len-1];
    while( len > 0  ){
        if( *trim != ' ' )break;
        --trim;
        --len;
    }
    trim[1] = '\n';
    PutStream( asm->fi, buff, len+1 );
#endif
}

extern void AsmClose( asm_fi *src  ){
    close( src->fi );
    free( src );
}

typedef struct src_fi {
    handle fi;
    int    curr_line;
    char  *curr;
    char   buff[80+1];
} src_fi;


extern  src_fi *CGSrcOpen( src_fi *src, char *name ) {
/*************************/
    int         retc;
#if _HOSTOS & _CMS
    errno = 0;
    retc = open( name, O_RDONLY+O_TEXT );
#else
    errno = 0;
    retc = open( name, O_RDONLY+O_TEXT, PMODE );
#endif
    if( retc == -1 ) {
        ObjError( errno );
    }
    src->fi = retc;
    src->buff[0] = '\n';
    src->buff[1] = '\0';
    src->curr = src->buff;
    src->curr_line = 0;
    return( src );

}

extern  int  CGSrcGet( src_fi *src, char *buff, int line, int len  ) {
/*************************/
    int curr_line;
    char *curr;
    char *endbuff;

    curr_line = src->curr_line;
    if( line < curr_line ){
        return( FALSE );
    }
    curr = src->curr;
    while( line > curr_line ){
        for(;;){
            if( *curr == '\n' )break;
            if( *curr == '\0' )break;
            ++curr;
        }
//      while( *curr != '\n' && *curr != '\0' ) ++curr;
        if( *curr == '\0' ){
            if( FillBuff( src ) == 0 ) {
                return( FALSE );
            }
            curr =  src->buff;
        }else{
           curr_line++;
           curr++;
       }
    }
    endbuff = &buff[len];
    while( buff < endbuff ){
        if( *curr == '\0' ){
            if( FillBuff( src ) == 0 ) {
                return( FALSE );
            }
            curr =  src->buff;
        }
        if( *curr == '\n' ){
            curr++;
            curr_line++;
            break;
        }
        *buff++ = *curr++;
    }
    src->curr_line = curr_line;
    src->curr = curr;
    return( len-(endbuff-buff) );
}

static int FillBuff( src_fi *src ) {
    int retlen;

    errno = 0;
    retlen = read( src->fi, src->buff, sizeof( src->buff )-1 );
    if( retlen == -1 ){
        ObjError( errno );
    }
    src->buff[retlen] = '\0';
    src->curr = src->buff;
    return( retlen );
}

extern void CGSrcClose( src_fi *src  ){
    close( src->fi );
}

extern void    CloseStream( handle h ) {
/***************************************/

    close( h );
}

static  void    EraseStream( char *name ) {
/*****************************************/

    remove( name );
}

extern  void    PutStream( handle h, byte *b, uint len ) {
/*******************************************************/

    int         retc;

    errno = 0;
    retc = write( h, b, len );
    if( errno != 0 ) {
        ObjError( errno );
    }
    if( (unsigned_16)retc != len ) {
        FatalError( "Error writing file - disk is full" );
    }
}


extern  void    ObjError( int    errcode ) {
/***************************************/

    FatalError( strerror(  errcode ) );
}

extern  void    DumpChar( char c ) {
/**********************************/

    if( DumpUsed == DUMP_SIZE ) {
        write( HStdOut, DumpBuff, DUMP_SIZE );
        DumpUsed = 0;
    }
    DumpBuff[ DumpUsed++ ] = c;
}

extern  void    DumpNL() {
/************************/

    DumpLine( DumpBuff, DumpUsed );
    DumpUsed = 0;
}


extern  void    DumpLine( char *s, unsigned len ) {
/*************************************************/

    char        c;

    c = s[len];
    s[len] = '\n';
    write( HStdOut, s, len + 1 );
    s[len] = c;
}

