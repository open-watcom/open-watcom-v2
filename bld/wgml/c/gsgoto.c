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
* Description: implement ...label and .go script control words
*               not all options are used / implemented
*
*  comments are from script-tso.txt
****************************************************************************/


#include "wgml.h"


static char *get_labelname( const char *p, char *labelname )
{
    int     i;

    i = 0;
    while( is_label_char( *p ) ) {
        if( i < LABEL_NAME_LENGTH ) {
            labelname[i++] = my_toupper( *p );
        }
        p++;
    }
    labelname[i] = '\0';
    return( (char *)p );
}

/***************************************************************************/
/*  search for  label name in current input label control block            */
/***************************************************************************/

static labelcb  *find_label( const char *labelname )
{
    labelcb *   lb;

    if( input_cbs->fmflags & II_tag_mac ) {
        lb = input_cbs->s.m->mac->label_cb;
    } else {
        lb = input_cbs->s.f->label_cb;
    }
    for( ; lb != NULL; lb = lb->prev ) {
        if( strcmp( labelname, lb->labelname ) == 0 ) {
            return( lb );
        }
    }
    return( NULL );
}


/***************************************************************************/
/*  check whether current input line is the active go to target            */
/***************************************************************************/

bool        gotarget_reached( void )
{
    bool            reached;
    char            *p;
    char            labelname[LABEL_NAME_LENGTH + 1];

    reached = false;
    if( gotargetno > 0 ) {  // lineno search
        if( input_cbs->fmflags & II_tag_mac ) {
            reached = ( input_cbs->s.m->lineno == gotargetno );
        } else {
            reached = ( input_cbs->s.f->lineno == gotargetno );
        }
    } else {                // label search
        p = buff2;
        if( p[0] == '.' && p[1] == '.' && p[2] == '.' ) {// "..."
            p += 3;
            SkipSpaces( p );
            if( *p != '\0' ) {
                p = get_labelname( p, labelname );
                if( strcmp( gotarget, labelname ) == 0 && ((*p == ' ') || (*p == '\0')) ) {
                    reached = true;
                }
            }
        }
    }
    return( reached );
}


/***************************************************************************/
/*  check whether new label is duplicate                                   */
/***************************************************************************/

static  condcode    test_duplicate( char *name, line_number lineno )
{
    labelcb     *   lb;

    lb = find_label( name );
    if( lb == NULL ) {
        return( CC_omit );                 // really new label
    }
    if( lb->lineno == lineno ) {
        return( CC_pos );                  // name and lineno match
    } else {
        return( CC_neg );                   // name matches, different lineno
    }
}


/**************************************************************************/
/* ... (SET LABEL) defines an input line that has a "label".              */
/*                                                                        */
/*      +-------+--------------------------------------------------+      */
/*      |       |                                                  |      */
/*      |  ...  |    <label|n>  <line>                             |      */
/*      |       |                                                  |      */
/*      +-------+--------------------------------------------------+      */
/*                                                                        */
/* A blank is not required between the ... and the label.                 */
/*                                                                        */
/* ...label <line>:  Labels are used as "target" lines for the GO control */
/*    word.   A label may consist of  a maximum of eight characters,  and */
/*    must be unique within the input file, Macro,  or Remote in which it */
/*    is defined.    Labels are stored  internally by SCRIPT  entirely in */
/*    uppercase; therefore, "...hi" and "... HI" are identical.           */
/* ...n <line>:  Numeric  labels are used to verify that  the line really */
/*    is the "nth" record in the input file,  Macro,  or Remote.   If the */
/*    verification fails, SCRIPT generates an error message.              */
/*                                                                        */
/* This  control word  does not  cause a  break.   The  optional text  or */
/* control-word "line", starting one blank after the label,  is processed */
/* after the  label field is scanned.    If "line" is omitted,   no other */
/* action is  performed.   Control  words within  the "line"  operand may */
/* cause a break.                                                         */
/*                                                                        */
/* EXAMPLES                                                               */
/* (1) ...skip .sk 2                                                      */
/*     This defines a "SKIP" label on a "skip two" statement.             */
/* (2) ... 99 This had better be line ninety-nine.                        */
/*     This verifies that the line of text occurs in input line 99 of the */
/*     current input file, or results in an error message if not.         */
/**************************************************************************/

