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
* Description:  WGML top level driver module and file I/O.
*
*   some logic / ideas adopted from Watcom Script 3.2 IBM S/360 Assembler
*   as found on http://www.cbttape.org/c249down.htm   files 280 - 288
*
*   Several comments in the source files are taken from the existing
*   documentation files for older versions:
*       scripttso.txt for SCRIPT control words and system variables
*       wgmlref.pdf   for WGML tags
****************************************************************************/


#if defined( __UNIX__ ) || defined( __WATCOMC__ )
#include <unistd.h>
#else
#include <io.h>
#endif

#include "wgml.h"
#include "findfile.h"
#include "gvars.h"
#include "banner.h"

#include "clibext.h"

/***************************************************************************/
/*  Usage info and end program                                             */
/***************************************************************************/

static void usage( void )
{
    int     k;
    int     kscreen;

    g_banner();

    kscreen = 0;
    for( k = inf_use_start; k <= inf_use_end; k++ ) {
        g_info_lm( k );
        if( isatty( fileno( stdin ) ) ) {
            if( kscreen == 22 ) {
                kscreen = 0;
                g_info_lm( inf_pause );
                getchar();
            } else {
                kscreen++;
            }
        }
    }
    my_exit( 4 );
}

#if 0                                   // not used for the moment
char *get_filename_full_path( char *buff, char const * name, size_t max )
{
    char    *   p;

    p = _fullpath( buff, name, max );
    if( p == NULL ) {
        p = (char *)name;
    }

#ifdef __UNIX__
    if( (p[0] == '/' && p[1] == '/') && (name[0] != '/' || name[1] != '/') ) {
        //
        // if the _fullpath result has a node number and
        // the user didn't specify one, strip the node number
        // off before returning
        //
        p += 2;
        while( *(p++) != '/' ) ;
    }
#endif
    return( p );
}
#endif


/***************************************************************************/
/*  Set the extension of the Master input file as default extension        */
/***************************************************************************/

static  void    set_default_extension( const char * masterfname )
{
    char        buff[FILENAME_MAX];
    char    *   ext;

    _splitpath2( masterfname, buff, NULL, NULL, NULL, &ext );
    if( strlen( ext ) > 0) {
        if( strlen( ext ) > strlen( def_ext ) ) {
            mem_free( def_ext);
            def_ext = mem_alloc( 1 + strlen( ext ) );
        }
        strcpy( def_ext, ext );
    }
    return;
}


/***************************************************************************/
/*  free input filenames                                                   */
/***************************************************************************/

static  void    free_filenames( void )
{
    fnstack * wk;
    fnstack * wk1;
    int32_t cnt;

    wk = fn_stack;
    if( GlobalFlags.statistics ) {
        cnt = 0;
        out_msg( "\nInput filenames:\n" );
    }
    while( wk != NULL ) {
        if( GlobalFlags.statistics ) {
            out_msg( "    %s\n", wk->fn );
            cnt++;
        }
        wk1 = wk->prev;
        mem_free( wk );
        wk = wk1;
    }
    if( GlobalFlags.statistics ) {
        out_msg( "Total files: %ld\n\n", cnt );
    }
    fn_stack = NULL;
    return;
}


/***************************************************************************/
/*  construct input filename stack. It is only freed at program end        */
/***************************************************************************/

static  char    * reuse_filename( const char * fn )
{
    fnstack * fnwk;

    for( fnwk = fn_stack; fnwk != NULL; fnwk = fnwk->prev ) {
        if( !strcmp( fnwk->fn, fn ) ) {
            return( fnwk->fn );         // filename is known
        }
    }

    fnwk = mem_alloc( sizeof( fnstack ) + strlen( fn ) );
    strcpy( fnwk->fn, fn );

    fnwk->prev = fn_stack;
    fn_stack = fnwk;
    return( fnwk->fn );
}


/***************************************************************************/
/*  add info about file  to LIFO list                                      */
/***************************************************************************/

