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
* Description:  WGML global include header.
*
****************************************************************************/

#ifndef WGML_H_INCLUDED
#define WGML_H_INCLUDED

#ifndef __STDC_WANT_LIB_EXT1__
    #define __STDC_WANT_LIB_EXT1__  1   /* use safer C library             */
#endif

#include <stdio.h>
#include <setjmp.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <ctype.h>
#include <limits.h>
#if defined( __WATCOMC__ ) || !defined( __UNIX__ )
#include <process.h>
#endif
#include "bool.h"
#include "clibext.h"

#include "copfiles.h"       // mostly for access to bin_device & wgml_fonts
#include "gtype.h"
#include "gtypelay.h"


#ifdef  __cplusplus
extern "C" {    /* Use "C" linkage when in C++ mode */
#endif


//================= Function Prototypes ========================

/* copfiles.c                           */
extern uint8_t      cop_in_trans( uint8_t in_char, font_number font );
extern void         cop_setup( void );
extern void         cop_teardown( void );
extern uint32_t     cop_text_width( char *text, uint32_t count, font_number font );
extern void         cop_ti_table( char * p );
extern void         fb_dbox( dbox_element * in_dbox );
extern void         fb_document( void );
extern void         fb_document_page( void );
extern void         fb_finish( void );
extern void         fb_hline( hline_element * in_hline );
extern void         fb_output_textline( text_line * out_line );
extern void         fb_start( void );
extern void         fb_vline( vline_element * in_vline );


/* devfuncs.c                           */
extern void         fb_absoluteaddress( void );
extern void         fb_new_section( uint32_t v_start );
extern void         fb_position( uint32_t h_start, uint32_t v_start );


/* gargutil.c                           */
extern  void        garginit( void );
extern  void        garginitdot( void );
extern  condcode    getarg( void );
extern  condcode    getqst( void );
extern  bool        is_quote_char( char c );
extern  bool        is_function_char( char c );
extern  bool        is_lay_att_char( char c );
extern  bool        is_id_char( char c );
extern  bool        is_macro_char( char c );
extern  bool        is_space_tab_char( char c );
extern  bool        is_stop_char( char c );
extern  bool        is_symbol_char( char c );
extern  char        parse_char( char * pa, int len );
extern  void        unquote_if_quoted( char * * a, char * * z );


/* gbanner.c                            */
extern  void        out_ban_bot( void );
extern  void        out_ban_top( void );
extern  void        set_banners( void );
extern  void        set_headx_banners( int hx_lvl );


/* gdata.c                              */
extern  void    init_global_vars( void );
extern  void    init_pass_data( void );


/* gdeflay.c                            */
extern  void    init_def_lay( void );


/* gdocpage.c                           */
extern  void            clear_doc_element( doc_element * element );
extern  void            do_page_out( void );
extern  void            full_page_out( void );
extern  void            insert_col_bot( doc_element * a_element );
extern  void            insert_col_fn( doc_element * a_element );
extern  void            insert_col_main( doc_element * a_element );
extern  void            insert_col_top( doc_element * a_element );
extern  void            insert_page_width( doc_element * a_element );
extern  void            last_page_out( void );
extern  void            reset_t_page( void );
extern  void            set_skip_vars( su * pre_skip, su * pre_top_skip, su * post_skip, uint32_t spacing, font_number font );
extern  bool            split_element( doc_element * a_element, uint32_t req_depth );
extern  void            text_page_out( void );


/* gdocsect.c                           */
extern  void    set_section_banners( doc_section ds );
extern  void    start_doc_sect( void );


/* gerror.c                             */
extern  void    out_msg( const char * fmt, ... );
extern  void    g_err( const msg_ids err, ... );
extern  void    g_warn( const msg_ids err, ... );
extern  void    g_info( const msg_ids err, ... );
extern  void    g_info_lm( const msg_ids err, ... );
extern  void    g_suicide( void );


/* gerrorxx.c                           */
extern  void    att_val_err( char * attname );
extern  void    auto_att_err( void );
extern  void    cw_err( void );
extern  void    dc_opt_err( const msg_ids num, char * pa );
extern  void    dc_opt_warn( char * pa );
extern  void    file_mac_info( void );
extern  void    file_mac_info_nest( void );
extern  void    internal_err( char * file, int line );
extern  void    nottag_err( void );
extern  void    numb_err( void );
extern  void    parm_extra_err( char *cw, char *pa );
extern  void    parm_miss_err( char *cw );
extern  void    val_parse_err( const char *pa, bool tag );
extern  void    tag_name_missing_err( void );
extern  void    tag_text_err( char * tagname );
extern  void    tag_text_req_err( char * tagname );
extern  void    xx_err( const msg_ids errid );
extern  void    xx_line_err( const msg_ids errid, char * pa );
extern  void    xx_nest_err( const msg_ids errid );
extern  void    xx_opt_err( char *cw, char *pa );
extern  void    xx_simple_err( const msg_ids errid );
extern  void    xx_simple_err_c( const msg_ids errid, const char * arg );
extern  void    xx_simple_err_i( const msg_ids errid, int arg );
extern  void    xx_simple_err_cc( const msg_ids errid, const char * arg1, const char * arg2 );
extern  void    xx_tag_err( const msg_ids errid, char const * cw );
extern  void    xx_warn( const msg_ids errid );
extern  void    g_err_tag( char *tagname );
extern  void    g_err_tag_nest( char *tagname );
extern  void    g_err_tag_rsloc( locflags inloc, char *pa );
extern  void    g_err_tag_no( char *tagname );
extern  void    g_err_tag_prec( char *tagname );
extern  void    g_err_tag_x_in_y( char *tagname1, char *tagname2 );


/* getnum.c                             */
extern condcode     getnum( getnum_block * gn );


/* gindexut.c                           */
extern ix_e_blk *   fill_ix_e_blk( ix_e_blk * * anchor, ix_h_blk * ref, ereftyp ptyp, char * text, size_t text_len );
extern  void        free_index_dict( ix_h_blk ** dict );
extern  void        free_ix_e_index_dict( ix_h_blk ** dict );
extern  void        gen_index( void );
extern  void        ixdump( ix_h_blk * dict );


/* glayutil.c                           */
extern void         eat_lay_sub_tag( void );
extern condcode     get_lay_sub_and_value( struct att_args * l_args );


/* glbanner.c                           */
extern void         lay_banner_end_prepare( void );


/* glbandef.c                           */
extern void         banner_defaults( void );


/* gmacdict.c                           */
extern  void        add_macro_entry( mac_entry * * dict, mac_entry * me );
extern  void        init_macro_dict( mac_entry * * dict );
extern  void        free_macro_dict( mac_entry * * dict );
extern  void        free_macro_entry( mac_entry * * dict, mac_entry * me );
extern  void        print_macro_dict( mac_entry * dict, bool with_mac_lines );
extern  mac_entry * find_macro( mac_entry * dict, char const * name );


/* gmemory.c                            */
extern  void        mem_free( void * p );
extern  void    *   mem_alloc( size_t size );
extern  void    *   mem_realloc( void * p, size_t size );

extern  void        g_trmem_init( void );
extern  void        g_trmem_prt_list( void );
extern  void        g_trmem_close( void );
extern  unsigned long   g_trmem_peak_usage( void );


/* gnestut.c                            */
extern  void            init_nest_cb( void );
extern  nest_stack  *   copy_to_nest_stack( void );


/* goptions.c                           */
extern  int     proc_options( char * cmdline );
extern  void    split_attr_file( char * filename, char * attr, size_t attrlen );


/* gpagegeo.c                           */
extern  void    do_layout_end_processing( void );
extern  void    init_def_margins( void );
extern  void    init_page_geometry( void );
extern  void    set_page_position( doc_section ds );


/* gppcnote.c                           */
extern  void    proc_p_pc( p_lay_tag * p_pc );


/* gprocess.c                           */
extern  void    classify_record( char firstchar );
extern  void    process_late_subst( void );
extern  void    process_line( void );
extern  bool    resolve_symvar_functions( char * buf );
extern  void    split_input( char * buf, char * split_pos, bool startofline );


/* gproctxt.c                           */
extern  void            do_justify( uint32_t left_m, uint32_t right_m, text_line * line );
extern  void            intrans( char * data, uint16_t * len, font_number font );
extern  void            process_line_full( text_line * a_line, bool justify );
extern  void            process_text( char * text, font_number font );
extern  text_chars  *   process_word( char * text, size_t count, font_number font );
extern  void            set_h_start( void );


/* grefdict.c                           */
extern  void        add_ref_entry( ref_entry * * dict, ref_entry * me );
extern  void        init_ref_dict( ref_entry * * dict );
extern  void        free_ref_dict( ref_entry * * dict );
extern  void        print_ref_dict( ref_entry * dict, const char * type );
extern  ref_entry   *find_refid( ref_entry * dict, char const * id );
extern  void        init_ref_entry( ref_entry * re, char * id, size_t len );
extern  void        fill_id( ref_entry * re, char * id, size_t len );
extern  char        *get_refid_value( char *p );


/* gresrch.c                            */
extern  void    add_GML_tag_research( char * tag );
extern  void    free_GML_tags_research( void );
extern  void    print_GML_tags_research( void );
extern  void    add_SCR_tag_research( char * tag );
extern  void    free_SCR_tags_research( void );
extern  void    print_SCR_tags_research( void );
extern  void    add_single_func_research( char * in );
extern  void    free_single_funcs_research( void );
extern  void    print_single_funcs_research( void );
extern  void    add_multi_func_research( char * in );
extern  void    free_multi_funcs_research( void );
extern  void    print_multi_funcs_research( void );
extern  void    printf_research( char * msg, ... );
extern  void    test_out_t_line( text_line  * a_line );


/* gsbr.c                               */
extern  void    scr_process_break( void );


/* gscan.c                              */
extern  const   gmltag * find_lay_tag( char * token, size_t toklen );
extern  const   gmltag * find_sys_tag( char * token, size_t toklen );
extern  void             scan_line( void );
extern  void             set_if_then_do( ifcb * cb );
extern  condcode         test_process( ifcb * cb );


/* gsfuncs.c                            */
extern  char    *   scr_multi_funcs( char * in, char * end, char * * ppval, int32_t valsize );


/* gsfunelu.c                           */
extern  char    *   scr_single_funcs( char * in, char * end, char * * result );
extern  char    *   scr_single_func_e( char * in, char * end, char * * result );
extern  char    *   scr_single_func_l( char * in, char * end, char * * result );
extern  char    *   scr_single_func_u( char * in, char * end, char * * result );
extern  char    *   scr_single_func_w( char * in, char * end, char * * result );


/* gsgoto.c                             */
extern  void    print_labels( labelcb * lb, char * name );
extern  bool    gotarget_reached( void );


/* gsgt.c                               */
extern  void    init_tag_att( void );


/* gsifdoel.c                           */
extern  void    show_ifcb( char * txt, ifcb * cb );


/* gsetvar.c                            */
extern char *   scan_sym( char * p, symvar * sym, sub_index * subscript );


/* gsmacro.c                            */
extern  void    add_macro_cb_entry( mac_entry * me, gtentry * ge );
extern  void    add_macro_parms( char * p );
extern  void    free_lines( inp_line * line );


/* gspe.c                               */
extern  void    reset_pe_cb( void );


/* gspu.c                               */
extern  char    *get_pu_file_name( char *buf, size_t buf_len, int numb );
extern  void    close_pu_file( int numb );
extern  void    close_all_pu_files( void );


/* gssk.c                               */
extern  void    calc_skip( void );
extern  int32_t calc_skip_value( void );


/* gsymvar.c                            */
extern void     init_dict( symvar * * dict );
extern void     free_dict( symvar * * dict );
extern int      find_symvar( symvar * * dict, char * name, sub_index subscript, symsub * * symsubval );
extern int      find_symvar_l( symvar * * dict, char * name, sub_index subscript, symsub * * symsubval );
extern int      add_symvar( symvar * * dict, char * name, char * val, sub_index subscript, symbol_flags f );
extern int      add_symvar_addr( symvar * * dict, char * name, char * val, sub_index subscript, symbol_flags f, symsub * * sub );
extern void     print_sym_dict( symvar * dict );
extern void     reset_auto_inc_dict( symvar * dict );


/* gsyssym.c                            */
extern  void    add_to_sysdir( char * var_name, char char_val );
extern  void    init_sys_dict( symvar * * dict );
extern  void    init_sysparm( char * cmdline, char * banner );


/* gtagdict.c                           */
extern  gtentry *   add_tag( gtentry * * dict, char const * name, char const * macro, const int flags );
extern  gtentry *   change_tag( gtentry * * dict, char const * name, char const * macro );
extern  void        init_tag_dict( gtentry * * dict );
extern  void        free_tag_dict( gtentry * * dict );
extern  gtentry *   free_tag( gtentry * * dict, gtentry * ge );
extern  void        print_tag_dict( gtentry * dict );
extern  void        print_tag_entry( gtentry * entry );
extern  gtentry *   find_tag( gtentry * * dict, char const * name );

/* gtxtpool.c                           */
extern  void            add_text_chars_to_pool( text_line * a_line );
extern  text_chars      *alloc_text_chars( char *p, size_t cnt, font_number font );
extern  void            add_text_line_to_pool( text_line * a_line );
extern  text_line       *alloc_text_line( void );
extern  void            add_ban_col_to_pool( ban_column * a_column );
extern  ban_column      *alloc_ban_col( void );
extern  void            add_box_col_set_to_pool( box_col_set * a_set );
extern  box_col_set     *alloc_box_col_set( void );
extern  void            add_doc_col_to_pool( doc_column * a_column );
extern  doc_column      *alloc_doc_col( void );
extern  void            add_doc_el_to_pool( doc_element * a_element );
extern  doc_element     *alloc_doc_el( element_type type );
extern  void            add_tag_cb_to_pool( tag_cb * cb );
extern  tag_cb          *alloc_tag_cb( void );
extern  void            free_pool_storage( void );


/* gtitlepo.c                           */
extern  void        titlep_output( void );


/* gusertag.c                           */
extern  bool        process_tag( gtentry * ge, mac_entry * me );


/* gutil.c                              */
extern  bool        att_val_to_su( su * spaceunit, bool pos );
extern  int32_t     conv_hor_unit( su * spaceunit );
extern  int32_t     conv_vert_unit( su * spaceunit, uint8_t spacing );
extern  bool        cw_val_to_su( char * * scaninput, su * spaceunit );
extern  char    *   format_num( uint32_t n, char * r, size_t rsize, num_style ns );
extern  char    *   get_att_value( char * p );
extern  su      *   greater_su( su * su_a, su * su_b, uint8_t spacing );
extern  char    *   int_to_roman( uint32_t n, char * r, size_t rsize );
extern  bool        lay_init_su( const char * p, su * in_su );
extern  int32_t     len_to_trail_space( char p[] , int32_t len );
extern  void        start_line_with_string( char * text, font_number font, bool leave1space );


/* outbuff.c                            */
extern void     cop_tr_table( char * p );
extern void     ob_binclude( binclude_element * in_el );
extern void     ob_direct_out( char * p );
extern void     ob_graphic( graphic_element * in_el );


/* wgmlmsg.c                            */
extern  int     init_msgs( void );
extern  void    fini_msgs( void );
extern  int     get_msg( msg_ids resourceid, char *buffer, size_t buflen );
//extern  void Msg_Do_Put_Args( char rc_buff[], MSG_ARG_LIST *arg_info, char *types, ... );
//extern  void Msg_Put_Args( char message[], MSG_ARG_LIST *arg_info, char *types, va_list *args );


/* wgmlsupp.c                           */
extern  bool    free_resources( errno_t in_errno );
extern  void    free_some_mem( void );
extern  void    free_layout_banner( void );
extern  void    g_banner( void );
// extern  char  * get_filename_full_path( char * buff, char const * name, size_t max );
extern  bool    get_line( bool researchoutput );
extern  void    inc_inc_level( void );

#ifdef __WATCOMC__
#pragma aux     my_exit aborts;
#endif
extern  void    my_exit( int );
extern  void    show_include_stack( void );


/*
 * prototypes for the gml tag processing routines
 */

#ifdef pickg
    #undef pickg
#endif
#define pickg( name, length, routine, gmlflags, locflags )  extern void routine( const gmltag * entry );

#include "gtags.h"

/*
 * prototypes for the layout tag processing routines
 */

#define pick( name, length, routine, flags )  extern void routine( const gmltag * entry );

#include "gtagslay.h"

/*
 * prototypes for the layout tag attribute processing routines
 */

/*          for input scanning              */
#define pick( name, funci, funco, restype ) \
    extern  bool    funci( char * buf, lay_att attr, restype * result );

#include "glayutil.h"

/*          for output via :convert tag     */
#define pick( name, funci, funco, restype ) \
    extern  void    funco( FILE * f, lay_att attr, restype * in );

#include "glayutil.h"

/*
 * prototypes for the script control word processing routines
 */

#undef picklab
#define picklab( name, routine, flags )  extern void routine( void );

#undef picks
#define picks( name, routine, flags )  extern void routine( void );

#include "gscrcws.h"

/*
 * prototypes for the script string function routines , ie. &'substr( ,..
 */

#define pick( name, length, mand_parms, opt_parms, routine ) \
    extern condcode routine( parm parms[MAX_FUN_PARMS], size_t parm_count, char **ppval, int32_t valsize );

#include "gsfuncs.h"


#ifdef  __cplusplus
}   /* End of "C" linkage for C++ */
#endif

#endif  /* WGML_H_INCLUDED */
