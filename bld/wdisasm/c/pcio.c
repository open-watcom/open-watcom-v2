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


#include <string.h>
#include "disasm.h"


int  FGetObj( char *buff, int len )
/*********************************/
{
    return( fread( buff, 1, len, ObjFile ) );
}


FILE  *OpenBinRead( char *obj_name )
/**********************************/
{
    return( fopen( obj_name, "rb" ) );
}


void  CloseBin( FILE *bin_file )
/******************************/
{
    fclose( bin_file );
}


FILE  *OpenTxtRead( char *txt_name )
/**********************************/
{
    return( fopen( txt_name, "rt" ) );
}


FILE  *OpenTxtWrite( char *txt_name )
/***********************************/
{
    /* we open "wb" here for speed, since we only print newlines when we
     * do a FPutTxtRec() -- NOT ANY MORE!!
     */
    return( fopen( txt_name, "w" ) );
}


void  CloseTxt( FILE *txt_file )
/******************************/
{
    fclose( txt_file );
}


void  FPutTxtRec( FILE *txt_file, char *buff, int len )
/*****************************************************/
{
    fwrite( buff, len, 1, txt_file );
#if 0
#if !defined( __QNX__ )
    if( txt_file != stdout && txt_file != stderr ) {
        fputc( '\r', txt_file );        /* see OpenTxtWrite for reason */
    }
#endif
#endif
    fputc( '\n', txt_file );
}


int  FGetTxtRec( FILE *txt_file, char *buff, int len )
/****************************************************/
{
    int                 read_len;

    if( fgets( buff, len, txt_file ) != NULL ) {
        read_len = strlen( buff ) - 1;
    } else {
        read_len = 0;
    }
    buff[ read_len ] = NULLCHAR;
    return( read_len );
}