void    scr_label( void )
{
    condcode        cc;
    getnum_block    gn;
    labelcb     *   lb;
    char            linestr[NUM2STR_LENGTH + 1];
    char            labelname[LABEL_NAME_LENGTH + 1];

    SkipSpaces( g_scandata.s );       // may be ...LABEL or ...<blanks>LABEL, skip over blanks
    if( *g_scandata.s == '\0'  ) {    // no label?
        xx_source_err_exit_c( ERR_MISSING_NAME, "" );
        /* never return */
    }
    gn.arg = g_scandata;
    gn.ignore_blanks = false;
    cc = getnum( &gn );             // try numeric expression evaluation
    if( cc == CC_pos ) {               // numeric linenumber
        g_scandata.s = gn.arg.s;      // start for next token

        // check if lineno from label matches actual lineno

        if( input_cbs->fmflags & II_tag_mac ) {
            if( (unsigned)gn.result != input_cbs->s.m->lineno ) {
                xx_source_err_exit_c( ERR_LABEL_LINE, gn.resultstr );
                /* never return */
            }
        } else {
            if( (unsigned)gn.result != input_cbs->s.f->lineno ) {
                xx_source_err_exit_c( ERR_LABEL_LINE, gn.resultstr );
                /* never return */
            }
        }

        if( input_cbs->fmflags & II_tag_mac ) {
              // numeric macro label no need to store
        } else {
            sprintf( linestr, "%d", input_cbs->s.f->lineno );
            xx_warn_info_cc( WNG_LABEL_NUM, INF_FILE_LINE, linestr, input_cbs->s.f->filename );
        }

    } else {                        // no numeric label
        cc = getarg();
        if( cc == CC_pos ) {           // label name specefied
            char    *p;

            p = get_labelname( g_tok_start, labelname );
            if( p - g_tok_start >  LABEL_NAME_LENGTH ) {
                xx_source_err_exit_c( ERR_SYM_LONG, labelname );
                /* never return */
            }

            if( input_cbs->fmflags & II_tag_mac ) {

                cc = test_duplicate( labelname, input_cbs->s.m->lineno );
                if( cc == CC_pos ) {   // ok name and lineno match
                    // nothing to do
                } else {
                    if( cc == CC_neg ) {   // name with different lineno
                        xx_source_err_exit_c( ERR_LABEL_DUP, labelname );
                        /* never return */
                    }
                    // new label
                    lb              = mem_alloc( sizeof( labelcb ) );
                    memset( lb, 0, sizeof( labelcb ) );
                    lb->prev        = input_cbs->s.m->mac->label_cb;
                    input_cbs->s.m->mac->label_cb = lb;
                    lb->lineno      = input_cbs->s.m->lineno;
                    strcpy( lb->labelname, labelname );
                }
            } else {
                cc = test_duplicate( labelname, input_cbs->s.f->lineno );
                if( cc == CC_pos ) {   // ok name and lineno match
                    // nothing to do
                } else {
                    if( cc == CC_neg ) {   // name with different lineno
                        xx_source_err_exit_c( ERR_LABEL_DUP, labelname );
                        /* never return */
                    }
                    // new label
                    lb              = mem_alloc( sizeof( labelcb ) );
                    lb->prev        = input_cbs->s.f->label_cb;
                    input_cbs->s.f->label_cb = lb;
                    lb->pos         = input_cbs->s.f->pos;
                    lb->lineno      = input_cbs->s.f->lineno;
                    strcpy( lb->labelname, labelname );
                }
            }
        } else {
            xx_source_err_exit_c( ERR_MISSING_NAME, "" );
            /* never return */
        }
    }

    if( *g_scandata.s == ' ' ) {
        g_scandata.s++;               // skip one blank

        if( *g_scandata.s != '\0' ) {         // rest of line is not empty
            split_input( buff2, g_scandata.s, input_cbs->fmflags );   // split and process next
        }
    }
    scan_restart = g_scandata.e;
    return;
}


/***************************************************************************/
/* GOTO transfers processing  to the specified input line  in the current  */
/* file or macro.                                                          */
/*                                                                         */
/*      +-------+--------------------------------------------------+       */
/*      |       |                                                  |       */
/*      |  .GO  |    <label|n|+n|-n>                               |       */
/*      |       |                                                  |       */
/*      +-------+--------------------------------------------------+       */
/*                                                                         */
/*                                                                         */
/* <label>:   The specified  label  will be  converted  to uppercase  and  */
/*    processing will transfer  to the line defined  by the corresponding  */
/*    SET LABEL  (...)  control  word within  the current  input file  or  */
/*    Macro.                                                               */
/* <n|+n|-n>:  Alternatively,  an absolute line number or signed relative  */
/*    line number  may be specified.    In either case,   processing will  */
/*    continue at the specified line.                                      */
/*                                                                         */
/* This control word does not cause a break.  The transfer of control may  */
/* be forward or backward within a Macro and within an input file that is  */
/* on a  DASD device (Disk),   but may only be  forward in an  input file  */
/* being processed from a Unit Record device (Card Reader).   If the GOTO  */
/* control word is  used within an If  or Nested If,  SCRIPT  cannot know  */
/* where it will be positioned in any If structure.  After a GOTO, SCRIPT  */
/* will therefore assume  it is no longer  in the range of  any If struc-  */
/* ture.                                                                   */
/*                                                                         */
/* EXAMPLES                                                                */
/* (1) .if '&*1' eq DONE .go DONE                                          */
/*     Not finished yet.                                                   */
/*     ...DONE The End.                                                    */
/* (2) This example formats a produces a list of the first fifty numbers:  */
/*     .sr i=0                                                             */
/*     ...loop .sr i = &i + 1                                              */
/*     &i                                                                  */
/*     .if &i lt 50 .go loop                                               */
/*                                                                         */
/*                                                                         */
/* ! goto linenumber is not implemented for FILE, only for MACRO           */
/*                                                                         */
/*                                                                         */
/***************************************************************************/

