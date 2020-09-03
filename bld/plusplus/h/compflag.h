/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2020 The Open Watcom Contributors. All Rights Reserved.
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

    boolbit     float_used                      : 1;
    boolbit     stats_printed                   : 1;
    boolbit     low_on_memory_printed           : 1;
    boolbit     external_defn_found             : 1;
    boolbit     scanning_c_comment              : 1;    // scanning ANSI style (/**/) comment
    boolbit     srcfile_compiled                : 1;    // source file has been compiled
    boolbit     errfile_opened                  : 1;    // error file has been opened
    boolbit     errfile_written                 : 1;    // something has been written
    boolbit     main_has_parms                  : 1;    /* on if "main" has parm(s) */
    boolbit     pgm_used_8087                   : 1;    /* on => 8087 ins. generated */
    boolbit     banner_printed                  : 1;    /* on => banner printed      */
    boolbit     cppi_segment_used               : 1;    /* C++ initializer segment */
    boolbit     rescan_buffer_done              : 1;    /* ## re-scan buffer used up */
    boolbit     has_winmain                     : 1;    // WinMain() is defined
    boolbit     has_libmain                     : 1;    // LibMain() is defined
    boolbit     has_dllmain                     : 1;    // DllMain() is defined
    boolbit     has_main                        : 1;    // main() is defined
    boolbit     has_longjmp                     : 1;    // has longjmp reference
    boolbit     extern_C_defn_found             : 1;    // something was declared as "C"
    boolbit     inline_intrinsics               : 1;    // inline intrinsic fns
    boolbit     scanning_cpp_comment            : 1;    // scanning a C++ style comment
    boolbit     pragma_library                  : 1;    // on => behave as if main seen
    boolbit     parsing_finished                : 1;    // program has been completely parsed
    boolbit     genned_static_dtor              : 1;    // genned DTOR of static var.
    boolbit     inline_fun_reg                  : 1;    // inline func. registration gen'ed
    boolbit     fatal_error                     : 1;    // fatal error has occured
    boolbit     codegen_active                  : 1;    // code generator is active
    boolbit     dt_method_pragma                : 1;    // pragma destruct encountered
    boolbit     dll_subsequent                  : 1;    // DLL called second time thru
    boolbit     dll_active                      : 1;    // DLL version of compiler
    boolbit     compile_failed                  : 1;    // compilation failed
    boolbit     cmdline_error                   : 1;    // error in cmd line
    boolbit     watch_for_pcheader              : 1;    // watch for first #include
    boolbit     pch_debug_info_write            : 1;    // writing dbg info into PCH
    boolbit     pch_debug_info_read             : 1;    // reading dbg info from PCH
    boolbit     batch_file_primary              : 1;    // batch file found: primary scan
    boolbit     batch_file_processing           : 1;    // processing the batch file
    boolbit     batch_file_eof                  : 1;    // EOF on the batch file
    boolbit     batch_file_continue             : 1;    // continue on errors
    boolbit     in_pragma                       : 1;    // scanning in a #pragma
    boolbit     ide_cmd_line_has_files          : 1;    // ide command line ok as is
    boolbit     ide_console_output              : 1;    // ide doing output to console
    boolbit     use_macro_tokens                : 1;    // macro expansion is token source
    boolbit     log_note_msgs                   : 1;    // on ==> NOTE_MSGs to error file
    boolbit     has_wide_char_main              : 1;    // has wide char version of "main" fn
    boolbit     vfun_reference_done             : 1;    // extrf: -zv record generated
    boolbit     namespace_checks_done           : 1;    // namespace ref/def checks done

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

    boolbit     extra_stats_wanted              : 1;

