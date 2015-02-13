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
* Description: implement .gt script control word (define Gml Tag)
*              not all options are implemented
*       of the TEXTxxx options, only TEXTError is used/implemented
*
*  comments are from script-tso.txt
****************************************************************************/

#define __STDC_WANT_LIB_EXT1__  1      /* use safer C library              */

#include "wgml.h"
#include "gvars.h"


/***************************************************************************/
/*  reset last used  tag and att entries                                   */
/***************************************************************************/

void    init_tag_att( void )
{
    tag_entry = NULL;
    att_entry = NULL;
    tagname[0] = '*';
    attname[0] = '*';
}


/***************************************************************************/
/* GML TAG defines a GML tag.                                              */
/*                                                                         */
/*      旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴커       */
/*      |       |                                                  |       */
/*      |  .GT  |    tagname ADD macro <tagopts>                   |       */
/*      |       |    tagname CHAnge macro                          |       */
/*      |       |    tagname OFF|ON                                |       */
/*      |       |    tagname|* DELete|PRint                        |       */
/*      |       |                                                  |       */
/*      읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴켸       */
/*                                                                         */
/* The first  form of this  control word defines the  name of a  GML tag,  */
/* identifies the macro that is to be invoked when the tag is encountered  */
/* in input,   and specifies "tag  options" that control  tag processing.  */
/* The other forms permit modifications to an already-defined tag, or the  */
/* display of information about already-defined tags.                      */
/*                                                                         */
/* Overview                                                                */
/*                                                                         */
/* SCRIPT provides abilities  for the text programmer to create  a set of  */
/* "Generalized Markup Language" (GML)  tags and the related application-  */
/* processing functions (APFs).  The collection of APF macros is called a  */
/* "layout".   For details on the syntax of  GML tags and an example of a  */
/* large tagset with multiple layouts, see the Waterloo SCRIPT/GML User's  */
/* Guide.                                                                  */
/*    The tags and  their attributes and values are defined  by using the  */
/* GML Tag (GT)   and GML Attribute (GA)  control  words.   SCRIPT builds  */
/* three internal data  structures from the information  specified by the  */
/* GT and GA control words:   a tagname structure, with an attribute-name  */
/* structure for each tagname, and a value-list structure for each attri-  */
/* bute of the tag.   The information in these data structures is used in  */
/* the processing of GML tags that occur in user input.  The application-  */
/* processing functions (APFs)  are defined as macros by using the Define  */
/* Macro control  word (DM).    The character  with which  the user  must  */
/* precede each tagname, in order for SCRIPT to recognize it,  is defined  */
/* by the text programmer by using  the "GML" operand of Define Character  */
/* (DC).                                                                   */
/*                                                                         */
/* The "ADD" Operand                                                       */
/*                                                                         */
/* This form of the control word is used to define a GML tag.   It speci-  */
/* fies the name of the GML tag,  the name of the macro that provides the  */
/* application-processing function for that tag,  and a number of related  */
/* "tag option" operands,  as appropriate.   A tagname may not consist of  */
/* more than  15 alphanumeric characters.    The "tag options"  and their  */
/* meanings are as follows; note that some of these operands are mutually  */
/* exclusive.                                                              */
/*                                                                         */
/* ATTributes:  This tag has one or  more attributes.   If not specified,  */
/*    then  any subsequent  GML  Attribute (GA)   control  word for  this  */
/*    tagname will result in an error message.                             */
/* CONTinue:   A "continue"  character (see  the CONT  operand of  Define  */
/*    Character)  is  to be  generated internally by  SCRIPT if  this tag  */
/*    occurs after column one of an input line.   If not specified,  each  */
/*    in-line occurrence  of the tag will  be processed as though  it had  */
/*    occurred at the start of an input line of its own.                   */
/* CSOFF:  This tag  will cause the termination of  an outstanding Condi-  */
/*    tional Section 99 (see CS).                                          */
/* NOCONTinue:  This tag  cannot be continued by another tag  on the same  */
/*    input line;   the second  tag will  be processed  as though  it had  */
/*    occurred at the start of an input line of its own.                   */
/* TAGnext:  Another tag  must follow this tag.   Whether  the second tag  */
/*    may occur on the same input line  is not relevant;  what matters is  */
/*    that a  text input line may  not follow this tag.    (The necessary  */
/*    processing code to enforce this during subsequent SCRIPT processing  */
/*    has not yet been implemented.)                                       */
/* TEXTDef 'default tagtext':  The specified  "default tagtext" string is  */
/*    to be used  as the tagtext for  this tag if the  user specified the  */
/*    tag without tagtext.                                                 */
/* TEXTError:  If tagtext  is specified with this tag,   an error message  */
/*    will result.                                                         */
/* TEXTLine:  Everything from the tagname to the end of the input line is  */
/*    to be treated as tagtext.                                            */
/* TEXTReqd:  If tagtext is not specified with this tag, an error message  */
/*    will result.                                                         */
/*                                                                         */
/* Other Forms of the Control Word                                         */
/*                                                                         */
/* The other  forms of  the control  word modify  or display  information  */
/* contained in the data structures.                                       */
/*                                                                         */
/* tagname CHAnge  macro:  The specified  "macro" name becomes  the macro  */
/*    that will  be invoked  when this tag  is encountered  in subsequent  */
/*    input.                                                               */
/* tagname DELete:   This tag and its  related attributes and  values are  */
/*    deleted from the data structures.                                    */
/* *  DELete:  All  tags  and their  related  attributes  and values  are  */
/*    deleted from the data structures.                                    */
/* tagname OFF|ON:  OFF  causes subsequent occurrences of this  tag to be  */
/*    ignored;  any attribute list or tagtext will not be validated,  and  */
/*    the APF macro  will not be invoked.   ON terminates  a previous OFF  */
/*    condition; no error results if the OFF condition was not in effect.  */
/* tagname PRint:   The current data-structure  information for  this tag  */
/*    and  its attributes  and values  will  be displayed  on the  output  */
/*    device  to which  the  results of  a TYpe  control  word (TY)   are  */
/*    directed.   (This will be of occasional  use to the text programmer  */
/*    as a debugging aid during the tagset-development process.)           */
/* * PRint:   As above,  but  displays the  information for all  tags and  */
/*    their related attributes and values.                                 */
/*                                                                         */
/* GML Tag Scanning                                                        */
/*                                                                         */
/* Once a Define Character "GML" character has been defined, SCRIPT exam-  */
/* ines every  subsequent input line  for occurrences of  that character.  */
/* When an occurrence of the character is found, SCRIPT begins collecting  */
/* subsequent characters until it encounters a blank,  a period,  another  */
/* "GML" character,  or the end of the input line.   SCRIPT then searches  */
/* the tagname data  structure looking for a tagname equal  to the upper-  */
/* case version  of the  collected characters.    If no  match is  found,  */
/* SCRIPT treats  the "GML" character  and collected characters  as text,  */
/* and continues scanning for the next occurrence of a "GML" character in  */
/* the input line.   If a match is found, however, then SCRIPT splits the  */
/* original input line into two "logical" input lines, the first of which  */
/* contains all characters  to the left of the "GML"  character,  and the  */
/* second  of which  begins  with the  first  character  of the  tagname.  */
/* SCRIPT  then resumes  the process  of scanning  this second  "logical"  */
/* input line, starting after the tagname and its delimiter,  looking for  */
/* another "GML" character.                                                */
/*    In this manner,   an input line containing (text and)   one or more  */
/* tags is parsed into one or more  "logical" input lines.   If the orig-  */
/* inal input  line started with text,   then the first of  the resulting  */
/* "logical" input lines will  be a text line,  and will  be processed as  */
/* any other text  input line,  with the possible exception  that the tag  */
/* that followed it may have caused a "continue" character to be appended  */
/* to it.                                                                  */
/*    Any remaining  "logical" input lines  (those representing  the tags  */
/* and what is assumed to be their attribute lists and tagtext)  are then  */
/* re-inserted  back into  the  input stream,   prefixed  with a  special  */
/* "Generated by  GML processing" (GG)   control word.   (Because  the GG  */
/* control word is not intended for use by the SCRIPT user, no commitment  */
/* is made to its continued existence as "GG";  it might be replaced in a  */
/* future version  of SCRIPT by  a control word  of some other  name,  or  */
/* deleted.)                                                               */
/*    The exception to the above is a  tag that has been defined with the  */
/* TEXTLine option.   Everything from the tagname to the end of the input  */
/* line will be treated as tagtext.                                        */
/*                                                                         */
/* Attribute-List and Tagtext Processing                                   */
/*                                                                         */
/* The  parsing and  validation of  the  attribute list  and tagtext  are  */
/* handled by the GG control word, as follows:                             */
/* (1) The existence  of this  tagname in the  tagname data  structure is  */
/*     verified,   or  an  error  message  is  produced  and  no  further  */
/*     processing occurs.   This re-verification is necessary because the  */
/*     tagname may have  been deleted from the data structure  by the APF  */
/*     macro for a previous tag on the same (original) input line.         */
/* (2) If the  OFF condition is  in effect  for the tagname,   no further  */
/*     processing occurs.   (Note  that a tag that has  been defined with  */
/*     the TEXTLine option and is in  the OFF condition provides the text  */
/*     programmer with  the ability  to define tags  that are  treated as  */
/*     comments).                                                          */
/* (3) The existence of the APF macro for this tagname is verified, or an  */
/*     error message is produced and no further processing occurs.         */
/* (4) A local symbol dictionary is pre-generated for the APF macro,  and  */
/*     all  AUTO and  DEFAULT  attribute names  and  values  (see GA  for  */
/*     details) are added to it as local symbols of the form "*attname".   */
/* (5) Any user-specified attribute list is scanned, from left to right.   */
/*       (a)  For an attribute of the  form "attname=value",  if the name  */
/*     does not exist  in the tag's attribute-list data  structure or the  */
/*     attribute name  has been  defined as  AUTO,  an  error message  is  */
/*     produced and no further processing occurs.   Otherwise, the attri-  */
/*     bute value is  verified in the value-list data  structure for this  */
/*     attribute of the tag.    If it is not valid,  an  error message is  */
/*     produced and no further processing occurs.   Otherwise, the attri-  */
/*     bute name and value are added to the local symbol dictionary being  */
/*     generated for the APF macro.                                        */
/*       (b)  For  an attribute of the  form "attname" with  no "=value",  */
/*     processing is  handled as in (a)   if the attribute has  a default  */
/*     value.   However,  if the attname does not exist in the attribute-  */
/*     list data structure, then a WARNING is generated and the string is  */
/*     assumed to be the start of the tagtext.                             */
/* (6) If tagtext  was specified  but not  permitted,  or  if tagtext  is  */
/*     required but was not specified,  an  error message is produced and  */
/*     no further processing occurs.   Otherwise, any tagtext (or default  */
/*     tagtext,  if any)  is added to  the local symbol dictionary as the  */
/*     local symbol  "*",  with  a "continue"  character appended  to its  */
/*     value if  required and  permitted (if  not permitted,   it is  not  */
/*     appended).                                                          */
/* (7) The tag-usage counter for this tagname is incremented by one,  and  */
/*     the resulting value is added to the local symbol dictionary as the  */
/*     value of  the local symbol  "*_N".   The  tagname is added  to the  */
/*     local symbol dictionary as the value of the local symbol "*_TAG".   */
/* (8) If  the "GML"  operand of  Input Trace  (IT)  is  in effect,   the  */
/*     "logical"  input  line  and  the  contents  of  the  local  symbol  */
/*     dictionary are displayed in the trace output.                       */
/* (9) The APF macro is invoked.                                           */
/*                                                                         */
/* The Local Symbol Dictionary for the APF Macro                           */
/*                                                                         */
/* The pre-generation of the local symbol dictionary completely by-passes  */
/* the  normal operand-scanning  and  initialization  processes of  macro  */
/* invocation.   As a result,  symbols such as "*0" and "*1" will have no  */
/* values within a macro that has been invoked as the APF macro for a GML  */
/* tag,  nor  will the  values of  the global  symbols "0",   "1",...  be  */
/* altered.                                                                */
/***************************************************************************/