static  void    add_file_cb_entry( void )
{
    filecb  *   new;
    inputcb *   nip;

    new = mem_alloc( sizeof( filecb ) );
    new->filename = reuse_filename( try_file_name );
    mem_free( try_file_name );
    try_file_name = NULL;

    nip = mem_alloc( sizeof( inputcb ) );
    nip->hidden_head = NULL;
    nip->hidden_tail = NULL;
    nip->if_cb       = mem_alloc( sizeof( ifcb ) );
    memset( nip->if_cb, '\0', sizeof( ifcb ) );
    nip->pe_cb.line = NULL;
    nip->pe_cb.count = 0;
    nip->fmflags = II_file;
    nip->s.f     = new;
    init_dict( &nip->local_dict );

    new->lineno   = 0;
    new->linemin  = line_from;
    new->linemax  = line_to;
    new->label_cb = NULL;

    if( try_fp ) {
        new->flags = FF_open;
        new->fp    = try_fp;
        try_fp     = NULL;
    } else {
        new->flags = FF_clear;
        new->fp    = NULL;
    }

    nip->prev = input_cbs;
    input_cbs = nip;
    return;
}


/***************************************************************************/
/*  remove info about file or macro  from LIFO list                        */
/***************************************************************************/

static  void    del_input_cb_entry( void )
{
    inputcb     *   wk;
    labelcb     *   lw;

    wk = input_cbs;
    if( wk == NULL ) {
        return;
    }
    free_lines( wk->hidden_head );

    free_dict( &wk->local_dict );
    if( wk->if_cb != NULL ) {
//      if( wk->if_cb->if_level > 0 ) {
//          char linestr[MAX_L_AS_STR];
//          utoa( wk->if_cb->if_level, linestr, 10 );
//          g_err( err_if_level, linestr );
//          show_include_stack();
//          err_count++;
//      }
        mem_free( wk->if_cb );
    }
    if( wk->pe_cb.line != NULL ) {
        mem_free( wk->pe_cb.line );
    }

    if( wk->fmflags & II_tag_mac ) {
/*
 *  The macrolines in s.m don't need to be freed, as these point to
 *  mac_entry, and freeing is done with macro_dict
 */
        mem_free( wk->s.m );
    } else {
        if( wk->s.f->flags & FF_open ) {// close file if neccessary
            fclose( wk->s.f->fp );
        }
        lw = wk->s.f->label_cb;
        if( GlobalFlags.research ) {
            print_labels( lw, wk->s.f->filename );  // print labels
        }
        while( lw != NULL ) {
           wk->s.f->label_cb = lw->prev;
           mem_free( lw );              // free labels
           lw = wk->s.f->label_cb;
        }
        mem_free( wk->s.f );
    }
    input_cbs = wk->prev;
    mem_free( wk );
    return;
}


/***************************************************************************/
/* remove leading  .  from input                                           */
/* special processing for   .:tag  construct                               */
/***************************************************************************/

static void remove_indentation( void )
{
    char    *   p;
    char    *   pb;
    int         offset;

    p = buff2;
    while( *p == SCR_char && *(p + 1) == ' ' ) {
        while( *++p == ' ' ) /* empty */ ;  // skip blanks
    }
    if( *p == SCR_char && *(p + 1) == GML_char ) {
        p++;                            // skip SCR_char
    }
    if( p != buff2 ) {                  // found some blanks now copy buffer

        offset = p - buff2;
        pb = buff2;
        while( *p ) {
            *pb++ = *p++;
        }
        if( offset > 0 ) {
            memset( pb, '\0', offset ); // clear rest
        }
//        buff2_lg = strlen( buff2 );
//        if( GlobalFlags.research && GlobalFlags.firstpass ) {
//            g_info( INF_INDENT_REM, buff2 );
//        }
    }
}


/***************************************************************************/
/*  test_macro_xxxx test for  special processing within false branch of    */
/* .if control word                                                        */
/*  ProcFlags.in_macro_define is abused as switch as no                    */
/*  'real' macro definition is possible in this case                       */
/*                                                                         */
/* .dm macname begin                                                       */
/*   all lines in between are ignored without any processing               */
/* .dm macname end                                                         */
/***************************************************************************/

static bool test_macro_xxxx( char const * beginend )
{
    char        cw[3];
    char        c;
    char    *   p;

    if( *buff2 == SCR_char ) {// only test script control words

        p = buff2 + 1;
        if( (*p == SCR_char)  || (*p == '\'') ) {
            p++;
        }
        cw[0] = tolower( *p++ );
        cw[1] = tolower( *p++ );
        c = *p++;

        cw[2] = '\0';
        if( c == '\0' || c == ' ' ) {
            if( !strcmp( cw, "dm" ) ) {
                while( *p && *p == ' ' ) {  // find macroname
                    p++;
                }
                while( *p && *p != ' ' ) {
                    p++;
                }
                while( *p && *p == ' ' ) {  // find begin end
                    p++;
                }
                return(  !strnicmp( p, beginend, strlen( beginend ) ) );
            }
        }
    }
    return(false );
}


