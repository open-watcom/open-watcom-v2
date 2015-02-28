/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2004-2013 The Open Watcom Contributors. All Rights Reserved.
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
* Description: implement .pu script control word (output temporary workfile)
*                        and helper functions for pu open / close
*
*
*  comments are from script-tso.txt
****************************************************************************/

#include <errno.h>
#include "wgml.h"

static FILE * workfile[9] =           // support for 9 workfiles sysusr0n.gml
    { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };

/***************************************************************************/
/*  get workfile n file name                                               */
/***************************************************************************/

char    *get_pu_file_name( char *buf, size_t buf_size, int n )
{
    char    filename[] = "sysusr00" GML_EXT;

    filename[7] = '0' + n;
    strcpy( buf, filename );
    return( token_buf );
}

/***************************************************************************/
/*  close workfile n if open                                               */
/***************************************************************************/

void    close_pu_file( int n )
{
    if( n > 0 && n < 10 ) {
        if( workfile[n - 1] != NULL ) {
            fclose( workfile[n - 1] );
            workfile[n - 1] = NULL;
        }
    }
}


/***************************************************************************/
/*  close all open workfiles                                               */
/***************************************************************************/

void    close_all_pu_files( void )
{
    int k;

    for( k = 1; k < 10; k++ ) {
        close_pu_file( k );
    }
}


/***************************************************************************/
/*  open  workfile n if not yet done                                       */
/***************************************************************************/

static  int open_pu_file( int n )
{
    int         erc = 0;
    char        filename[13];

    if( n > 0 && n < 10 ) {
        if( workfile[n - 1] == NULL ) {   // not yet open
            get_pu_file_name( filename, sizeof( filename ), n );
            workfile[n - 1] = fopen( filename, "wt" );
            if( workfile[n - 1] == NULL ) {
                erc = errno;
            }
        }
    }
    return( erc );
}


/***************************************************************************/
/* PUT WORKFILE writes a line of  information (control or text)  into the  */
/* specified file.                                                         */
/*                                                                         */
/*      旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴커       */
/*      |       |                                                  |       */
/*      |  .PU  |    <1|n> <line>                                  |       */
/*      |       |                                                  |       */
/*      읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴켸       */
/*                                                                         */
/* This control word does not cause a break.  The first operand specifies  */
/* which workfile is to be used (from 1 to 9);  if not specified,  "1" is  */
/* assumed.  The first occurrence of .PU control word for the file causes  */
/* the file  to be opened.    If the "line"  operand is omitted  then the  */
/* output file will be closed.   An Imbed  (.IM)  or Append (.AP)  with a  */
/* numeric  filename  will  close  a workfile  created  with  .PU  before  */
/* processing the file as input.                                           */
/*                                                                         */
/* NOTES                                                                   */
/* (1) In the OS/VS batch environment, workfiles must be allocated with a  */
/*     DDname of "SYSUSR0n", where "n" ranges from 1 to 9.                 */
/* (2) In  CMS,  workfiles  are  allocated for  you  with  a "fileid"  of  */
/*     "SYSUSR0n SCRIPT",   but this  may  be  overridden with  your  own  */
/*     FILEDEF with  the PERM option before  invoking SCRIPT,  or  with a  */
/*     FILEDEF in a SYSTEM (.SY) control word within the SCRIPT file.      */
/* (3) The default file attributes are RECFM=VB, LRECL=136,  BLKSIZE=800.  */
/*     A fixed file may also be created,   in which case the defaults are  */
/*     RECFM=FB, LRECL=80, BLKSIZE=800.                                    */
/* (4) If  the file  is defined  with a  DISPosition of  MOD then  output  */
/*     records will be added to the end of the file.   If the file is not  */
/*     defined with a DISPosition of MOD then output records will replace  */
/*     the file.                                                           */
/*                                                                         */
/***************************************************************************/


/***************************************************************************/
/*  scr_pu    implement .pu control word                                   */
/***************************************************************************/

void    scr_pu( void )
{
    int             workn;
    condcode        cc;
    char        *   p;

    garginit();                         // find end of CW

    cc = getarg();                      // workfile number

    if( cc == omit ) {
        numb_err();                     // we need workfile number
        return;
    }

    p = tok_start;

    if( (arg_flen > 1) || (*p < '1') || (*p > '9') ) {
        numb_err();
        return;
    }
    workn = *p - '0';
    scan_restart = scan_stop;

    cc = getarg();                      // text follows

    if( cc == omit ) {                  // no then close workfile
        close_pu_file( workn );
        return;
    }

    open_pu_file( workn );              // open if not already done
    fputs( tok_start, workfile[workn - 1] );
    fputc( '\n', workfile[workn - 1] );

    return;
}
