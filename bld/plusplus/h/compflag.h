/****************************************************************************
*
*                            Open Watcom Project
*
*    Portions Copyright (c) 1983-2002 Sybase, Inc. All Rights Reserved.
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
* Description:  This file contains the structure definition for the global
*               compiler flags. The flags (CompFlags) are defined in
*               plusplus.h
*
****************************************************************************/

#ifndef _COMPFLAG_H_
#define _COMPFLAG_H_

typedef struct comp_flags {
/*****************************************************************************
//                                                                          //
//  execution status flags                                                  //
//                                                                          //
*****************************************************************************/
/*  00 */
    unsigned float_used             : 1;
    unsigned stats_printed          : 1;
    unsigned low_on_memory_printed  : 1;
    unsigned external_defn_found    : 1;
    unsigned scanning_c_comment     : 1;    // scanning ANSI style (/**/) comment
    unsigned srcfile_compiled       : 1;    // source file has been compiled
    unsigned errfile_opened         : 1;    // error file has been opened
    unsigned errfile_written        : 1;    // something has been written
/*  08  */
    unsigned main_has_parms         : 1;    /* on if "main" has parm(s) */
    unsigned pgm_used_8087          : 1;    /* on => 8087 ins. generated */
    unsigned banner_printed         : 1;    /* on => banner printed      */
    unsigned cppi_segment_used      : 1;    /* C++ initializer segment */
    unsigned rescan_buffer_done     : 1;    /* ## re-scan buffer used up */
    unsigned has_winmain            : 1;    // WinMain() is defined
    unsigned has_libmain            : 1;    // LibMain() is defined
    unsigned has_dllmain            : 1;    // DllMain() is defined
/*  16  */
    unsigned has_main               : 1;    // main() is defined
    unsigned has_longjmp            : 1;    // has longjmp reference
    unsigned extern_C_defn_found    : 1;    // something was declared as "C"
    unsigned inline_intrinsics      : 1;    // inline intrinsic fns
    unsigned scanning_cpp_comment   : 1;    // scanning a C++ style comment
    unsigned pragma_library         : 1;    // on => behave as if main seen
    unsigned parsing_finished       : 1;    // program has been completely parsed
    unsigned genned_static_dtor     : 1;    // genned DTOR of static var.
/*  24  */
    unsigned inline_fun_reg         : 1;    // inline func. registration gen'ed
    unsigned fatal_error            : 1;    // fatal error has occured
    unsigned codegen_active         : 1;    // code generator is active
    unsigned dt_method_pragma       : 1;    // pragma destruct encountered
    unsigned dll_subsequent         : 1;    // DLL called second time thru
    unsigned dll_active             : 1;    // DLL version of compiler
    unsigned compile_failed         : 1;    // compilation failed
    unsigned cmdline_error          : 1;    // error in cmd line
/*  32  */
    unsigned watch_for_pcheader     : 1;    // watch for first #include
    unsigned pch_debug_info_write   : 1;    // writing dbg info into PCH
    unsigned pch_debug_info_read    : 1;    // reading dbg info from PCH
    unsigned batch_file_primary     : 1;    // batch file found: primary scan
    unsigned batch_file_processing  : 1;    // processing the batch file
    unsigned batch_file_eof         : 1;    // EOF on the batch file
    unsigned batch_file_continue    : 1;    // continue on errors
    unsigned in_pragma              : 1;    // scanning in a #pragma
/*  40 */
    unsigned ide_cmd_line           : 1;    // ide command line ok as is
    unsigned ide_console_output     : 1;    // ide doing output to console
    unsigned use_macro_tokens       : 1;    // macro expansion is token source
    unsigned log_note_msgs          : 1;    // on ==> NOTE_MSGs to error file
    unsigned has_wide_char_main     : 1;    // has wide char version of "main" fn
    unsigned vfun_reference_done    : 1;    // extrf: -zv record generated
    unsigned namespace_checks_done  : 1;    // namespace ref/def checks done

    //  don't add options here; these are flags that represent the status
    //  during execution
    //  Add options at offset 80 or greater (the number is not significant in any way!
    unsigned filler00               : 1;
/*  48 */
    unsigned filler01               : 1;
    unsigned filler02               : 1;
    unsigned filler03               : 1;
    unsigned filler04               : 1;
    unsigned filler05               : 1;
    unsigned filler06               : 1;
    unsigned filler07               : 1;
    unsigned filler08               : 1;
/*  56  */
    unsigned filler09               : 1;
    unsigned filler10               : 1;
    unsigned filler11               : 1;
    unsigned filler12               : 1;
    unsigned filler13               : 1;
    unsigned filler14               : 1;
    unsigned filler15               : 1;
    unsigned filler16               : 1;

/*****************************************************************************
//                                                                          //
// Command Line Option flags                                                //
//                                                                          //
*****************************************************************************/
/*****************************************************************************
//                                                                          //
// 'dbg' target flags                                                       //
//                                                                          //
*****************************************************************************/

/*  64  */
    unsigned extra_stats_wanted     : 1;

    unsigned dbgfiller00            : 1;
    unsigned dbgfiller01            : 1;
    unsigned dbgfiller02            : 1;
    unsigned dbgfiller03            : 1;
    unsigned dbgfiller04            : 1;
    unsigned dbgfiller05            : 1;
    unsigned dbgfiller06            : 1;
/*  72  */
    unsigned dbgfiller07            : 1;
    unsigned dbgfiller08            : 1;
    unsigned dbgfiller09            : 1;
    unsigned dbgfiller10            : 1;
    unsigned dbgfiller11            : 1;
    unsigned dbgfiller12            : 1;
    unsigned dbgfiller13            : 1;
    unsigned dbgfiller14            : 1;

/*****************************************************************************
//                                                                          //
//  'any' target flags                                                      //
//                                                                          //
*****************************************************************************/
/*  80  */
    unsigned signed_char            : 1;
    unsigned check_syntax           : 1;
    unsigned extensions_enabled     : 1;
    unsigned inline_functions       : 1;    // inline user def'd 'inline' fns
    unsigned dump_prototypes        : 1;    // output prototypes into .DEF file
    unsigned use_base_types         : 1;    // use base types in .DEF protos
    unsigned quiet_mode             : 1;
    unsigned keep_comments          : 1;    // wcpp - output comments
/*  88  */
    unsigned emit_library_names     : 1;    // on => put LIB name in obj
    unsigned comments_wanted        : 1;    // on => comments wanted
    unsigned undefine_all_macros    : 1;    // on => -u all macros
    unsigned cpp_line_wanted        : 1;    // wcpp - emit #line
    unsigned cpp_ignore_line        : 1;    /* wcpp - ignore #line */
    unsigned cpp_output             : 1;    // WCC doing CPP output
    unsigned cpp_output_to_file     : 1;    // WCC doing CPP output to?.i
    unsigned cpp_output_requested   : 1;
/*  96  */
    unsigned no_debug_type_names    : 1;
    unsigned emit_names             : 1;
    unsigned warnings_cause_bad_exit: 1;
    unsigned use_unicode            : 1;
    unsigned unique_functions       : 1;
    unsigned jis_to_unicode         : 1;
    unsigned emit_dependencies      : 1;
    unsigned emit_targimp_symbols   : 1;    // Insert target specific import symbols
/*  104 */
    unsigned emit_browser_info      : 1;
    unsigned excs_enabled           : 1;    // exceptions enabled
    unsigned rtti_enabled           : 1;    // RTTI enabled
    unsigned extended_defines       : 1;    // -d swallows many tokens
    unsigned virtual_stripping      : 1;    // enable virtual stripping
    unsigned returns_promoted       : 1;    // promote arg,return to int
    unsigned all_debug_type_names   : 1;
    unsigned fhw_switch_used        : 1;
/*  112 */
    unsigned fhr_switch_used        : 1;
    unsigned modifier_bind_compatibility:1; // bind modifiers into generic types
    unsigned prototype_instantiate  : 1;    // instantiate fn prototypes if possible
    unsigned encrypt_preproc_output : 1;    // try to encrypt compiler output
    unsigned check_truncated_fnames : 1;    // if can't find file, check 8.3
    unsigned make_enums_an_int      : 1;    // don't overly optimize enum storage
    unsigned use_pcheaders          : 1;    // pre-compiled headers are active
    unsigned no_pch_warnings        : 1;    // don't explain PCH activity
/*  120 */
    unsigned pch_debug_info_opt     : 1;    // optimize dbg info in PCH
    unsigned pch_min_check          : 1;    // assume PCH header files haven't changed
    unsigned bd_switch_used         : 1;
    unsigned bm_switch_used         : 1;
    unsigned bw_switch_used         : 1;
    unsigned ee_switch_used         : 1;
    unsigned ep_switch_used         : 1;
    unsigned eq_switch_used         : 1;
/*  128 */
    unsigned ew_switch_used         : 1;
    unsigned op_switch_used         : 1;
    unsigned br_switch_used         : 1;
    unsigned rw_registration        : 1;    // RW-centric function registration
    unsigned original_enum_setting  : 1;    // original -ei setting
    unsigned ignore_environment     : 1;    // don't process WPP* and INCLUDE
    unsigned ignore_current_dir     : 1;    // ignore current dir for include files
    unsigned progress_messages      : 1;    // output msgs as compile progresses
/*  136 */
    unsigned no_error_sym_injection : 1;    // don't insert error symbols
    unsigned error_use_full         : 1;    // use full path names in error msgs
    unsigned vc_alloca_parm         : 1;    // allow alloca to occur in parms
    unsigned emit_all_default_libs  : 1;    // emit default libs in .OBJ file
    unsigned bc_switch_used         : 1;    // building a console app
    unsigned bg_switch_used         : 1;    // building a GUI app
    unsigned optbr_v                : 1;    // browse: variables
    unsigned optbr_f                : 1;    // browse: functions
/*  144 */
    unsigned optbr_t                : 1;    // browse: types
    unsigned optbr_m                : 1;    // browse: data members
    unsigned optbr_p                : 1;    // browse: preprocessor macros
    unsigned static_inline_fns      : 1;    // emit inline fns in normal code seg
    unsigned fhwe_switch_used       : 1;    // display but don't treat as errors
    unsigned warn_about_padding     : 1;    // warn when padding is added

    unsigned overload_13332         : 1;    // implement WP 13.3.3.2 better
    unsigned line_comments          : 1;    // output // #line instead of #line
/*  152 */
    unsigned dont_align_segs        : 1;    // bug fix: don't align segments
    unsigned fixed_name_mangling    : 1;    // bug fix: name mangling
    unsigned plain_char_promotion   : 1;    // char -> int rvalue promotion warning
    unsigned obfuscate_typesig_names: 1;    // mangle type sig names
    unsigned dont_autogen_ext_inc   : 1;    // don't convert #include <string> to <string.h> or <string.hpp>
    unsigned dont_autogen_ext_src   : 1;
    unsigned use_old_for_scope      : 1;
    unsigned no_alternative_tokens  : 1;    // disable alternative tokens
/*  160 */
    unsigned enable_std0x           : 1;    // enable some C++0x features
    

/*****************************************************************************
//                                                                          //
// 'i86' '386' target flags
//                                                                          //
*****************************************************************************/
    unsigned register_conventions   : 1;    // on for -3r, off for -3s
    unsigned strings_in_code_segment: 1;    // on => put strings in CODE
    unsigned save_restore_segregs   : 1;    // save/restore segment regs
    unsigned target_multi_thread    : 1;    // multiple execution threads
    unsigned use_stdcall_at_number  : 1;    // mangle __stdcall names as "_*@n"
    unsigned fs_registration        : 1;    // using fs for function registration
    unsigned sg_switch_used         : 1;
/*  168 */
    unsigned st_switch_used         : 1;
    unsigned zc_switch_used         : 1;
    unsigned zm_switch_used         : 1;
    unsigned zmf_switch_used        : 1;
    unsigned zo_switch_used         : 1;
    unsigned zu_switch_used         : 1;
    unsigned zx_switch_used         : 1;
    unsigned mfi_switch_used        : 1;    // flat model interrupts (ss stays same)

/*  176 */
    unsigned i86filler00            : 1;
    unsigned i86filler01            : 1;
    unsigned i86filler02            : 1;
    unsigned i86filler03            : 1;
    unsigned i86filler04            : 1;
    unsigned i86filler05            : 1;
    unsigned i86filler06            : 1;
    unsigned i86filler07            : 1;
/* 184 */
    unsigned i86filler08            : 1;

    unsigned generate_auto_depend   : 1;
    unsigned ignore_fnf             : 1;
    unsigned disable_ialias         : 1;
    unsigned cpp_ignore_env         : 1;    // ignore include path env vars
    unsigned ignore_default_dirs    : 1;    // ignore all default dirs for file search (., ../h, ../c, ...)
} COMP_FLAGS;