/***************************************************************************/
/*  process .gt xxx ADD xxx options                                        */
/*  Allowed options are shown in the strnicmp calls                        */
/*  Minimum Abbreviation is the uppercase part of each option              */
/***************************************************************************/

static  condcode    scan_tag_options( gtflags * tag_flags )
{
    char        *   p;
    condcode        cc = pos;

    while( cc == pos ) {

        cc = getarg();
        if( cc == omit ) {              // nothing more
            break;
        }
        p = tok_start;
        switch( tolower( *p ) ) {
        case   'a' :
            if( (arg_flen > 2) && (arg_flen < 12)
                && !strnicmp( "ATTributes", p, arg_flen ) ) {

                *tag_flags |= tag_attr;
            } else {
                cc = neg;               // invalid option
            }
            break;
        case   'c' :
            if( (arg_flen == 5)
                && !strnicmp( "CSOFF", p, 5 ) ) {

                *tag_flags |= tag_csoff;
            } else {
                if( (arg_flen > 3) && (arg_flen < 9)
                    && !strnicmp( "CONTinue", p, arg_flen ) ) {

                    *tag_flags |= tag_cont;

                } else {
                    cc = neg;           // invalid option
                }
            }
            break;
        case   'n' :
            if( (arg_flen > 5) && (arg_flen < 11)
                && !strnicmp( "NOCONTinue", p, arg_flen ) ) {

                *tag_flags |= tag_nocont;
            } else {
                cc = neg;               // invalid option
            }
            break;
        case   't' :
            if( (arg_flen > 3) && (arg_flen < 8)
                && !strnicmp( "TAGNext", p, arg_flen ) ) {

                *tag_flags |= tag_next;
            } else {
                if( (arg_flen > 4 && (arg_flen < 10) ) ) {
                    if( !strnicmp( "TEXTError", p, arg_flen ) ) {

                        *tag_flags |= tag_texterr;
                    } else {
                        if( !strnicmp( "TEXTReqd", p, arg_flen ) ) {

                            *tag_flags |= tag_textreq;
                        } else {
                            if( !strnicmp( "TEXTLine", p, arg_flen ) ) {

                                *tag_flags |= tag_textline;
                            } else {
                                if( !strnicmp( "TEXTDef", p, arg_flen ) ) {

                                    *tag_flags |= tag_textdef;
                                } else {
                                    cc = neg;   // invalid option
                                }
                            }
                        }
                    }
                } else {
                    cc = neg;           // invalid option
                }
            }
            break;
        default:
            cc = neg;                   // invalid option
            break;
        }
    }
    return( cc );
}