void    scr_go( void )
{
    condcode        cc;
    getnum_block    gn;
    labelcb         *golb;
    char            labelname[LABEL_NAME_LENGTH + 1];

    input_cbs->if_cb->if_level = 0;     // .go terminates
    ProcFlags.keep_ifstate = false;     // ... all .if controls

    cc = getarg();
    if( cc != CC_pos ) {
        xx_source_err_exit_c( ERR_MISSING_NAME, "" );
        /* never return */
    }

    gn.arg.s = g_tok_start;
    gn.arg.e = g_scandata.e;
    gn.ignore_blanks = false;
    cc = getnum( &gn );             // try numeric expression evaluation
    if( cc == CC_pos  || cc == CC_neg) {     // numeric linenumber
        gotarget[0] = '\0';             // no target label name
        if( gn.num_sign == ' '  ) {     // absolute number
            gotargetno = gn.result;
        } else {
            if( input_cbs->fmflags & II_tag_mac ) {
                gotargetno = input_cbs->s.m->lineno;
            } else {
                gotargetno = input_cbs->s.f->lineno;
            }
            gotargetno += gn.result;    // relative target line number
        }

        if( gotargetno < 1 ) {
            xx_source_err_exit( ERR_LABEL_ZERO );
            /* never return */
        }
        if( input_cbs->fmflags & II_tag_mac ) {
            if( gotargetno <= input_cbs->s.m->lineno ) {
                input_cbs->s.m->lineno = 0; // restart from beginning
                input_cbs->s.m->macline = input_cbs->s.m->mac->macline;
            }
        }
    } else {                            // no numeric target label
        gotargetno = 0;                 // no target lineno known
        if( arg_flen >  LABEL_NAME_LENGTH ) {
            xx_source_err_exit_c( ERR_SYM_LONG, g_tok_start );
            /* never return */
        }
        get_labelname( g_tok_start, labelname );
        golb = find_label( labelname );
        if( golb != NULL ) {            // label already known
            *gotarget = '\0';
            gotargetno = golb->lineno;
            if( input_cbs->fmflags & II_tag_mac ) {
                if( golb->lineno <= input_cbs->s.m->lineno ) {
                    input_cbs->s.m->lineno = 0; // restart from beginning
                    input_cbs->s.m->macline = input_cbs->s.m->mac->macline;
                }
            } else {
                if( golb->lineno <= input_cbs->s.f->lineno ) {
                    fsetpos( input_cbs->s.f->fp, &golb->pos );
                    input_cbs->s.f->lineno = golb->lineno - 1;// position file
                }
            }
        } else {
            strcpy( gotarget, labelname );
        }
    }
    free_lines( input_cbs->hidden_head );   // delete split line
    input_cbs->hidden_head = NULL;
    input_cbs->hidden_tail = NULL;
    ProcFlags.goto_active = true;           // special goto processing
    scan_restart = g_scandata.e;
}


/***************************************************************************/
/*  print list of defined labels for macro or file                         */
/***************************************************************************/

void        print_labels( labelcb * lcb, char *name )
{
    char            fill[LABEL_NAME_LENGTH + 1];
    unsigned        len;

    if( lcb != NULL ) {
        memset( fill, ' ', LABEL_NAME_LENGTH );
        fill[LABEL_NAME_LENGTH] = '\0';
        out_msg( "\nList of defined labels for %s:\n\n", name );
        while( lcb != NULL ) {
            len = LABEL_NAME_LENGTH - strlen( lcb->labelname );
            out_msg( "Label='%s'%s at line %d\n", lcb->labelname, fill + len, lcb->lineno );
            lcb = lcb->prev;
        }
    }
}