/***************************************************************************/
/*  test for comment     .cm :cmt                                          */
/*  returns true if  comment found                                         */
/*   .* comment returns false                                              */
/***************************************************************************/

static  bool    test_comment( void )
{

    if( *buff2 == SCR_char ) {          // test for .*  .cm
//      if( ( *(buff2 + 1) == '*') ||    TBD
        if(
                ( (tolower( *(buff2 + 1) ) == 'c') &&
                  (tolower( *(buff2 + 2) ) == 'm') &&
                  (*(buff2+3) == ' ')
                )
        ) {
           return( true );
        }
    } else {                            // test for :cmt
        if( (*buff2 == GML_char) &&
            (tolower( *(buff2 + 1) ) == 'c') &&
            (tolower( *(buff2 + 2) ) == 'm') &&
            (tolower( *(buff2 + 3) ) == 't')
        ) {
            if( (*(buff2 + 4) == ' ') ||
                (*(buff2 + 4) == '.')  ) {

                if( ProcFlags.literal ) {   // special
                    if( li_cnt < LONG_MAX ) {// we decrement, do not wait for .li OFF
                        if( li_cnt-- <= 0 ) {
                            ProcFlags.literal = false;
                        }
                    }
                }
                return( true );
            }
        }
    }
    return( false );
}

/***************************************************************************/
/*  process the input file                                                 */
/***************************************************************************/

