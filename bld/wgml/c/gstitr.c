/****************************************************************************
*
*                            Open Watcom Project
*
*  Copyright (c) 2004-2009 The Open Watcom Contributors. All Rights Reserved.
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
* Description: implement .ti (translate input)  script control word
*                    and .tr (translate output) script control word
*
*  comments are from script-tso.txt
****************************************************************************/

#define __STDC_WANT_LIB_EXT1__  1      /* use safer C library              */

#include "wgml.h"
#include "gvars.h"


/**************************************************************************/
/* TRANSLATE ON INPUT allows the user  to specify an escape character and */
/* a translate table to be used on input lines.                           */
/*                                                                        */
/*      ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿      */
/*      |       |                                                  |      */
/*      |       |    <SET <char>>                                  |      */
/*      |  .TI  |    <s <s|t>>                                     |      */
/*      |       |    <<s1 t1> <s2 t2> ...>                         |      */
/*      |       |                                                  |      */
/*      ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ      */
/*                                                                        */
/* This control word does not cause a break.                              */
/*                                                                        */
/* SET <char>:  In all subsequent input text and control lines, the char- */
/*    acter immediately  following the  escape character  "char" will  be */
/*    translated  according  to  the .TI  translate  table  currently  in */
/*    effect,  and the escape character removed.   If .TI is used without */
/*    any operands, the translate table specified by the TRANSLATE option */
/*    when  SCRIPT was  invoked will  be reinstated  and any  previously- */
/*    defined escape character  will be nullified.   If  SET is specified */
/*    without a  "char" operand,   the current  escape character  will be */
/*    nullified but the  translate table will remain.   When  there is no */
/*    escape character in effect, no input translations are performed.    */
/* <s1 t1> <s2  t2> ...:  specifies "source" characters  to be translated */
/*    to "target" characters.                                             */
/* s <s|t>:  This form  of the control word allows a  single source char- */
/*    acter to  be translated  into itself  if no  "target" character  is */
/*    specified.                                                          */
/*                                                                        */
/* Many of  the commoner uses  of .TI  can be more  conveniently achieved */
/* through the use of  the &x' function.   The .TI control  word is occa- */
/* sionally of use when output must have  a character set larger than the */
/* input character set.   For example, a 029 keypunch lacks the lowercase */
/* alphabetics but using .TI and .TR translate tables "$A" could print as */
/* uppercase and "A" as lower.   See the Translate control word (.TR) for */
/* more information on specifying the input translate table.              */
/*                                                                        */
/* EXAMPLES                                                               */
/* (1) .ti < AD > BD ( C0 ) D0 . AF                                       */
/*     .ti set ›                                                          */
/*     ›. INDex›<-Queues›> ›(›<Time=(›<mm›>›<,ss›>)›)›>                   */
/*     The above sequence produces:                                       */
/*     ù INDex[-Queues] {[Time=([mm][,ss])}]                              */
/**************************************************************************/


/**************************************************************************/
/* TRANSLATE allows the user to specify a  translate table to be used for */
/* output.                                                                */
/*                                                                        */
/*      ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿      */
/*      |       |                                                  |      */
/*      |  .TR  |    <<s1 t1> <s2 t2> ...>                         |      */
/*      |       |    <s <s|t>>                                     |      */
/*      |       |                                                  |      */
/*      ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ      */
/*                                                                        */
/* This control word does not cause a break.  Unless the TRANSLATE option */
/* was specified when SCRIPT was invoked, no output translation will take */
/* place until  a .TR table  has been  defined.   Specifying .TR  with no */
/* operands eliminates all output translations and reinstates the initial */
/* default.                                                               */
/*                                                                        */
/* <s1 t1> <s2  t2> ...:  adds the  specified "source-to-target" transla- */
/*    tions to the translate table.   The "s" and "t" can be single char- */
/*    acters or two-digit hexadecimal values using uppercase or lowercase */
/*    letters.   All  subsequent output  lines will  be printed  with all */
/*    occurrences of "s1" replaced by "t1", etc.                          */
/* s <s|t>:  this form  specifies that "s" is to be  translated to itself */
/*    if a "t" is not specified.                                          */
/*                                                                        */
/* Many of the common  uses of .TR can also be achieved  by using the &x' */
/* function.    No  translation,   except  uppercase  conversion  if  the */
/* TRANSLATE option was specified, will be in effect until .TR is encoun- */
/* tered with operands.                                                   */
/*                                                                        */
/* NOTES                                                                  */
/* (1) The text of  footnotes and other storage blocks  are translated as */
/*     they are input.                                                    */
/* (2) The text of running titles is translated using the translate table */
/*     current when the title is output.                                  */
/* (3) SCRIPT control lines are never translated.                         */
/* (4) Translate pairs remain active until explicitly re-specified.       */
/* (5) Hexadecimal numbers are recognized by  the presence of two charac- */
/*     ters (instead of one)  and may  use uppercase or lowercase letters */
/*     A-F.                                                               */
/* (6) The last pair in  a .TR line may consist of  only a "source" char- */
/*     acter, which indicates that the character is to be translated into */
/*     itself.                                                            */
/*                                                                        */
/* EXAMPLES                                                               */
/* (1) .TR 8D ( 9D ) B0 0 ... B9 9                                        */
/*     Causes the superscript parentheses and numbers of the TN character */
/*     set to display as ordinary parentheses and numbers.                */
/* (2) .TR % 7C                                                           */
/*     Causes occurrences  of the  character "%"  to be  replaced by  the */
/*     character X'7C', the "@" character, which may not be easy to enter */
/*     into a file by virtue of being the "character delete" character in */
/*     some systems.                                                      */
/* (3) .TR 40 ?                                                           */
/*     Causes all blanks to appear as "?" on output.                      */
/* (4) .TR 05 40                                                          */
/*     This is an unsuccessful attempt to  remove all TAB characters from */
/*     the input and replace them with blanks.   It will fail because TAB */
/*     characters are expanded during input processing, not on output.    */
/**************************************************************************/

void    scr_ti( void )
{
    char        *   p;

    p = scan_start;
    while( *p && *p != ' ' ) {          // over cw
        p++;
    }
    while( *p && *p == ' ' ) {          // next word start
        p++;
    }

    cop_ti_table( p );
    add_to_sysdir( "$tiset", in_esc );  // put in dictionary

    scan_restart = scan_stop + 1;
    return;
}


void    scr_tr( void )
{
    char        *   p;

    p = scan_start;
    while( *p && *p != ' ' ) {          // over cw
        p++;
    }
    while( *p && *p == ' ' ) {          // next word start
        p++;
    }
    cop_tr_table( p );

    scan_restart = scan_stop + 1;
    return;
}

