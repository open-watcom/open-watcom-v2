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
* Description: implement .dc define character
*                            only CW, GML and TB options implemented
*                        .cw script control word separator
*  comments are from script-tso.txt
****************************************************************************/

#include "wgml.h"

#include "clibext.h"

/***************************************************************************/
/* DEFINE CHARACTER defines  characters of special meaning  when found in  */
/* input and produced as output.                                           */
/*                                                                         */
/*      旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴커       */
/*      |       |                                                  |       */
/*      |  .DC  |    option <char ...|OFF>                         |       */
/*      |       |                                                  |       */
/*      읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴켸       */
/*                                                                         */
/* ! only CW, GML and (STOP) are used in OW documentation and only these   */
/* ! are implemented                                                       */
/*                                                                         */
/*                                                                         */
/* "Option" specifies  which special  character or  characters are  to be  */
/* defined.  Any operands that follow specify the values of those charac-  */
/* ters.   All character operands may be entered as a single character or  */
/* as a pair of hexadecimal digits.   In all cases an "OFF" operand disa-  */
/* bles the feature  and a missing operand reinstates  the default value.  */
/* The default  and "OFF" values for  the characters are  specified under  */
/* the various  character options.    The character  options that  may be  */
/* specified are:                                                          */
/*                                                                         */
/* BS  (backspace):   defines a  character to be  treated as  a backspace  */
/*    character in  input text and  control words.   The  initial default  */
/*    value and the "OFF" value is hex 16,  the real backspace character,  */
/*    which  is always  recognized  as a  backspace  character even  when  */
/*    .DC BS has  been used  to define another.    The &SYSBS  Set Symbol  */
/*    returns the current user-defined backspace character,  or hex 16 if  */
/*    none has been defined.                                               */
/* CONT  (continue):   defines a character  for continuing an  input line  */
/*    without an intervening blank.   If an input text line ends with the  */
/*    continue character, the following input text line is placed immedi-  */
/*    ately following.   A control word that causes a break will clear an  */
/*    outstanding continue character and the character will not appear in  */
/*    the output.   The  initial and "OFF" value for continue  is a blank  */
/*    (hex 40).   The  Set Symbol &SYSCONT  returns the  current continue  */
/*    character.                                                           */
/* CW   (control  word  separator):  defines  a  character  for  entering  */
/*    multiple control words on one input  line.   Text lines (lines that  */
/*    do not begin with a control  word indicator)  are never affected by  */
/*    this character.   The initial control word separator character is a  */
/*    semicolon (;).  The "OFF" value is a hex 00.  The Set Symbol &SYSCW  */
/*    returns the current control word separator character.                */
/* GML  (Generalized Markup Language tag  indicator):  the character that  */
/*    precedes a GML Tag.  The initial and "OFF" value for GML is a blank  */
/*    (hex 40).   The default character, if none is specified, is a colon  */
/*    (:).  The &SYSGML Set Symbol returns the current GML character.      */
/* HYPH  (hyphenation character):  defines the  character to be placed at  */
/*    the end  of words hyphenated by  SCRIPT.   The default  and initial  */
/*    Hyphenation Character is a minus (-, hex 60).  The "OFF" value is a  */
/*    blank.                                                               */
/* HYTR  (Hyphenation Translate):   defines character  pairs ("from"  and  */
/*    "to") to be used when a word is processed for hyphenation.  Leading  */
/*    characters may be ignored by translating  them to single quotes and  */
/*    embedded characters can be treated as  valid break points by trans-  */
/*    lating them to a hyphen.  The default translation is none.           */
/* IXB  (Index  Blank):  define characters that  are to be treated  as if  */
/*    they were blanks when sorting Index Headers.   The table of IXB and  */
/*    IXI characters is  reset when no characters are  specified or "OFF"  */
/*    is used.  There are no initial IXB characters.  See .DC IXI also.    */
/* IXI  (Index  Ignore):  define characters that  are to be  ignored when  */
/*    sorting Index Headers.   There are no initial IXI characters.   See  */
/*    .DC IXB also.                                                        */
/* IXJ  (IndeX  reference Join  character):  defines  the character  that  */
/*    will be used  to join index references on  consecutive pages.   The  */
/*    default and initial index reference  join character is a hyphen(-).  */
/*    The "OFF"  value is a blank.    The &SYSIXJ Set Symbol  returns the  */
/*    index reference join character.                                      */
/* LB  (leading blank character):  defines  how a leading blank character  */
/*    on  an input  text record  is to  be processed.    The initial  and  */
/*    default action is  to process leading blanks as  Required Blanks or  */
/*    En Spaces in the current font.  Any printable character may also be  */
/*    selected.   The "OFF"  value will treat these  characters as blanks  */
/*    with width equal to the Characters Per Inch (CPI) option.            */
/* LI  (control word  indicator):  defines the character  that identifies  */
/*    special control  records when it  occurs at  the start of  an input  */
/*    line.   If the character occurs once then it may be a macro invoca-  */
/*    tion or a native control word.   If the character occurs twice in a  */
/*    row then  it may only  be a  control word indicator.    The initial  */
/*    control word  indicator is  a period  (.).   The  "OFF" value  is a  */
/*    blank.   The  &SYSLI Set  Symbol returns  the current  control word  */
/*    indicator.                                                           */
/* LINB  (Nobreak  control word indicator):   defines the  character that  */
/*    identifies an input line as a control word but suppresses the break  */
/*    action of  that control word.    The initial  and "OFF" value  is a  */
/*    blank.  The &SYSLINB Set Symbol returns the current nobreak control  */
/*    word indicator.                                                      */
/* MCS  (GML Markup/Content Separator):  the  character that terminates a  */
/*    GML Tag and its attributes.   The initial and default character, if  */
/*    none is specified,  is a period (.).   The "OFF" value for MCS is a  */
/*    blank (hex 40).    The &SYSMCS Set  Symbol returns the  current MCS  */
/*    character.                                                           */
/* PIX  (Primary IndeX reference entry indicator):  defines the character  */
/*    that,  when  used as the first  or only character in  an "alternate  */
/*    reference entry" with the .IX control word, causes that index entry  */
/*    to appear at the beginning of the list of reference entries.   This  */
/*    character does not get added to  the reference entry.   The default  */
/*    and initial primary index character is an asterisk (*).   The "OFF"  */
/*    value is  a blank.    The &SYSPIX  Set Symbol  returns the  current  */
/*    primary index character.                                             */
/* PS  (Page  Symbol):  defines the character  that is to be  replaced by  */
/*    the current  page number in title  control words.   It may  also be  */
/*    used  in Set  Symbol assignments.    The default  and initial  page  */
/*    symbol character is the percent (%).    The "OFF" value is a blank.  */
/*    The &SYSPS Set Symbol returns the current page symbol character.     */
/* PUNC  (Spell Punctuation):   define characters that are  to be consid-  */
/*    ered as part of a word if they  occur in the middle of a word,  but  */
/*    are to be ignored  if they occur at the end.    The initial default  */
/*    PUNC character is the single quote.  See .DC WORD also.              */
/* RB  (Required Blank):  defines a character that  is to be treated as a  */
/*    required blank (will appear as a blank  on output,  but will not be  */
/*    recognized as an interword blank).  This is useful for keeping text  */
/*    together on one line.  The initial and default value for a required  */
/*    blank is  hex 00.   The "OFF"  value is  a blank.   The  &SYSRB Set  */
/*    Symbol returns the current required blank character.                 */
/* STOP  (full-stop punctuation):  defines the characters that are recog-  */
/*    nized as "full stops"  when they occur at the end  of an input text  */
/*    line.  Two intervening blanks will be placed after such characters,  */
/*    before the next word on the  output line.   The initial and default  */
/*    list of stop characters are the period (.), question (?),  exclama-  */
/*    tion (!)   and colon (:).   When  SCRIPT is invoked with  the PHOTO  */
/*    option, "OFF" is default.                                            */
/* SUB (SUBscript  characters):  defines  the characters  that are  to be  */
/*    used for  the Subscript  Function (&s').    Character operands  are  */
/*    specified in  pairs,  with  the first  specifying the  normal input  */
/*    character  and the  second specifying  the output  character to  be  */
/*    generated when  the input character is  used in the operand  of the  */
/*    Subscript Function.   The initial  and default Subscript characters  */
/*    are  defined in  the  ALA print  character  assignments.   See  the  */
/*    Appendix on "Print  Trains" for those character  assignments.   The  */
/*    "OFF" operand translates all Subscript Function characters to them-  */
/*    selves.                                                              */
/* SUP  (SUPerscript characters):   defines character  pairs ("from"  and  */
/*    "to") to be used with the Superscript Function (&S').   The default  */
/*    character assignments are those of the TN character assignments.     */
/* TB  (TaB  character):  defines a character  that will be treated  as a  */
/*    tab character in input text and control words.  The initial default  */
/*    value and the "OFF" value is hex 05, the real tab character,  which  */
/*    is always recognized  as a tab character even when  .DC TB has been  */
/*    used to define another.   The &SYSTB Set Symbol returns the current  */
/*    user-defined tab character, or hex 05 if none has been defined.      */
/* TI  (Translate on Input):  defines an escape character that causes the  */
/*    character immediately  following it to  be translated  according to  */
/*    the .TI translate table currently  in effect.   The initial default  */
/*    value  and the  "OFF" value  is a  blank.   The  &SYSTI Set  Symbol  */
/*    returns the current escape character.                                */
/* WORD   (Spell Word  Delimiters):   define  characters that  are  never  */
/*    considered as part of a word.  By default, only the alphabetics are  */
/*    valid  characters and  everything else  will terminate  a word  for  */
/*    purposes of Spelling Verification.  The single quote (see .DC PUNC)  */
/*    is valid within a word.   Any given character may be either a Spell  */
/*    Punctuation character or a Spell Word Delimiter character,  but not  */
/*    both at the same time.                                               */
/* XTEXT  (Transparent Text):   defines the character that  delimits text  */
/*    that is transparent for purposes of  justifying lines when it imme-  */
/*    diately follows the escape character.    Such transparent text will  */
/*    follow <escape><xtext> and is terminated on the right by an <xtext>  */
/*    character  or the  end of  the input  line.   Any  text except  the  */
/*    <xtext> character itself,  including  other escapes and backspaces,  */
/*    may appear between the delimiters.    The initial default value and  */
/*    the "OFF" value is a blank.    The &SYSXTEXT Set Symbol returns the  */
/*    current transparent text character.                                  */
/*                                                                         */
/* This control word does not cause a break in any of its forms.           */
/*                                                                         */
/* EXAMPLES                                                                */
/* (1) .dc BS   <      ;.* User Backspace Character                        */
/*     .dc CONT +      ;.* Input Continue Character                        */
/*     .dc RB @        ;.* User Required Blank Character                   */
/*     .ce 1                                                               */
/*     Centre this cont+                                                   */
/*     inued text line                                                     */
/*     H<_ilighted text<<<<____                                            */
/*     .br                                                                 */
/*     Special terms in "@your@text@" may require                          */
/*     special S@P@A@C@I@N@G and must always be                            */
/*     kept@together@on@a@single@output@line.                              */
/*     produces:                                                           */
/*                      Centre this continued text line                    */
/*     Hilighted text                                                      */
/*     Special terms  in " your text " may require  special S P A C I N G  */
/*     and must always be kept together on a single output line.           */
/***************************************************************************/

