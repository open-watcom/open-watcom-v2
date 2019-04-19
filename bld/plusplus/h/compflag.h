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
    bool    float_used                      : 1;
    bool    stats_printed                   : 1;
    bool    low_on_memory_printed           : 1;
    bool    external_defn_found             : 1;
    bool    scanning_c_comment              : 1;    // scanning ANSI style (/**/) comment
    bool    srcfile_compiled                : 1;    // source file has been compiled
    bool    errfile_opened                  : 1;    // error file has been opened
    bool    errfile_written                 : 1;    // something has been written
/*  08  */
    bool    main_has_parms                  : 1;    /* on if "main" has parm(s) */
    bool    pgm_used_8087                   : 1;    /* on => 8087 ins. generated */
    bool    banner_printed                  : 1;    /* on => banner printed      */
    bool    cppi_segment_used               : 1;    /* C++ initializer segment */
    bool    rescan_buffer_done              : 1;    /* ## re-scan buffer used up */
    bool    has_winmain                     : 1;    // WinMain() is defined
    bool    has_libmain                     : 1;    // LibMain() is defined
    bool    has_dllmain                     : 1;    // DllMain() is defined
/*  16  */
    bool    has_main                        : 1;    // main() is defined
    bool    has_longjmp                     : 1;    // has longjmp reference
    bool    extern_C_defn_found             : 1;    // something was declared as "C"
    bool    inline_intrinsics               : 1;    // inline intrinsic fns
    bool    scanning_cpp_comment            : 1;    // scanning a C++ style comment
    bool    pragma_library                  : 1;    // on => behave as if main seen
    bool    parsing_finished                : 1;    // program has been completely parsed
    bool    genned_static_dtor              : 1;    // genned DTOR of static var.
/*  24  */
    bool    inline_fun_reg                  : 1;    // inline func. registration gen'ed
    bool    fatal_error                     : 1;    // fatal error has occured
    bool    codegen_active                  : 1;    // code generator is active
    bool    dt_method_pragma                : 1;    // pragma destruct encountered
    bool    dll_subsequent                  : 1;    // DLL called second time thru
    bool    dll_active                      : 1;    // DLL version of compiler
    bool    compile_failed                  : 1;    // compilation failed
    bool    cmdline_error                   : 1;    // error in cmd line
/*  32  */
    bool    watch_for_pcheader              : 1;    // watch for first #include
    bool    pch_debug_info_write            : 1;    // writing dbg info into PCH
    bool    pch_debug_info_read             : 1;    // reading dbg info from PCH
    bool    batch_file_primary              : 1;    // batch file found: primary scan
    bool    batch_file_processing           : 1;    // processing the batch file
    bool    batch_file_eof                  : 1;    // EOF on the batch file
    bool    batch_file_continue             : 1;    // continue on errors
    bool    in_pragma                       : 1;    // scanning in a #pragma
/*  40 */
    bool    ide_cmd_line                    : 1;    // ide command line ok as is
    bool    ide_console_output              : 1;    // ide doing output to console
    bool    use_macro_tokens                : 1;    // macro expansion is token source
    bool    log_note_msgs                   : 1;    // on ==> NOTE_MSGs to error file
    bool    has_wide_char_main              : 1;    // has wide char version of "main" fn
    bool    vfun_reference_done             : 1;    // extrf: -zv record generated
    bool    namespace_checks_done           : 1;    // namespace ref/def checks done

    //  don't add options here; these are flags that represent the status
    //  during execution
    //  Add options at offset 80 or greater (the number is not significant in any way!
    bool    filler00                        : 1;
/*  48 */
    bool    filler01                        : 1;
    bool    filler02                        : 1;
    bool    filler03                        : 1;
    bool    filler04                        : 1;
    bool    filler05                        : 1;
    bool    filler06                        : 1;
    bool    filler07                        : 1;
    bool    filler08                        : 1;
/*  56  */
    bool    filler09                        : 1;
    bool    filler10                        : 1;
    bool    filler11                        : 1;
    bool    filler12                        : 1;
    bool    filler13                        : 1;
    bool    filler14                        : 1;
    bool    filler15                        : 1;
    bool    filler16                        : 1;

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
    bool    extra_stats_wanted              : 1;

    bool    dbgfiller00                     : 1;
    bool    dbgfiller01                     : 1;
    bool    dbgfiller02                     : 1;
    bool    dbgfiller03                     : 1;
    bool    dbgfiller04                     : 1;
    bool    dbgfiller05                     : 1;
    bool    dbgfiller06                     : 1;
