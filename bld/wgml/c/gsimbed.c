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
* Description:  SCRIPT  .im  .ap   include file control words
*
****************************************************************************/

#define __STDC_WANT_LIB_EXT1__  1       /* use safer C library             */

#include "wgml.h"
#include "gvars.h"


/***************************************************************************/
/*  .im   processing  IMBED                                                */
/*         .im filename                                                    */
/*         .im n         -> sysusr0n.gml                                   */
/*                                                                         */
/*  For reference the description from script tso is included, but is only */
/*  partly relevant for the PC.                                            */
/*                                                                         */
/* !no options are supported                                               */
/*                                                                         */
/*                                                                         */
/* IMBED suspends  processing of the  current input file,   processes the  */
/* specified input file, and resumes processing of the suspended file.     */
/*                                                                         */
/*      旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴커       */
/*      |       |                                                  |       */
/*      |       |    Filename                < . <n1 <n2>>>        |       */
/*      |  .IM  |                     <args>                       |       */
/*      |       |    Filename(member)        < . <label>>          |       */
/*      |       |                                                  |       */
/*      읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴켸       */
/*                                                                         */
/* This control word does not cause a break.   The operands are identical  */
/* to those  for the APPEND  control word;   see the .AP  description for  */
/* details.   The  number of  levels to  which files  may be  imbedded is  */
/* limited by the  amount of storage available to  contain control blocks  */
/* and buffers:  one file may imbed another file or itself, and then that  */
/* file may imbed another file or itself, and then that file ... this can  */
/* continue until the  stack of imbedded files becomes so  large that all  */
/* available storage contains  imbedded file control blocks  and an error  */
/* results.                                                                */
/*                                                                         */
/* NOTES                                                                   */
/* (1) The word  "imbed" does  not exist in  the English  language.   Its  */
/*     usage here merely  reflects the ".IM" control  word.   The correct  */
/*     word for the function is "embed" but ".EM" was taken.               */
/*                                                                         */
/*                                                                         */
/*                                                                         */
/*                                                                         */
/* APPEND  terminates processing  of the  current input  file and  starts  */
/* processing of the specified input file.                                 */
/*                                                                         */
/*      旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴커       */
/*      |       |                                                  |       */
/*      |       |    Filename                < . <n1 <n2>>>        |       */
/*      |  .AP  |                     <args>                       |       */
/*      |       |    Filename(member)        < . <label>>          |       */
/*      |       |                                                  |       */
/*      읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴켸       */
/*                                                                         */
/*                                                                         */
/* Filename<(member)>:  If no operands other than the filename are speci-  */
/*    fied, the new file will be processed starting at the first line.     */
/* . <n1 <n2>|label>:  The file will be processed from lines "n1" to "n2"  */
/*    or starting at label "...label".   The "." must be the control-word  */
/*    indicator (normally a period) that is currently in effect.           */
/* args:  Any operands specified ("args")   after the filename and before  */
/*    the "."  are passed to the file being appended in the same way that  */
/*    operands are passed to a macro when it is invoked.                   */
/*                                                                         */
/* This control word  does not create a  break.   An "*" may  be coded in  */
/* place of "n1 n2" to indicate that  the entire file is to be processed.  */
/* If no  other operands appear after  the filename,  the entire  file is  */
/* processed.   If "n1" is  coded,  "*" may be coded in  place of "n2" to  */
/* indicate that the entire remainder of the file is to be processed;  if  */
/* neither "n2" or "*" is specified, "*" is assumed.                       */
/*                                                                         */
/* NOTES                                                                   */
/* (1) If .AP FILENAME is used to supply the filename, a five-step proce-  */
/*     dure is followed in searching for the referenced file.   In order,  */
/*     a search is conducted for:                                          */
/*     -1- A  DD  statement  "FILENAME"  which  points  to  a  sequential  */
/*         dataset.                                                        */
/*     -2- A  DD  statement  "FILENAME" which  points  to  a  partitioned  */
/*         dataset, which itself contains a member "FILENAME'.             */
/*     -3- A member "FILENAME"  in an active partitioned  dataset.   (The  */
/*         active partitioned datasets are searched for member "FILENAME"  */
/*         in reverse order of opening.)                                   */
/*     -4- A  member  "FILENAME"  in the  optional  "SYSLIB"  partitioned  */
/*         dataset.                                                        */
/*     -5- A  member "FILENAME"  in the  optional "SCRIPLIB"  partitioned  */
/*         dataset.                                                        */
/* If nothing is  found,  a message is  printed out and an  empty file is  */
/* APpended.                                                               */
/* (2) If .AP FILENAME(MEMBERNAME) is used to supply the file name,  step  */
/*     -3- of the search is skipped, and in steps -2- and -4-, the parti-  */
/*     tioned  datasets  in question  are  searched  for a  member  named  */
/*     "MEMBERNAME" not "FILENAME".                                        */
/*                                                                         */
/* EXAMPLES                                                                */
/* (1) .AP CHA4CONT                                                        */
/*     The file  named CHA4CONT  SCRIPT will  be read  and formatted  for  */
/*     output as a continuation of the  current SCRIPT file.   &*0 is set  */
/*     to zero.   &* is set to the  null string.   If the output is being  */
/*     created in  OS,  a DD statement  similar to one of  the statements  */
/*     below would have to be included in the job setup for input of this  */
/*     file.  If CHA4CONT were a sequential file:                          */
/*       //CHA4CONT DD DSNAME=xxxxxx,DISP=SHR                              */
/*     If CHA4CONT were a  member of a library it could  be referenced by  */
/*     including a "SYSLIB DD" statement.                                  */
/*       //SYSLIB   DD DSNAME=xxxxxx,DISP=SHR                              */
/*     If CHA4CONT were a member of  a particular partitioned dataset the  */
/*     following "DD statement" would be necessary:                        */
/*       //CHA4CONT DD DSNAME=xxxxxx(CHA4CONT),DISP=SHR                    */
/* (2) .AP FIG2 one 'one plus one' . 12 *                                  */
/*     The  file named  FIG2 will  be read  starting with  record 12  and  */
/*     output as a continuation of the current input file.   The value of  */
/*     set symbol &*0 will be set to "2",  with &*1 and &*2 taking values  */
/*     "one" and "one plus one" respectively.   The value of &* is set to  */
/*     "one 'one plus one'".                                               */
/* (3) .ap TEST type=CHAPTER TITLE='New World' err(5)=4*4                  */
/*     The file named TEST will be read with keywords TYPE, TITLE and ERR  */
/*     set.  &*0 will be set to zero.                                      */
/*                                                                         */
/*                                                                         */
/*                                                                         */
/***************************************************************************/