#ifdef OPT_BR
    #define BrinfActive() (     \
        CompFlags.optbr_v ||    \
        CompFlags.optbr_f ||    \
        CompFlags.optbr_t ||    \
        CompFlags.optbr_m ||    \
        CompFlags.optbr_p       \
        )
#endif

typedef enum                            // DTM -- destruction method
{   DTM_DIRECT                          // - direct calls
,   DTM_DIRECT_TABLE                    // - direct calls + table
,   DTM_DIRECT_SMALL                    // - direct calls + smallness
,   DTM_TABLE                           // - table
,   DTM_TABLE_SMALL                     // - table + smallness
,   DTM_COUNT                           // number of methods
} DT_METHOD;

typedef struct                          // DLL_DATA -- data for DLL
{   void (*print_str)( char const* );   // - print: string
    void (*print_chr)( char );          // - print: character
    void (*print_line)( char const* );  // - print: line
    char *cmd_line;                     // - command line
    int argc;                           // - argc/argv command line
    char **argv;
} DLL_DATA;

#include <pushpck4.h>
struct comp_info {                      // Compiler information
    TYPE        ptr_diff_near;          // - type from near ptr subtraction
    TYPE        ptr_diff_far;           // - type from far ptr subtraction
    TYPE        ptr_diff_far16;         // - type from far16 ptr subtraction
    TYPE        ptr_diff_huge;          // - type from huge ptr subtraction
    uint_8      init_priority;          // - initialization priority
    DT_METHOD   dt_method;              // - destruction method
    DT_METHOD   dt_method_speced;       // - specified destruction method
    unsigned    :0;                     // - alignment
    DLL_DATA*   dll_data;               // - data for DLL
    void*       exit_jmpbuf;            // - jmpbuf for DLL exit
    void*       dll_handle;             // - dll handle
    void*       dll_callbacks;          // - dll call backs
    int         compfile_max;           // - max # files to compile
    int         compfile_cur;           // - current # of file to compile
    void*       primary_srcfile;        // - SRCFILE for primary file
    unsigned    fc_file_line;           // - line # in -fc file
    char*       pch_buff_cursor;        // - PCH read: buffer cursor
    char*       pch_buff_end;           // - PCH read: end of buffer
};
#include <poppck.h>
#endif