static  void    proc_input( char * filename )
{
    ifcb        *   ic;
    filecb      *   cb;
    laystack    *   curr_lay_file;
    char            attrwork[32];
    ifcb            ic_work;
    condcode        cc;

    ProcFlags.newLevelFile = 1;
    strcpy( token_buf, filename );

    for( ; ; ) {                        // as long as there is input
        if( ProcFlags.newLevelFile ) {
            ProcFlags.newLevelFile = 0; // start a new include FILE level

            /***************************************************************/
            /*  split off attribute  (f:xxxx)                              */
            /***************************************************************/
            split_attr_file( token_buf, attrwork, sizeof( attrwork ) );

            if( attrwork[0] ) {
                g_warn( wng_fileattr_ignored, attrwork, token_buf );
                wng_count++;
            }
            if( search_file_in_dirs( token_buf, def_ext, alt_ext, ds_doc_spec ) ) {

                if( inc_level >= MAX_INC_DEPTH ) {
                    g_err( err_max_input_nesting, token_buf );
                    err_count++;
                    show_include_stack();
                    continue;           // don't start new include level
                }
            } else {
                g_err( err_input_file_not_found, token_buf );
                err_count++;
                if( inc_level > 0 ) {
                    show_include_stack();
                    continue;           // don't start new include level
                } else {                // master file included from cmdline
                    g_info( inf_included, "cmdline" );
                    break;              // no input file leave loop
                }
            }
            inc_inc_level();            // record max include level
            add_file_cb_entry();
            cb = input_cbs->s.f;
            cb->flags |= FF_crlf;       // delete crlf at end
            if( attrwork[0] ) {
                strcpy( cb->fileattr, attrwork );
            } else {
                cb->fileattr[0] = '\0';
            }
            if( GlobalFlags.inclist ) {
                g_info_lm( inf_curr_input, "file", cb->filename );
            }

            /***************************************************************/
            /*  If ( LAYOUT file option specified, then process            */
            /*  layout file(s)   before master file                        */
            /***************************************************************/

//            if( (lay_files != NULL) && (inc_level == 1) && (cb->lineno == 0) ) {
            if( (lay_files != NULL) && (inc_level == 1) ) {
                curr_lay_file = lay_files;
                strcpy( token_buf, lay_files->layfn );
                lay_files = curr_lay_file->next;
                mem_free( curr_lay_file );
                ProcFlags.newLevelFile = 1; // start a new include FILE level
                continue;               // with cmdline    layout option file
            }
        } // new include FILE processing

        if( inc_level == 0 ) {
            break;                 // we are done (master document not found)
        }


        /*******************************************************************/
        /*  process an input file / macro                                  */
        /*******************************************************************/

        while( !(input_cbs->fmflags & II_eof) ) {

            ic = input_cbs->if_cb;      // .if .th .el controlblock

            if( GlobalFlags.firstpass && input_cbs->fmflags & II_research ) {
                show_ifcb( "procin 1", ic );
            }

            if( !get_line( true ) ) {
                if( ProcFlags.goto_active ) {   // goto active at EOF
                    char    linestr[MAX_L_AS_STR];

                    ProcFlags.goto_active = false;
                    err_count++;
                    if( input_cbs->fmflags & II_tag_mac ) {
                        if( gotargetno > 0 ) {
                            utoa( gotargetno, linestr, 10 );
                            g_err( err_goto, linestr,
                                   input_cbs->s.m->mac->name );
                        } else {
                            g_err( err_goto, gotarget,
                                   input_cbs->s.m->mac->name );
                        }
                    } else {
                        g_err( err_goto, gotarget, input_cbs->s.f->filename );
                    }
                    show_include_stack();
                }
                break;                  // EOF
            }
#if 0
            if( (buff2_lg < 1) && !ProcFlags.concat) {
                ProcFlags.empty_doc_el = true;
                scr_process_break();
                continue;               // minimal processing for empty line
            }
#endif
            remove_indentation();       // ".  .  .  .cw"  becomes ".cw"

            if( ProcFlags.goto_active ) {
                if( !gotarget_reached() ) {
                    continue;           // skip processing
                }
                ProcFlags.goto_active = false;
            }

            if( test_comment() ) {      // minimal processing for comment
                continue;
            }
            if( !ProcFlags.keep_ifstate ) {
                if( ic->if_level > 0 ) {// if .if active
                    if( ic->if_flags[ic->if_level].ifelse // after else
                        && !ic->if_flags[ic->if_level].ifdo ) {// no do group

                        ic->if_level--; // pop .if stack one level

                        if( GlobalFlags.firstpass &&
                            input_cbs->fmflags & II_research ) {
                            show_ifcb( "procin -1", ic );
                        }
                    }
                }
                if( !ic->if_flags[ic->if_level].ifdo ) {  // no do group

                    ic->if_flags[ic->if_level].ifthen = false;// not in then
                    ic->if_flags[ic->if_level].ifelse = false;// not in else

                    if( GlobalFlags.firstpass &&
                        input_cbs->fmflags & II_research ) {
                        show_ifcb( "procin 2", ic );
                    }
                }
            } else {
                ProcFlags.keep_ifstate = false;
            }

            /***************************************************************/
            /*  suppress some processing for line to be skipped            */
            /*  and not .if .th .el .do control line                       */
            /*  special handling for define macro inside false branch      */
            /*  (ignore all up to .dm end)                                 */
            /***************************************************************/

            if( !ProcFlags.literal ) {
                ic_work = *ic;          // create a copy of if control block

                if( ProcFlags.in_macro_define ) {
                    if( test_macro_xxxx( "end" ) ) {
                        ProcFlags.in_macro_define = false;
                    }
                }
                if( ProcFlags.in_macro_define ) {
                    if( input_cbs->fmflags & II_research && GlobalFlags.firstpass ) {
                        g_info_lm( inf_skip_line );
                    }
                    continue;           // skip processing
                }

                set_if_then_do( &ic_work );
                cc = test_process( &ic_work );
                if( cc != pos ) {
                    if( test_macro_xxxx( "begin" ) ) {
                        ProcFlags.in_macro_define = true;
                    }
                    if( input_cbs->fmflags & II_research &&
                        GlobalFlags.firstpass ) {
                        g_info_lm( inf_skip_line );
                    }
                    set_if_then_do( ic );
                    test_process( ic );

                    continue;           // skip processing
                }
            }

            process_line();             // substitute variables + functions
            scan_line();

            if( ProcFlags.newLevelFile ) {
                break;            // imbed and friends found, start new level
            }
        }
        if( ProcFlags.newLevelFile ) {  // include / imbed new file
            continue;
        }

        if( inc_level == 1 ) {          // EOF for master file end
            last_page_out();            // forces final page(s) out
            /***************************************************************/
            /*  Test for missing eXXX tag                                  */
            /***************************************************************/
            if( (nest_cb != NULL) && (nest_cb->c_tag != t_NONE) ) {
                g_err_tag_nest( str_tags[nest_cb->c_tag + 1] );// eXXX expected
            }
        }
        del_input_cb_entry();           // one level finished
        inc_level--;
        if( inc_level == 0 ) {          // EOF for master document file
            break;
        }
        if( lay_files != NULL ) {   // any more  LAYfiles
            curr_lay_file = lay_files;
            strcpy( token_buf, lay_files->layfn );
            lay_files = curr_lay_file->next;
            mem_free( curr_lay_file );
            ProcFlags.newLevelFile = 1; // start a new include file level
            continue;                   // with cmdline layout option file
        }
        if( input_cbs->fmflags & II_file ) {
            if( GlobalFlags.inclist ) {
                char    linestr[MAX_L_AS_STR];

                cb = input_cbs->s.f;
                ultoa( cb->lineno, linestr, 10 );
                g_info_lm( inf_curr_line, cb->filename, linestr );
            }
        } else {
            if( GlobalFlags.inclist ) {
                g_info_lm( inf_curr_input, "macro", input_cbs->s.m->mac->name );
            }
        }
    }                                   // for loop
}