/*****************************************************************************
//                                                                          //
//  'any' target flags                                                      //
//                                                                          //
*****************************************************************************/

    boolbit     signed_char                     : 1;
    boolbit     check_syntax                    : 1;
    boolbit     extensions_enabled              : 1;
    boolbit     inline_functions                : 1;    // inline user def'd 'inline' fns
    boolbit     dump_prototypes                 : 1;    // output prototypes into .DEF file
    boolbit     use_base_types                  : 1;    // use base types in .DEF protos
    boolbit     quiet_mode                      : 1;
    boolbit     cpp_keep_comments               : 1;    // wcpp - output comments
    boolbit     emit_library_names              : 1;    // on => put LIB name in obj
    boolbit     undefine_all_macros             : 1;    // on => -u all macros
    boolbit     cpp_line_wanted                 : 1;    // wcpp - emit #line
    boolbit     cpp_ignore_line                 : 1;    /* wcpp - ignore #line */
    boolbit     cpp_output                      : 1;    // compiler doing CPP output
    boolbit     cpp_output_to_file              : 1;    // compiler doing CPP output to file (default extension .i)
    boolbit     cpp_mode                        : 1;    // compiler CPP mode
    boolbit     no_debug_type_names             : 1;
    boolbit     emit_names                      : 1;
    boolbit     warnings_cause_bad_exit         : 1;
    boolbit     use_unicode                     : 1;
    boolbit     unique_functions                : 1;
    boolbit     jis_to_unicode                  : 1;
    boolbit     emit_dependencies               : 1;
    boolbit     emit_targimp_symbols            : 1;    // Insert target specific import symbols
    boolbit     emit_browser_info               : 1;
    boolbit     excs_enabled                    : 1;    // exceptions enabled
    boolbit     rtti_enabled                    : 1;    // RTTI enabled
    boolbit     extended_defines                : 1;    // -d swallows many tokens
    boolbit     virtual_stripping               : 1;    // enable virtual stripping
    boolbit     returns_promoted                : 1;    // promote arg,return to int
    boolbit     all_debug_type_names            : 1;
    boolbit     fhw_switch_used                 : 1;
    boolbit     fhr_switch_used                 : 1;
    boolbit     modifier_bind_compatibility     : 1; // bind modifiers into generic types
    boolbit     prototype_instantiate           : 1;    // instantiate fn prototypes if possible
    boolbit     encrypt_preproc_output          : 1;    // try to encrypt compiler output
    boolbit     check_truncated_fnames          : 1;    // if can't find file, check 8.3
    boolbit     make_enums_an_int               : 1;    // don't overly optimize enum storage
    boolbit     use_pcheaders                   : 1;    // pre-compiled headers are active
    boolbit     no_pch_warnings                 : 1;    // don't explain PCH activity
    boolbit     pch_debug_info_opt              : 1;    // optimize dbg info in PCH
    boolbit     pch_min_check                   : 1;    // assume PCH header files haven't changed
    boolbit     bd_switch_used                  : 1;
    boolbit     bm_switch_used                  : 1;
    boolbit     bw_switch_used                  : 1;
    boolbit     ee_switch_used                  : 1;
    boolbit     ep_switch_used                  : 1;
    boolbit     eq_switch_used                  : 1;
    boolbit     ew_switch_used                  : 1;
    boolbit     op_switch_used                  : 1;
    boolbit     br_switch_used                  : 1;
    boolbit     rw_registration                 : 1;    // RW-centric function registration
    boolbit     original_enum_setting           : 1;    // original -ei setting
    boolbit     ignore_environment              : 1;    // don't process WPP* and INCLUDE
    boolbit     ignore_current_dir              : 1;    // ignore current dir for include files
    boolbit     progress_messages               : 1;    // output msgs as compile progresses
    boolbit     no_error_sym_injection          : 1;    // don't insert error symbols
    boolbit     error_use_full                  : 1;    // use full path names in error msgs
    boolbit     vc_alloca_parm                  : 1;    // allow alloca to occur in parms
    boolbit     emit_all_default_libs           : 1;    // emit default libs in .OBJ file
    boolbit     bc_switch_used                  : 1;    // building a console app
    boolbit     bg_switch_used                  : 1;    // building a GUI app
    boolbit     optbr_v                         : 1;    // browse: variables
    boolbit     optbr_f                         : 1;    // browse: functions
    boolbit     optbr_t                         : 1;    // browse: types
    boolbit     optbr_m                         : 1;    // browse: data members
    boolbit     optbr_p                         : 1;    // browse: preprocessor macros
    boolbit     static_inline_fns               : 1;    // emit inline fns in normal code seg
    boolbit     fhwe_switch_used                : 1;    // display but don't treat as errors
    boolbit     warn_about_padding              : 1;    // warn when padding is added
    boolbit     overload_13332                  : 1;    // implement WP 13.3.3.2 better
    boolbit     cpp_line_comments               : 1;    // output // #line instead of #line
    boolbit     dont_align_segs                 : 1;    // bug fix: don't align segments
    boolbit     fixed_name_mangling             : 1;    // bug fix: name mangling
    boolbit     plain_char_promotion            : 1;    // char -> int rvalue promotion warning
    boolbit     obfuscate_typesig_names         : 1;    // mangle type sig names
    boolbit     dont_autogen_ext_inc            : 1;    // don't convert #include <string> to <string.h> or <string.hpp>
    boolbit     dont_autogen_ext_src            : 1;
    boolbit     use_old_for_scope               : 1;
    boolbit     no_alternative_tokens           : 1;    // disable alternative tokens
    boolbit     enable_std0x                    : 1;    // enable some C++0x features
    boolbit     generate_auto_depend            : 1;
    boolbit     ignore_fnf                      : 1;
    boolbit     cpp_ignore_env                  : 1;    // ignore include path env vars
    boolbit     ignore_default_dirs             : 1;    // ignore all default dirs for file search (., ../h, ../c, ...)
    boolbit     non_iso_compliant_names_enabled : 1;    // enable all compiler non-ISO compliant names (macros, symbols, etc.)

/*****************************************************************************
//                                                                          //
// 'i86' '386' target flags
//                                                                          //
*****************************************************************************/

    boolbit     register_conventions            : 1;    // on for -3r, off for -3s
    boolbit     strings_in_code_segment         : 1;    // on => put strings in CODE
    boolbit     save_restore_segregs            : 1;    // save/restore segment regs
    boolbit     target_multi_thread             : 1;    // multiple execution threads
    boolbit     use_stdcall_at_number           : 1;    // mangle __stdcall names as "_*@n"
    boolbit     fs_registration                 : 1;    // using fs for function registration
    boolbit     sg_switch_used                  : 1;
    boolbit     st_switch_used                  : 1;
    boolbit     zc_switch_used                  : 1;
    boolbit     zm_switch_used                  : 1;
    boolbit     zmf_switch_used                 : 1;
    boolbit     zo_switch_used                  : 1;
    boolbit     zu_switch_used                  : 1;
    boolbit     zx_switch_used                  : 1;
    boolbit     mfi_switch_used                 : 1;    // flat model interrupts (ss stays same)

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

#endif
