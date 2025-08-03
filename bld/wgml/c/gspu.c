/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2004-2025 The Open Watcom Contributors. All Rights Reserved.
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


#include "wgml.h"


static FILE * workfile[9] =           // support for 9 workfiles SYSUSR0x.GML
    { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };

/***************************************************************************/
/*  get workfile n name                                                    */
/***************************************************************************/

char *get_workfile_name( int n )
{
    static char     workfile_name[20] = "sysusr0x.gml";

    workfile_name[7] = '0' + n;
    return( workfile_name );
}

/***************************************************************************/
/*  close workfile n if open                                               */
/***************************************************************************/

void    close_pu_file( int n )
{
    FILE    *fp;

    fp = workfile[n - 1];
    if( fp != NULL ) {
        fclose( fp );
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

static FILE     *open_pu_file( int n )
{
    FILE            *fp;

    fp = workfile[n - 1];
    if( fp == NULL ) {   // not yet open
        fp = fopen( get_workfile_name( n ), "wt" );
        workfile[n - 1] = fp;
    }
    return( fp );
}


/***************************************************************************/
/* PUT WORKFILE writes a line of  information (control or text)  into the  */
/* specified file.                                                         */
/*                                                                         */
/*      +-------+--------------------------------------------------+       */
/*      |       |                                                  |       */
/*      |  .PU  |    <1|n> <line>                                  |       */
/*      |       |                                                  |       */
/*      +-------+--------------------------------------------------+       */
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
    char            *p;
    char            *pa;
    condcode        cc;                 // result code
    getnum_block    gn;
    int             len;
    int             fno;
    FILE            *fp;

    p = g_scandata.s;
    SkipSpaces( p );                    // next word start
    pa = p;
    SkipNonSpaces( p );                 // end of word
    len = p - pa;

    if( len == 0 ) {                    // omitted
        fno = 1;                        // "1" is default
    } else {

        gn.arg.s = pa;
        gn.arg.e = p;
        gn.ignore_blanks = false;
        cc = getnum( &gn );

        if( cc != CC_notnum ) {            // number found
            if( (len > 1) || (cc != CC_pos)  || (*pa < '1') || (*pa > '9') ) {
                numb_err_exit();
                /* never return */
            }
            fno = *pa - '0';            // workfile specified
            pa++;
            SkipSpaces( pa );           // next word start
        } else {
            fno = 1;                    // workfile not given, "1" is default
        }
    }
    scan_restart = g_scandata.e;           // do here because returns below all need it

    if( *pa == '\0' ) {                 // no text follows
        close_pu_file( fno );
        return;
    }
    fp = open_pu_file( fno );           // get or open if not already done
    if( fp != NULL ) {
        fputs( pa, fp );
        fputc( '\n', fp );
    }
}