/*  72  */
    bool    dbgfiller07                     : 1;
    bool    dbgfiller08                     : 1;
    bool    dbgfiller09                     : 1;
    bool    dbgfiller10                     : 1;
    bool    dbgfiller11                     : 1;
    bool    dbgfiller12                     : 1;
    bool    dbgfiller13                     : 1;
    bool    dbgfiller14                     : 1;

/*****************************************************************************
//                                                                          //
//  'any' target flags                                                      //
//                                                                          //
*****************************************************************************/
/*  80  */
    bool    signed_char                     : 1;
    bool    check_syntax                    : 1;
    bool    extensions_enabled              : 1;
    bool    inline_functions                : 1;    // inline user def'd 'inline' fns
    bool    dump_prototypes                 : 1;    // output prototypes into .DEF file
    bool    use_base_types                  : 1;    // use base types in .DEF protos
    bool    quiet_mode                      : 1;
    bool    keep_comments                   : 1;    // wcpp - output comments
/*  88  */
    bool    emit_library_names              : 1;    // on => put LIB name in obj
    bool    comments_wanted                 : 1;    // on => comments wanted
    bool    undefine_all_macros             : 1;    // on => -u all macros
    bool    cpp_line_wanted                 : 1;    // wcpp - emit #line
    bool    cpp_ignore_line                 : 1;    /* wcpp - ignore #line */
    bool    cpp_output                      : 1;    // WCC doing CPP output
    bool    cpp_output_to_file              : 1;    // WCC doing CPP output to?.i
    bool    cpp_output_requested            : 1;
/*  96  */
    bool    no_debug_type_names             : 1;
    bool    emit_names                      : 1;
    bool    warnings_cause_bad_exit         : 1;
    bool    use_unicode                     : 1;
    bool    unique_functions                : 1;
    bool    jis_to_unicode                  : 1;
    bool    emit_dependencies               : 1;
    bool    emit_targimp_symbols            : 1;    // Insert target specific import symbols
/*  104 */
    bool    emit_browser_info               : 1;
    bool    excs_enabled                    : 1;    // exceptions enabled
    bool    rtti_enabled                    : 1;    // RTTI enabled
    bool    extended_defines                : 1;    // -d swallows many tokens
    bool    virtual_stripping               : 1;    // enable virtual stripping
    bool    returns_promoted                : 1;    // promote arg,return to int
    bool    all_debug_type_names            : 1;
    bool    fhw_switch_used                 : 1;
/*  112 */
    bool    fhr_switch_used                 : 1;
    bool    modifier_bind_compatibility     : 1; // bind modifiers into generic types
    bool    prototype_instantiate           : 1;    // instantiate fn prototypes if possible
    bool    encrypt_preproc_output          : 1;    // try to encrypt compiler output
    bool    check_truncated_fnames          : 1;    // if can't find file, check 8.3
    bool    make_enums_an_int               : 1;    // don't overly optimize enum storage
    bool    use_pcheaders                   : 1;    // pre-compiled headers are active
    bool    no_pch_warnings                 : 1;    // don't explain PCH activity
/*  120 */
    bool    pch_debug_info_opt              : 1;    // optimize dbg info in PCH
    bool    pch_min_check                   : 1;    // assume PCH header files haven't changed
    bool    bd_switch_used                  : 1;
    bool    bm_switch_used                  : 1;
    bool    bw_switch_used                  : 1;
    bool    ee_switch_used                  : 1;
    bool    ep_switch_used                  : 1;
    bool    eq_switch_used                  : 1;
/*  128 */
    bool    ew_switch_used                  : 1;
    bool    op_switch_used                  : 1;
    bool    br_switch_used                  : 1;
    bool    rw_registration                 : 1;    // RW-centric function registration
    bool    original_enum_setting           : 1;    // original -ei setting
    bool    ignore_environment              : 1;    // don't process WPP* and INCLUDE
    bool    ignore_current_dir              : 1;    // ignore current dir for include files
    bool    progress_messages               : 1;    // output msgs as compile progresses
