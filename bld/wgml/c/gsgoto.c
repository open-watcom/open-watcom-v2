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
* Description: implement ...label and .go script control words
*               not all options are used / implemented
*
*  comments are from script-tso.txt
****************************************************************************/

#include "wgml.h"
#include "gvars.h"

#include "clibext.h"


/***************************************************************************/
/*  search for  label name in current input label control block            */
/***************************************************************************/

static  labelcb *   find_label( char    *   name )
{
    labelcb *   lb;

    if( input_cbs->fmflags & II_tag_mac ) {
        lb = input_cbs->s.m->mac->label_cb;
    } else {
        lb = input_cbs->s.f->label_cb;
    }
    for( ; lb != NULL; lb = lb->prev ) {
        if( !strncmp( name, lb->label_name, MAC_NAME_LENGTH ) ) {
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
    bool        reached;
    char    *   p;
    int         k;

    reached = false;
    if( gotargetno > 0 ) {              // lineno search
        if( input_cbs->fmflags & II_tag_mac ) {
            reached = ( input_cbs->s.m->lineno == gotargetno );
        } else {
            reached = ( input_cbs->s.f->lineno == gotargetno );
        }
    } else {                            // label name search
        if( (*buff2 == *(buff2 + 1)) && (*buff2 == *(buff2 + 2)) ) {// "..."
            p = buff2 + 3;
            while( *p == ' ' ) {
                p++;
            }
            if( *p != '\0' ) {
                k = 0;
                while( gotarget[k] && *p == gotarget[k] ) {
                    k++;
                    p++;
                }
                if( gotarget[k] == '\0' && ((*p == ' ') || (*p == '\0')) ) {
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
        return( omit );                 // really new label
    }
    if( lb->lineno == lineno ) {
        return( pos );                  // name and lineno match
    } else {
        return( neg);                   // name matches, different lineno
    }
}


/**************************************************************************/
/* ... (SET LABEL) defines an input line that has a "label".              */
/*                                                                        */
/*      旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴커      */
/*      |       |                                                  |      */
/*      |  ...  |    <label|n>  <line>                             |      */
/*      |       |                                                  |      */
/*      읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴켸      */
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
    char            linestr[MAX_L_AS_STR];

    scan_start += 2;                    // over dots

    while( *scan_start == ' ' ) {       // may be ...LABEL or ...      LABEL
        scan_start++;                   // over blanks
    }
    if( *scan_start == '\0'  ) {        // no label?
        scan_err = true;
        err_count++;
        g_err( err_missing_name, "" );
        g_info_inp_pos();
        show_include_stack();
        return;
    } else {

        gn.argstart = scan_start;
        gn.argstop = scan_stop;
        gn.ignore_blanks = 0;

        cc = getnum( &gn );             // try numeric expression evaluation
        if( cc == pos ) {               // numeric linenumber

            scan_start = gn.argstart;   // start for next token

            // check if lineno from label matches actual lineno

            if( (input_cbs->fmflags & II_tag_mac) && gn.result != input_cbs->s.m->lineno
              || (input_cbs->fmflags & II_tag_mac) == 0 && gn.result != input_cbs->s.f->lineno ) {
                scan_err = true;
                err_count++;
                g_err( err_label_line, gn.resultstr );
                g_info_inp_pos();
                show_include_stack();
                return;
            }

            if( input_cbs->fmflags & II_tag_mac ) {
                // numeric macro label no need to store
            } else {
                wng_count++;
                g_warn( wng_label_num );
                ultoa( input_cbs->s.f->lineno, linestr, 10 );
                g_info( inf_file_line, linestr, input_cbs->s.f->filename );
                show_include_stack();
            }

        } else {                        // no numeric label
            cc = getarg();
            if( cc == pos ) {           // label name specefied
                char    *   p;
                char    *   pt;
                int         len;

                p   = tok_start;
                pt  = token_buf;
                len = 0;
                while( len < arg_flen ) {   // copy to buffer
                    *pt++ = *p++;
                    len++;
                }
                *pt = '\0';
                if( len >  MAC_NAME_LENGTH ) {
                    err_count++;
                    g_err( err_sym_long, token_buf );
                    ultoa( input_cbs->s.f->lineno, linestr, 10 );
                    g_info( inf_file_line, linestr, input_cbs->s.f->filename );
                    show_include_stack();
                    token_buf[MAC_NAME_LENGTH] = '\0';
                }

                if( input_cbs->fmflags & II_tag_mac ) {

                    cc = test_duplicate( token_buf, input_cbs->s.m->lineno );
                    if( cc == pos ) {   // ok name and lineno match
                        // nothing to do
                    } else {
                        if( cc == neg ) {   // name with different lineno
                            scan_err = true;
                            err_count++;
                            g_err( err_label_dup, token_buf );
                            ultoa( input_cbs->s.m->lineno, linestr, 10 );
                            g_info( inf_mac_line, linestr,
                                     input_cbs->s.m->mac->name );
                            show_include_stack();
                            return;
                        } else {        // new label
                            lb              = mem_alloc( sizeof( labelcb ) );
                            lb->prev        = input_cbs->s.m->mac->label_cb;
                            input_cbs->s.m->mac->label_cb = lb;
                            memset( &lb->pos, 0, sizeof( lb->pos ) );
                            lb->lineno      = input_cbs->s.m->lineno;
                            strcpy( lb->label_name, token_buf );
                        }
                    }
                } else {
                    cc = test_duplicate( token_buf, input_cbs->s.f->lineno );
                    if( cc == pos ) {   // ok name and lineno match
                        // nothing to do
                    } else {
                        if( cc == neg ) {   // name with different lineno
                            scan_err = true;
                            err_count++;
                            g_err( err_label_dup, token_buf );
                            ultoa( input_cbs->s.f->lineno, linestr, 10 );
                            g_info( inf_file_line, linestr, input_cbs->s.f->filename );
                            show_include_stack();
                            return;
                        } else {        // new label

                            lb              = mem_alloc( sizeof( labelcb ) );
                            lb->prev        = input_cbs->s.f->label_cb;
                            input_cbs->s.f->label_cb = lb;
                            lb->pos         = input_cbs->s.f->pos;
                            lb->lineno      = input_cbs->s.f->lineno;
                            strcpy( lb->label_name, token_buf );
                        }
                    }
                }
            } else {
                scan_err = true;
                err_count++;
                g_err( err_missing_name, "" );
                g_info_inp_pos();
                show_include_stack();
                return;
            }
        }

        if( *scan_start == ' ' ) {
            scan_start++;               // skip one blank

            if( *scan_start ) {         // rest of line is not empty
                split_input( buff2, scan_start, false );// split and process next
            }
        }
        scan_restart = scan_stop;
        return;
    }
}



/***************************************************************************/
/* GOTO transfers processing  to the specified input line  in the current  */
/* file or macro.                                                          */
/*                                                                         */
/*      旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴커       */
/*      |       |                                                  |       */
/*      |  .GO  |    <label|n|+n|-n>                               |       */
/*      |       |                                                  |       */
/*      읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴켸       */
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
    labelcb     *   golb;
    int             k;

    input_cbs->if_cb->if_level = 0;     // .go terminates
    ProcFlags.keep_ifstate = false;     // ... all .if controls

    garginit();

    cc = getarg();
    if( cc != pos ) {
        scan_err = true;
        err_count++;
        g_err( err_missing_name, "" );
        g_info_inp_pos();
        show_include_stack();
        return;
    }

    gn.argstart = tok_start;
    gn.argstop = scan_stop;
    gn.ignore_blanks = 0;

    cc = getnum( &gn );             // try numeric expression evaluation
    if( cc == pos  || cc  == neg) {     // numeric linenumber
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
            scan_err = true;
            err_count++;
            g_err( err_label_zero );
            g_info_inp_pos();
            show_include_stack();
            return;
        }
        if( input_cbs->fmflags & II_tag_mac ) {
            if( gotargetno <= input_cbs->s.m->lineno ) {
                input_cbs->s.m->lineno = 0; // restart from beginning
                input_cbs->s.m->macline = input_cbs->s.m->mac->macline;
            }
        }
    } else {                            // no numeric target label

        gotargetno = 0;                 // no target lineno known
        if( arg_flen >  MAC_NAME_LENGTH ) {
            err_count++;
            g_err( err_sym_long, tok_start );
            g_info_inp_pos();
            show_include_stack();
            arg_flen = MAC_NAME_LENGTH;
        }

        for( k = 0; k < MAC_NAME_LENGTH; k++ ) {// copy to work
            gotarget[k] = *tok_start++;
        }
        gotarget[k] = '\0';

        golb = find_label( gotarget );
        if( golb != NULL ) {            // label already known
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
        }
    }
    free_lines( input_cbs->hidden_head );   // delete split line
    input_cbs->hidden_head = NULL;
    input_cbs->hidden_tail = NULL;
    ProcFlags.goto_active = true;       // special goto processing
    scan_restart = scan_stop;

}


/***************************************************************************/
/*  print list of defined labels for macro or file                         */
/***************************************************************************/

void        print_labels( labelcb * lcb, char * name )
{
    static const char   fill[10] = "         ";
    size_t              len;
    labelcb         *   lb;

    lb = lcb;
    if( lb != NULL ) {
        out_msg( "\nList of defined labels for %s:\n\n", name);
        while( lb != NULL ) {
            len = strlen( lb->label_name );
            out_msg( "Label='%s'%s at line %d\n", lb->label_name, &fill[len], lb->lineno );
            lb = lb->prev;
        }
    }
}