void    scr_im( void )
{
    char        *   fnstart;
    char        *   p;
    char            quote;
    condcode        cc;
    getnum_block    gn;

    p = scan_start;
    while( *p == ' ' ) {
        p++;
    }

    gn.argstart = p;
    gn.argstop = scan_stop;
    gn.ignore_blanks = 0;

    cc = getnum( &gn );

    if( (cc == pos) && (gn.result < 10) ) { // include sysusr0n.gml

        close_pu_file( gn.result );     // if still open
        get_pu_file_name( token_buf, buf_size, gn.result );

    } else {
        p = gn.argstart;

        if( *p == '"' || *p == '\'' ) {
            quote = *p;
            ++p;
        } else {
            quote = ' ';                // error??
        }
        fnstart = p;
        while( *p && *p != quote ) {
            ++p;
        }
        *p = '\0';
        strcpy_s( token_buf, buf_size, fnstart );
    }

    scan_restart = scan_stop;
    ProcFlags.newLevelFile = 1;
    line_from = LINEFROM_DEFAULT;
    line_to   = LINETO_DEFAULT;

    return;
}


/***************************************************************************/
/*  .ap processing APPEND                                                  */
/*  format  .ap filename                                                   */
/***************************************************************************/

extern  void    scr_ap( void )
{
    input_cbs->s.f->flags |= FF_eof;    // simulate EOF for .append
    input_cbs->fmflags   |= II_eof;     // simulate EOF for .append
    scr_im();                           // do .imbed processing
}