/*  136 */
    bool    no_error_sym_injection          : 1;    // don't insert error symbols
    bool    error_use_full                  : 1;    // use full path names in error msgs
    bool    vc_alloca_parm                  : 1;    // allow alloca to occur in parms
    bool    emit_all_default_libs           : 1;    // emit default libs in .OBJ file
    bool    bc_switch_used                  : 1;    // building a console app
    bool    bg_switch_used                  : 1;    // building a GUI app
    bool    optbr_v                         : 1;    // browse: variables
    bool    optbr_f                         : 1;    // browse: functions
/*  144 */
    bool    optbr_t                         : 1;    // browse: types
    bool    optbr_m                         : 1;    // browse: data members
    bool    optbr_p                         : 1;    // browse: preprocessor macros
    bool    static_inline_fns               : 1;    // emit inline fns in normal code seg
    bool    fhwe_switch_used                : 1;    // display but don't treat as errors
    bool    warn_about_padding              : 1;    // warn when padding is added

    bool    overload_13332                  : 1;    // implement WP 13.3.3.2 better
    bool    line_comments                   : 1;    // output // #line instead of #line
/*  152 */
    bool    dont_align_segs                 : 1;    // bug fix: don't align segments
    bool    fixed_name_mangling             : 1;    // bug fix: name mangling
    bool    plain_char_promotion            : 1;    // char -> int rvalue promotion warning
    bool    obfuscate_typesig_names         : 1;    // mangle type sig names
    bool    dont_autogen_ext_inc            : 1;    // don't convert #include <string> to <string.h> or <string.hpp>
    bool    dont_autogen_ext_src            : 1;
    bool    use_old_for_scope               : 1;
    bool    no_alternative_tokens           : 1;    // disable alternative tokens
/*  160 */
    bool    enable_std0x                    : 1;    // enable some C++0x features

/*****************************************************************************
//                                                                          //
// 'i86' '386' target flags
//                                                                          //
*****************************************************************************/
    bool    register_conventions            : 1;    // on for -3r, off for -3s
    bool    strings_in_code_segment         : 1;    // on => put strings in CODE
    bool    save_restore_segregs            : 1;    // save/restore segment regs
    bool    target_multi_thread             : 1;    // multiple execution threads
    bool    use_stdcall_at_number           : 1;    // mangle __stdcall names as "_*@n"
    bool    fs_registration                 : 1;    // using fs for function registration
    bool    sg_switch_used                  : 1;
/*  168 */
    bool    st_switch_used                  : 1;
    bool    zc_switch_used                  : 1;
    bool    zm_switch_used                  : 1;
    bool    zmf_switch_used                 : 1;
    bool    zo_switch_used                  : 1;
    bool    zu_switch_used                  : 1;
    bool    zx_switch_used                  : 1;
    bool    mfi_switch_used                 : 1;    // flat model interrupts (ss stays same)

/*  176 */
    bool    i86filler00                     : 1;
    bool    i86filler01                     : 1;
    bool    i86filler02                     : 1;
    bool    i86filler03                     : 1;
    bool    i86filler04                     : 1;
    bool    i86filler05                     : 1;
    bool    i86filler06                     : 1;
    bool    i86filler07                     : 1;
/* 184 */
    bool    i86filler08                     : 1;

    bool    generate_auto_depend            : 1;
    bool    ignore_fnf                      : 1;
    bool    cpp_ignore_env                  : 1;    // ignore include path env vars
    bool    ignore_default_dirs             : 1;    // ignore all default dirs for file search (., ../h, ../c, ...)
    bool    non_iso_compliant_names_enabled : 1;    // enable all compiler non-ISO compliant names (macros, symbols, etc.)
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
typedef struct comp_info {              // Compiler information
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
    void*       idehdl;                 // - IDE handle
    void*       idecbs;                 // - IDE call backs
    int         compfile_max;           // - max # files to compile
    int         compfile_cur;           // - current # of file to compile
    void*       primary_srcfile;        // - SRCFILE for primary file
    unsigned    fc_file_line;           // - line # in -fc file
    char*       pch_buff_cursor;        // - PCH read: buffer cursor
    char*       pch_buff_end;           // - PCH read: end of buffer
} comp_info;
#include <poppck.h>
#endif