/***************************************************************************/
/*  printStats show statistics at program end                              */
/***************************************************************************/

static  void    print_stats( clock_t duration_ticks )
{
    char            linestr[30];
    char            linestr2[30];
    ldiv_t          hour_min;
    ldiv_t          sec_frac;
    unsigned long   peak;

    g_info_lm( inf_stat_0 );

    utoa( pass, linestr, 10 );
    if( pass == passes ) {
        linestr2[0] = '\0';
    } else {
        strcpy( linestr2, "of " );
        utoa( passes, linestr2 + 3, 10 );
    }
    g_info_lm( inf_stat_1, linestr, linestr2 );

    utoa( max_inc_level, linestr, 10 );
    g_info_lm( inf_stat_2, linestr );

    utoa( err_count, linestr, 10 );
    g_info_lm( inf_stat_3, linestr );

    utoa( wng_count, linestr, 10 );
    g_info_lm( inf_stat_4, linestr );

    utoa( err_count ? 8 : wng_count ? 4 : 0, linestr, 10 );
    g_info_lm( inf_stat_5, linestr );

    peak = mem_get_peak_usage();
    if( peak ) {
        ultoa( peak, linestr, 10 );
        g_info_lm( inf_stat_6, linestr );
    }

    // convert duration from clock ticks to HH:MM:SS.hh
    hour_min = ldiv( duration_ticks / CLOCKS_PER_SEC / 60L, 60L );
    sec_frac  = ldiv( duration_ticks, CLOCKS_PER_SEC );
    sprintf( linestr, "%02lu:%02lu:%02lu.%02lu",
        hour_min.quot, hour_min.rem, sec_frac.quot % 60, sec_frac.rem / 10 );
    g_info_lm( inf_stat_7, linestr );
}


/***************************************************************************/
/*  initPass                                                               */
/***************************************************************************/
static  void    init_pass( void )
{

    init_pass_data();                   // (re)set processing flags + vars

    if( GlobalFlags.research && (research_to > 0) ) {
        if( research_file_name[0] == '\0' ) {
            strcpy( research_file_name, master_fname );
        }
        ProcFlags.researchfile = true;
    }

    if( pass > 1 ) {
        GlobalFlags.firstpass = 0;

/*
 * design question: free dictionaries or not                            TBD
 *                  setsymbol defines from cmdline must not be deleted
 */

        reset_auto_inc_dict( global_dict );// let auto inc start with 1 again

//      free_dict( &global_dict );      // free dictionaries
//      free_macro_dict( &macro_dict );
        free_tag_dict( &tag_dict );
        init_nest_cb();
    } else {
        GlobalFlags.firstpass = 1;
    }
    if( pass < passes ) {
        GlobalFlags.lastpass = 0;
    } else {
        GlobalFlags.lastpass = 1;
    }

    line_from   = 1;                  // processing line range Masterdocument
    line_to     = LINENO_MAX;

    apage               = 0;            // absolute pageno 1 - n
    page                = 0;            // current pageno (in body 1 - n)
    line                = 0;            // current output lineno on page
    lc                  = 0;            // remaining lines on page
    post_space          = 0;

    init_tag_att();                     // reset last defined GML tag

}

/***************************************************************************/
/*  main WGML                                                              */
/***************************************************************************/

