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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


// INITSPEC.H -- INITFINI specification for C++ compile
//
// This is not #ifdef guarded because it is included under controlled
// conditions by appropriate header and source files.




// An EXIT_BEG/EXIT_END pair establishes the duration of the associated
// data/process.
//
// phase:
// -----
//
// start                cpp
//                      *
// cmd-line             *   cpp_front_end
//                      *   *
// pre-process          *   *   cpp_preproc cpp_preproc_only
// only                 *   *   *           *
//                      *   *
// analysis             *   *   cpp_preproc cpp_object cpp_analysis
//                      *   *   *           *          *
// code-generation      *   *   cgback      *
//                      *   *   *           *
// end                  *   *
//                      *   *
//
// SPLIT_INIT entries cause completion at the parameterized exit point.

EXIT_BEG( mem_management )      // memory manager
  EXIT_REG( memmgr )
EXIT_END

// constraints:
// - file_names must end after io_support so that the temp file filename
//   will not be freed before io_support has a chance to delete the temp file
// - dump_reports must end before io_support since it now uses io_support
//   for output purposes
// - extra_reports must start before names since names registers some counters
// - pchdrs must init before the command line is parsed
// - pstk must be just after mem_cleanup
EXIT_BEG( cpp )                 // entire compiler
  EXIT_REG( mem_cleanup )
  EXIT_REG( pstk )
#ifdef OPT_BR
  EXIT_REG( browse_macros )
#endif
  EXIT_REG( cmdlnany )
  EXIT_REG( context )
#ifdef XTRA_RPT
  EXIT_REG( extra_reports )
  EXIT_REG( keyword )
#endif
  EXIT_REG( names )
  EXIT_REG( cpp_names )
  EXIT_REG( rtf_names )
  EXIT_REG( pchdrs )
  EXIT_REG( h_files )
  EXIT_REG( file_names )
  EXIT_REG( io_support )
#ifdef XTRA_RPT
  EXIT_REG( dump_reports )
  EXIT_REG( analyse_reports )
  EXIT_REG( rpt_rankings )
  EXIT_REG( cnv_reports )
#endif
  EXIT_REG( error_file )
EXIT_END

// constraints:
// - statistics must end after macros because macros may generate some errors
// - browse_info must precede just about everything because just about anything
//   can generate browsing information
EXIT_BEG( cpp_front_end )       // cmd line, pre-processing, analysis, codegen
  EXIT_REG( cgio )
  SPLIT_INIT( statistics, cpp_analysis )
#ifdef OPT_BR
  EXIT_REG( browse_info )
#endif
  SPLIT_INIT( macros, cpp_analysis )
  EXIT_REG( rtn_gen )
  EXIT_REG( fun_repository )
  EXIT_REG( srcfile )
  EXIT_REG( rewriting )
  EXIT_REG( types )
  EXIT_REG( type_compare )
#ifdef XTRA_RPT
  EXIT_REG( repos_rpts )
#endif
EXIT_END

EXIT_BEG( cpp_preproc )         // pre-processing generally
  EXIT_REG( pragmas )
  EXIT_REG( pragma_extref )
  EXIT_REG( ppexpn )
#if _INTEL_CPU
  EXIT_REG( assembler )
#endif
EXIT_END

EXIT_BEG( cpp_preproc_only )    // pre-processing only
EXIT_END

EXIT_BEG( cpp_analysis )        // analysis
  EXIT_REG( front_end )
  EXIT_REG( labels )
  EXIT_REG( overload_operator )
  EXIT_REG( defarg )
EXIT_END

// constraints:
// - cg_typing must execute after the command line is processed
// - dbg must be before scopes
EXIT_BEG( cpp_object )          // analysis, code generation
#ifndef NDEBUG
  EXIT_REG( dbg )
#endif
  EXIT_REG( global_stack )
  EXIT_REG( argument_lists )
  EXIT_REG( dbg_supp )
  EXIT_REG( const_pool )
  EXIT_REG( cd_opt )
  EXIT_REG( cg_typing )
  EXIT_REG( extref_86 )
  EXIT_REG( type_signature )
  EXIT_REG( rtti_descriptors )
  EXIT_REG( parse_tree )
  EXIT_REG( symbol_locations )
  EXIT_REG( parse_tree_decoration )
  EXIT_REG( parser )
  EXIT_REG( hashing )
  EXIT_REG( type_formatting )
  EXIT_REG( functions )
  EXIT_REG( linkage )
  EXIT_REG( scopes )
  EXIT_REG( strings )
  EXIT_REG( template )
  EXIT_REG( fn_overload )
  EXIT_REG( data_init )
  EXIT_REG( node_promo )
  EXIT_REG( segment )
  //EXIT_REG( access_filter )
#ifndef NDEBUG
  EXIT_REG( comma_insertion )
#endif
EXIT_END


EXIT_BEG( cgback )              // code generation
  EXIT_REG( auto_rel )
  EXIT_REG( blk_posn )
  EXIT_REG( cg_back )
  EXIT_REG( cg_call_back )
  EXIT_REG( cg_cmds )
  EXIT_REG( cg_cdtor )
  EXIT_REG( cg_decl )
  EXIT_REG( cg_info )
  EXIT_REG( call_stab )
  EXIT_REG( call_stack )
  EXIT_REG( cond_label )
  EXIT_REG( fn_ctl )
  EXIT_REG( fun_registration )
  EXIT_REG( ibp )
  EXIT_REG( state_table )
  EXIT_REG( sym_trans )
  EXIT_REG( throw )
  EXIT_REG( type_sigs )
  EXIT_REG( cg_switch )
  EXIT_REG( cg_expr )
  EXIT_REG( conditional_blocks )
  EXIT_REG( cg_obj_init )
EXIT_END