/***************************************************************************/
/*  scr_gt    implement .gt control word                                   */
/***************************************************************************/

void    scr_gt( void )
{
    char        *   p;
    char        *   pn;
    char            savetag;
    int             k;
    int             len;
    char            macname[MAC_NAME_LENGTH + 1];
    condcode        cc;
    gtentry     *   wk;
    gtflags         tag_flags;
    enum {
        f_add       = 1,
        f_change,
        f_delete,
        f_off,
        f_on,
        f_print
    } function;

    garginit();                         // find end of CW

    /***********************************************************************/
    /*  isolate tagname   or use previous if tagname *                     */
    /***********************************************************************/

    cc = getarg();                      // Tagname

    if( cc == omit ) {
        // no operands
        tag_name_missing_err();
        return;
    }

    p = tok_start;

    if( *p == '*' ) {                   // single * as tagname
        if( arg_flen > 1 ) {
            xx_err( err_tag_name_inv );
            return;
        }
        savetag = '*';         // remember for possible global delete / print
        if( GlobalFlags.firstpass && input_cbs->fmflags & II_research ) {
            if( tag_entry != NULL ) {
                out_msg("  using tagname %s %s\n", tagname, tag_entry->name );
            }
        }
    } else {
        savetag = ' ';               // no global function for delete / print

        init_tag_att();            // forget previous values for quick access
        attname[0] = '*';

        pn      = tagname;
        len     = 0;

        while( *p && is_macro_char( *p ) ) {
            if( len < TAG_NAME_LENGTH ) {
                *pn++ = tolower( *p++ );// copy lowercase tagname
                *pn   = '\0';
            } else {
                break;
            }
            len++;
        }
        for( k = len; k < TAG_NAME_LENGTH; k++ ) {
            tagname[k] = '\0';
        }
        tagname[TAG_NAME_LENGTH] = '\0';

        if( len < arg_flen ) {
            xx_err( err_tag_name_inv );
            return;
        }
    }


    /***********************************************************************/
    /* get function operand  add, change, ...                              */
    /***********************************************************************/

    cc = getarg();

    if( cc == omit ) {
        xx_err( err_tag_func_inv );
        return;
    }

    p = tok_start;
    function = 0;
    switch( tolower( *p ) ) {
    case   'a':
        if( !strnicmp( "ADD ", p, 4 ) ) {

            function = f_add;
        }
        break;
    case 'c' :
        if( (arg_flen > 2) && (arg_flen < 7)
            && !strnicmp( "CHANGE", p, arg_flen ) ) {

            function = f_change;
        }
        break;
    case 'o' :
        if( !strnicmp( "OFF", p, 3 ) ) {

            function = f_off;
        } else {
            if( !strnicmp( "ON", p, 2 ) ) {

                function = f_on;
            }
        }
        break;
    case 'd' :
        if( (arg_flen > 2) && (arg_flen < 7)
            && !strnicmp( "DELETE", p, arg_flen ) ) {

            function = f_delete;
        }
        break;
    case 'p' :
        if( (arg_flen > 1) && (arg_flen < 6)
            && !strnicmp( "PRINT", p, arg_flen ) ) {

            function = f_print;
        }
        break;
    default:
        // nothing
        break;
    }
    if( function == 0 ) {               // no valid function specified
        xx_err( err_tag_func_inv );
        return;
    }

    cc = getarg();                      // get possible next parm

    /***********************************************************************/
    /*  for add and change    get macroname                                */
    /***********************************************************************/

    if( function == f_add || function == f_change ) {   // need macroname
        if( cc == omit ) {
            xx_err( err_tag_mac_name );
            return;
        }
        p = tok_start;

        pn      = macname;
        len     = 0;

        while( *p && is_macro_char( *p ) ) {
            if( len < MAC_NAME_LENGTH ) {
                *pn++ = tolower( *p++ );    // copy lowercase macroname
                *pn   = '\0';
            } else {
                break;
            }
            len++;
        }
        for( k = len; k < MAC_NAME_LENGTH; k++ ) {
            macname[k] = '\0';
        }
        macname[MAC_NAME_LENGTH] = '\0';

        tag_flags = 0;

        if( function == f_add ) {       // collect tag options
            cc = scan_tag_options( &tag_flags );
            if( cc != omit ) {          // not all processed error
               xx_err( err_tag_opt_inv );
            }
            tag_entry = add_tag( &tag_dict, tagname, macname, tag_flags );  // add to dictionary
            // if tag_entry is now NULL, error (+ msg) was output in add_tag
        } else {                        // is function change
            tag_entry = change_tag( &tag_dict, tagname, macname );
        }
    } else {

    /***********************************************************************/
    /*  after delete, off, on, print nothing allowed                       */
    /***********************************************************************/

        if( cc != omit ) {
            xx_err( err_tag_toomany );  // nothing more allowed
        }

        switch( function ) {
        case f_print :
            if( savetag == '*' ) {
                print_tag_dict( tag_dict );
            } else {
                print_tag_entry( find_tag( &tag_dict, tagname ) );
            }
            break;
        case f_delete :
            if( savetag == '*' ) {
                free_tag_dict( &tag_dict );
            } else {
                free_tag( &tag_dict, find_tag( &tag_dict, tagname ) );
            }
            break;
        case f_off :
            if( savetag == '*' && tag_entry != NULL ) {// off for last defined
                tag_entry->tagflags |= tag_off;
            } else {
                wk = find_tag( &tag_dict, tagname );
                if( wk != NULL ) {
                    wk->tagflags |= tag_off;
                }
            }
            break;
        case f_on :
            if( savetag == '*' && tag_entry != NULL ) {// on for last defined
                tag_entry->tagflags |= tag_off;
            } else {
                wk = find_tag( &tag_dict, tagname );
                if( wk != NULL ) {
                    wk->tagflags &= ~tag_off;
                }
            }
            break;
        default:
            break;
        }
    }
    scan_restart = scan_stop;
    return;
}
