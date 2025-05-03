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
* Description: currently, non-implementation of control word CD
*
****************************************************************************/


#include "wgml.h"


/****************************************************************************/
/* HYPHENATE is used to set the automatic hyphenation function, to set      */
/* the level of automatic hyphenation required and to manipulate the        */
/* Hyphenation Exception Dictionary.                                        */
/*                                                                          */
/*       旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴커       */
/*       |       |                                                  |       */
/*       |       | <ON|USER|OFF|SUP>                                |       */
/*       |       | <LADDER <3|n|+n|-n>>                             |       */
/*       |       | <MAXPT <3|n|+n|-n>>                              |       */
/*       |       | <MINPT <3|n|+n|-n>>                              |       */
/*       |  .HY  | <MINWORD <5|n|+n|-n>>                            |       */
/*       |       | <UPPER <ALL|ON|OFF>>                             |       */
/*       |       | <USER <ON|OFF>>                                  |       */
/*       |       | <RULEs value>                                    |       */
/*       |       | <ADD|CHANGE|DELETE> word-with-breaks             |       */
/*       |       | <DUMP|PURGE>                                     |       */
/*       |       | <TEST word>                                      |       */
/*       |       |                                                  |       */
/*       읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴켸       */
/*                                                                          */
/* The Levels of Hyphenation                                                */
/*                                                                          */
/* ON: Enables the automatic hyphenation of words that are too long to      */
/*    fit on the output line.                                               */
/* USER: No automatic hyphenation will be performed. However, words         */
/*    containing hyphens such as "brother-in-law", may be broken after a    */
/*    "-" when concatenating text. It also causes a hyphen at the end of    */
/*    any input line to be treated as a conditional hyphen. The conditional */
/*    hyphen will be discarded if there is space for more text on           */
/*    the current output line and will remain as entered if the line can    */
/*    hold no more.                                                         */
/* OFF: No hyphenation at all will be performed. Only blanks between        */
/*    words are to be considered for the purpose of concatenating text.     */
/* SUP: Suppresses automatic hyphenation to the "OFF" level until the       */
/*    next break occurs. This is useful to effectively turn hyphenation     */
/*    off until the end of a paragraph, at which point the current          */
/*    setting is restored.                                                  */
/*                                                                          */
/* The Hyphenation Algorithm                                                */
/*                                                                          */
/* The technique used to hyphenate words may be selected from the           */
/* following list. You must still enable hyphenation with ".HY ON" for      */
/* any of the rules to be enabled. The user exception dictionary of         */
/* words is always used in conjunction with any rule selected.              */
/*                                                                          */
/* RULEs ON: This enables the default hyphenation for the English           */
/*    language. It is done with three different techniques plus a           */
/*    built-in exception dictionary. The algorithmic hyphenation rules      */
/*    employed have been adapted and extended from a program package        */
/*    called HYPHENATION/360. See the IBM Application Description           */
/*    Manual, form E20-0257, for a clear description (in ten pages) of      */
/*    the algorithms employed.                                              */
/* RULEs ENGLISH: This enables hyphenation by comparing segments of the     */
/*    word with a predefined table of English word patterns. The technique  */
/*    used to derive these patterns from a hyphenated English word          */
/*    list is described in: "Word Hy-phen-a-tion by Com-put-er" by          */
/*    Franklin Mark Liang of the Department of Computer Science at Stanford */
/*    University (Report No. STAN-CS-83-977).                               */
/* RULEs FRENCH: This enables hyphenation with a table of French word       */
/*    patterns.                                                             */
/* RULEs GERMAN: This enables hyphenation with a table of German word       */
/*    patterns.                                                             */
/* RULEs ITALIAN: Hyphenation with Italian word patterns.                   */
/* Other RULEs: The operands DANISH, DUTCH, SPANISH and SWEDISH are also    */
/*    recognized but no hyphenation patterns for these languages are        */
/*    currently defined.                                                    */
/*                                                                          */
/* Limits for Automatic Hyphenation                                         */
/*                                                                          */
/* For all of these Hyphenation operands, a value of zero means "the        */
/* largest possible value".                                                 */
/*                                                                          */
/* LADDER <3|n|+n|-n>: Defines the maximum number of consecutive output     */
/*    lines that will be eligible for automatic hyphenation. After "n"      */
/*    consecutive hyphenated lines on the same page, the next output line   */
/*    will not be hyphenated. (SUP is an alias for LADDER.)                 */
/* MAXPT <3|n|+n|-n>: Defines the minimum number of characters that must    */
/*    be left after the hyphen. The initial and default value of 3 means    */
/*    at least three characters at the end of a word will be kept           */
/*    together. (ENDPT is an alias for MAXPT.)                              */
/* MINPT <3|n|+n|-n>: Defines the minimum number of characters that must    */
/*    appear before the hyphen. The initial and default value of 3 means    */
/*    at least three characters at the start of a word will be kept         */
/*    together.                                                             */
/* MINWORD <5|n|+n|-n>: Defines minimum number of characters in a           */
/*    hyphenated word. The value must be positive and has an initial and    */
/*    default value of 5, which means that no word with fewer than five     */
/*    characters will be hyphenated. (THRESH is an alias for MINWORD.)      */
/* UPPER ALL: All words containing letters and the user delimiters '-'      */
/*    and '/' are eligible for hyphenation, even if the word is all         */
/*    uppercase.                                                            */
/* UPPER ON: Words in which all the letters are uppercase will not be       */
/*    eligible for hyphenation. This is the initial value; proper names     */
/*    and the first word of a sentence may be hyphenated, but special       */
/*    terms such as "FORTRAN" or "ELECTROSTATIC" will never be hyphenated.  */
/* UPPER OFF: Only words that contain only lowercase letters are            */
/*    eligible for hyphenation.                                             */
/* USER ON: Compound words are eligible for hyphenation at points other     */
/*    than the compound break point (at a "-" or "/" in the word). The      */
/*    compound break point would only be used if it were the best break     */
/*    for the current length of line. This is the initial value for         */
/*    "USER".                                                               */
/* USER OFF: When a compound word is being examined for hyphenation         */
/*    points, only the compound break points in the word may be used.       */
/*    Therefore, a word such as "extra-special" may only be broken at the   */
/*    "-".                                                                  */
/*                                                                          */
/* The Hyphenation Exception Dictionary                                     */
/*                                                                          */
/* SCRIPT uses both algorithmic and exception-dictionary techniques to      */
/* hyphenate words. When running with multiple passes, you should           */
/* construct any large "user" exception dictionary only on the first        */
/* pass, as the exception dictionary is not cleared between passes.         */
/*    The exception-dictionary technique is essential, since the algorithms */
/* may not work successfully for all words in the language. SCRIPT          */
/* is distributed with a built-in dictionary of known "exception" words,    */
/* and it is possible to add to or override this internal dictionary via    */
/* a "user" dictionary. The "user" exception dictionary is something        */
/* that must be constructed during SCRIPT processing via the operands       */
/* discussed below.                                                         */
/*                                                                          */
/* ADD word-with-breaks: Inserts a word into the "user" exception           */
/*    dictionary. This is sometimes necessary to distinguish between        */
/*    words such as the noun "pres-ent" and the verb "pre-sent". A word     */
/*    may be specified without break points, such as "Waterloo", to         */
/*    prevent it from being broken across output lines. A word already      */
/*    in the dictionary may be added again, as these duplicates are         */
/*    entered in a "last in, first out" order.                              */
/* CHANGE word-with-breaks: Operates as "ADD" if the alphabetic characters  */
/*    of the word operand do not match any current entry. If the            */
/*    characters do match an entry, that entry is replaced with its new     */
/*    break points.                                                         */
/* DELETE word-with-breaks: To "DELETE" a word, the letters of the word     */
/*    and its break points must match. (Words cannot be DELETEd from the    */
/*    built-in internal dictionary.)                                        */
/* TEST word: To find all the hyphen break points of a word, use the        */
/*    "TEST" operand. SCRIPT will display the input word and the word       */
/*    with its break points shown as special characters.                    */
/*     *  yes/no table                                                      */
/*     -  exception dictionary                                              */
/*     /  suffix                                                            */
/*     +  probability algorithm                                             */
/*     @  pattern analysis                                                  */
/*    Numeric digits are used to indicate break points when word patterns   */
/*    are used, with higher numbers indicating higher probabilities.        */
/* PURGE: Deletes all the entries in the "user" exception dictionary.       */
/* DUMP: Displays all words in the current "user" exception dictionary      */
/*    at the terminal. Note that the words list by word length within       */
/*    the first letter of the word.                                         */
/*                                                                          */
/* This control word does not cause a break. There are no default operands. */
/* The initial setting of the hyphenation level is "USER" with no           */
/* entries in the "user" exception word dictionary.                         */
/*                                                                          */
/* NOTE: HY defaults to OFF in wgml 4.0                                     */
/****************************************************************************/

/************************************************************************/
/* this simply skips rest of the logical record                         */
/* the reason is that the OW Docs do not need it, but they do use it    */
/* but they never set HY to any state but OFF, which it starts at       */
/************************************************************************/

void scr_hy( void )
{
    scan_restart = scan_stop + 1;
    return;
}

