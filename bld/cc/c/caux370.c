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


#include "cvars.h"
#include "cgswitch.h"
#include "pragdefn.h"
#include "pdefn2.h"

extern void CreateAux( char *id );
extern int OpenSrcFile( char *filename, int delimiter );
extern void PragEnding( void );
extern int PragRecog( char * what );
extern int NextToken(void);
extern void SetAuxName( void * filename );
extern void CErr1( int msgnum );
extern void CErr( int msgnum, ... );


local int GetConst( int token )
{
    if( token != T_CONSTANT ) CErr1( ERR_NOT_A_CONSTANT_EXPR );
    return( Constant );
}

void ParseAuxFile()
{
    int         offset;
    int         token;
    FCB        *prevfile;
    int         prevchar;

    if( AuxName == NULL ) return;
    prevfile = SrcFile;
    prevchar = CurrChar;
    SrcFile = NULL;
    if( OpenSrcFile( AuxName, 0 ) == FALSE ){
        SrcFile = prevfile; /* reset source */
        CurrChar = prevchar;
        ErrFName = SrcFile->src_name;
        return;
    }
    CMemFree( AuxName );
    offset = 0;
    while( (token = NextToken()) != T_EOF ){
        if( token == T_PERCENT ) { /* comment */
            for( ;; ) {
                token = NextToken();
                if( token == T_NULL ) break;
                if( token == T_EOF ) break;
            }
        } else if( token == T_ID ) {
            if( strcmp( Buffer, "include" ) == 0 ) {
                token = NextToken();
                if( token != T_EOF ){
                    SetAuxName( Buffer );
                    OpenSrcFile( AuxName, 0 );
                    CMemFree( AuxName );
                }else{
                    CErr( ERR_EXPECTING_BUT_FOUND, "a aux file name"
                         "end of file" );
                }
            } else if( strcmp( Buffer, "origin" ) == 0 ) {
                token = NextToken();
                offset = GetConst( token );
            } else if( strcmp( Buffer, "offset" ) == 0 ) {
                token = NextToken();
                if( token == T_ID ){
                    CreateAux( Buffer );
                    CurrAlias = &DefaultInfo;
                    *CurrInfo = *CurrAlias;
                    CurrEntry->offset = offset;
                    token = NextToken();
                    offset += GetConst( token );
                    PragEnding();
                }else{
                    CErr1( ERR_EXPECTING_ID );
                }
            } else {
                CErr( ERR_EXPECTING_BUT_FOUND, "include, offset or origin",
                       Buffer );
            }
        } else if( token != T_NULL  ) {
            CErr1( ERR_EXPECTING_ID );
        }
        if( token == T_EOF )break;
    }
    SrcFile = prevfile; /* reset source */
    CurrChar = prevchar;
    ErrFName = SrcFile->src_name;
}