int main( int argc, char * argv[] )
{
    char    *   cmdline;
    int         cmdlen;
    jmp_buf     env;
    int         tok_count;
    int         passcount;
    clock_t     start_time;
    clock_t     end_time;

    argc = argc; argv = argv;
    environment = &env;
    if( setjmp( env ) ) {               // if fatal error has occurred
        my_exit( 16 );
    }

    start_time = clock();               // remember start time
    passcount = 0;
    mem_init();                         // init memory tracker if necessary

    init_global_vars();

    token_buf = mem_alloc( buf_size );

    // out_msg( "define enum %d %d\n", INF_CMDLINE, inf_cmdline );

    init_msgs();                        // init msg resources

    ff_setup();                         // init findfile

    cmdlen = _bgetcmd( NULL, 0 ) + 1;
    cmdline = mem_alloc( cmdlen );
    _bgetcmd( cmdline, cmdlen );

    g_info_lm( INF_CMDLINE, cmdline );

    tok_count = proc_options( cmdline );
    init_sysparm( cmdline, banner1w( "Script/GML", _WGML_VERSION_ ) );
    /* don't mem_free cmdline now as it is used for sysparm variable */
    g_banner();
    if( tok_count < 4 ) {               // file ( device xyz   is minimum
        usage();                        // display usage and exit
    }
    cop_setup();                        // init copfiles

    if( master_fname != NULL ) {        // filename specified
        int     rc;
        symsub  * passnoval;
        symsub  * passofval;

        rc = find_symvar( &sys_dict, "$passof", no_subscript, &passofval );
        rc = find_symvar( &sys_dict, "$passno", no_subscript, &passnoval );
        utoa( passes, passofval->value, 10 );   // fill no of passes

        set_default_extension( master_fname );// make this extension first choice

        init_def_lay();                 // set default layout values

        fb_start();                     // START :PAUSE & :INIT processing.

        if( (GlobalFlags.inclist || GlobalFlags.statistics ||
             GlobalFlags.research ) && (lay_files != NULL) ) {

            laystack *lwk = lay_files;

            out_msg( "\nLAYOUT file(s) specified on cmdline:\n" );
            while( lwk != NULL ) {
                out_msg( "\t%s\n", lwk->layfn );
                lwk = lwk->next;
            }
        }

        for( pass = 1; pass <= passes; pass++ ) {

            init_pass();
            utoa( pass, passnoval->value, 10 ); // fill current passno

            g_info_lm( INF_PASS_1, passnoval->value, passofval->value,
                    GlobalFlags.research ? "research" : "normal" );
//          if( GlobalFlags.research ) {
//              mem_prt_curr_usage();
//          }

            proc_input( master_fname );

            while( nest_cb != NULL ) {
                tag_cb  *   cb = nest_cb->prev;

                add_tag_cb_to_pool( nest_cb );
                nest_cb = cb;
            }
            if( GlobalFlags.research && (pass < passes) ) {
                print_sym_dict( global_dict );
            }
            msg_indent = 0;
            g_info_lm( INF_PASS_2, passnoval->value, passofval->value,
                    GlobalFlags.research ? "research" : "normal" );

//          if( GlobalFlags.research && (pass < passes) ) {
//              mem_prt_curr_usage();
//          }
            passcount = pass;
            if( !GlobalFlags.lastpass && (err_count > 0) ) {
                g_info_lm( inf_error_stop, passes - pass > 1 ? "es" : "" );

                ixdump( index_dict );   // test show unformatted index TBD

                break;                  // errors found stop now
            }
        }

        fb_finish();                    // :FINISH block processing.

    } else {
        g_err( err_missing_mainfilename );
        err_count++;
        usage();
    }
    if( GlobalFlags.research ) {
        print_GML_tags_research();
        free_GML_tags_research();

        print_SCR_tags_research();
        free_SCR_tags_research();

        print_macro_dict( macro_dict, true );
        print_ref_dict( ref_dict, "HDREF" );
        print_ref_dict( fig_dict, "FIGREF" );
        print_ref_dict( fn_dict, "FNREF" );
        print_ref_dict( iref_dict, "INDEX" );

        if( tag_dict != NULL ) {
            print_tag_dict( tag_dict );
        }
        print_single_funcs_research();
        free_single_funcs_research();

        print_multi_funcs_research();
        free_multi_funcs_research();

        if( global_dict != NULL ) {
            print_sym_dict( global_dict );
        }
        print_sym_dict( sys_dict );
    }

    close_all_pu_files();

    ff_teardown();                      // free memory allocated in findfunc
    cop_teardown();                     // free memory allocated in copfiles

    mem_free( cmdline );
    free_some_mem();
    free_filenames();

    end_time = clock();                 // get end time
    pass = passcount;
    print_stats( end_time - start_time );

    fini_msgs();                        // end of msg resources, no more msgs built from
                                        // resources possible after this point
    mem_fini();

/// test
//    for( ; ; );

/// test

    my_exit( err_count ? 8 : wng_count ? 4 : 0 );
    return( 0 );                    // never reached, but makes compiler happy
}