/***************************************************************************/
/* CONTROL WORD SEPARATOR defines the character  that SCRIPT is to recog-  */
/* nize as  a "logical line  end" so that  multiple control words  may be  */
/* entered on one physical input line.                                     */
/*                                                                         */
/*      旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴커       */
/*      |       |                                                  |       */
/*      |  .CW  |    <;|character>                                 |       */
/*      |       |                                                  |       */
/*      읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴켸       */
/*                                                                         */
/* This control word does not cause a break.  All subsequent control-word  */
/* input lines  are examined for the  control word separator.    If found  */
/* then  the physical  input line  is  logically divided  into two  input  */
/* lines,  the second of which starts at the first column after the sepa-  */
/* rator.   The control-word-separator  scanning rule is then  applied to  */
/* this resulting second  input line.   The initial  default control word  */
/* separator character is the semi-colon (;).  Any character or two-digit  */
/* hexadecimal value may be specified.                                     */
/*                                                                         */
/* NOTES                                                                   */
/* (1) The last "logical" input line may be a text line.                   */
/* (2) If substitution (.SU) of Set Symbols is ON and the input line is a  */
/*     control-word line,   the substitution scan  will terminate  at the  */
/*     first control word separator.   The remainder  of the line will be  */
/*     substituted when it is processed later as an input line.            */
/* (3) Scanning for  the control  word separator may  be suspended  on an  */
/*     input control line if a single quote (',  the apostrophe)  immedi-  */
/*     ately  follows  the  control-word  indicator.    This  allows  the  */
/*     control-word separator to be used as operand text.  It also allows  */
/*     an entire input line to be substituted with set symbols.            */
/* (4) If the  operand is omitted,  the  existence of and scanning  for a  */
/*     control-word separator is terminated.  Therefore, multiple control  */
/*     words will not be recognized on subsequent input lines,  nor may a  */
/*     control word be followed by text on the same input line.            */
/* (5) Disabling the separator character is the same as ".CW 00".          */
/*                                                                         */
/* EXAMPLES                                                                */
/* (1) .'bd This is text; it contains a semi-colon.                        */
/*     produces:  This is text; it contains a semi-colon.                  */
/*     The  control word  modifier  (')  suspends  control-word-separator  */
/*     scanning and  allows semi-colons  to be  entered in  the operands.  */
/*     The  same  input line  without  the  control word  modifier  would  */
/*     produce:  This is text                                              */
/*      it contains a semi-colon.                                          */
/* (2) .sr i = &i + 1;The value of I is "&i".                              */
/*     The value of "i"  is incremented by one and the  result is used in  */
/*     the text input line that follows.                                   */
/* (3) .sk;.of 4;(a) This is text in a list.                               */
/* (4) .cw #;.rt top /left;/center;/right;/#.cw ;#.sk;.cm                  */
/*     This example makes the separator "#",  defines a TOP running title  */
/*     containing semi-colons,  redefines  the separator to be  ";",  and  */
/*     then uses it.                                                       */
/***************************************************************************/

/***************************************************************************/
/*  scr_cw    implement .cw control word                                   */
/***************************************************************************/

void    scr_cw( void )
{
    char        *   pa;
    char        *   p;
    int             len;

    p = scan_start;
    while( *p && *p == ' ' ) {          // next word start
        p++;
    }
    pa = p;
    while( *p && *p != ' ' ) {          // end of word
        p++;
    }
    len = p - pa;
    if( len > 2 ) {
        xx_line_err( err_inv_cw_sep, pa );
        return;
    } else if( len > 0 ) {             // 1 char or 2 hex characters
        CW_sep_char = parse_char( pa, len );
    } else {
        CW_sep_char = '\0';
    }
    add_to_sysdir( "$cw", CW_sep_char );
    scan_restart = pa + len;
    return;
}

/***************************************************************************/
/*  scr_dc    implement .dc define character control word                  */
/*              only some options are implemented                    TBD   */
/*              the STOP option is accepted, but ignored                   */
/***************************************************************************/

void    scr_dc( void )
{
    char        *   pa;
    char        *   p;
    char            c;
    int             len;
    int             k;
    char    string[2] = { 0, 0 };
    int             opt;
    static const char   options[5] [5] = { "cw", "gml", "tb", "stop" };
                                        // please add new options at end
    int const   max_opt = sizeof( options) / sizeof( options[0] );

    p = scan_start;
    while( *p && *p != ' ' ) {          // over dc
        p++;
    }
    while( *p && *p == ' ' ) {          // next word start = option
        p++;
    }
    pa = p;
    while( *p && *p != ' ' ) {          // end of word
        p++;
    }
    len = p - pa;
    opt = 0;
    if( len > 0 ) {
        for( k = 0; k < max_opt; k++ ) {
            if( !strnicmp( pa, options[k], len ) ) {
                opt = k + 1;
                break;
            }
        }
    }
    if( opt == 0 ) {                   // omitted / unknown / not implemented
        dc_opt_warn_len( pa, len );
        return;
    }
    while( *p && *p == ' ' ) {          // next word start = option value
        p++;
    }
    pa = p;
    c = '\0';
    while( *p && *p != ' ' ) {          // end of word
        p++;
    }
    len = p - pa;
    if( len == 1 ) {
        c = *pa;
    }
    switch( opt ) {
    case 1 :                            // CW option
        if( len > 2 ) {
            if( len == 3 ) {
                if( strnicmp( pa, "OFF", len ) ) {
                    xx_line_err_len( err_dc_not_off, pa, len );   // only OFF is valid
                    return;
                }
            } else {
                xx_line_err_len( err_dc_not_off, pa, len );   // only OFF is valid
                return;
            }
        } else {
            c = parse_char( pa, len );
        }
        scan_restart = pa + len;
        CW_sep_char = c;
        add_to_sysdir( "$cw", CW_sep_char );
        break;
    case 2 :                            // GML option
        if( len > 2 ) {
            if( len == 3 ) {
                if( strnicmp( pa, "OFF", len ) ) {
                    xx_line_err_len( err_dc_not_off, pa, len );   // only OFF is valid
                    return;
                }
                c = ' ';                    // OFF is blank
            } else {
                xx_line_err_len( err_dc_not_off, pa, len );   // only OFF is valid
                return;
            }
        } else {
            c = parse_char( pa, len );
        }
        scan_restart = pa + len;
        GML_char = c;
        string[0] = c;
        add_symvar( &global_dict, "gml", string, no_subscript,
                    predefined );
        add_to_sysdir( "$gml", GML_char );
        break;
    case 3 :                            // TB option
        if( len > 2 ) {
            if( len == 3 ) {
                if( strnicmp( pa, "OFF", len ) ) {
                    xx_line_err_len( err_dc_not_off, pa, len );  // only OFF is valid
                    return;
                }
                c = 0x09;               // OFF is 0x09
            } else {
                xx_line_err_len( err_dc_not_off, pa, len );  // only OFF is valid
                return;
            }
        } else {
            c = parse_char( pa, len );
        }
        scan_restart = pa + len;
        tab_char = c;
        string[0] = c;
        add_to_sysdir( "$tb", tab_char );
        add_to_sysdir( "$tab", tab_char );
        break;
    case 4 :                            // STOP option   DUMMY processing

/***************************************************************************/
/*  this is done to allow OW help processing to continue instead of exit   */
/*  due to premature output of text '.dc stop off' from              TBD   */
/*        docs\doc\whelp\whelp.gml line 765                                */
/*                                                                         */
/***************************************************************************/

        scan_restart = pa + len;
        /* fall thru */

    default:                            // unknown / unimplemented option
        dc_opt_warn_len( pa, len );
        break;
    }
    return;
}
