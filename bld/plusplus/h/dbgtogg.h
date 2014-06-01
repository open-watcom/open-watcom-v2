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


                                // #pragma on/off( <toggle-name> )
toggle_pick( dump_tokens )      // - dump tokens as they are scanned
toggle_pick( dump_scopes )      // - dump scopes at end of compilation
toggle_pick( dump_emit_ic )     // - dump IC codes as they are emitted
toggle_pick( dump_exec_ic )     // - dump IC codes as they are executed
toggle_pick( dump_parse )       // - dump shift/reduce actions
toggle_pick( dump_ptree )       // - dump parse trees before/after analysis
toggle_pick( dump_types )       // - dump types as they are built
toggle_pick( dump_memory )      // - dump unfreed memory at end of compilation
toggle_pick( ic_trace_on )      // - inject a "start trace" ic code
toggle_pick( ic_trace_off )     // - inject an "end trace" ic code
toggle_pick( dump_noformat )    // - dump types as raw memory rather than formatted
toggle_pick( dump_rank )        // - dump ranking info during overloads
toggle_pick( dump_init )        // - dump data initialization steps
toggle_pick( dump_cdopt )       // - dump cdopt caches at end of compilation
toggle_pick( dump_labels )      // - dump front-end label processing
toggle_pick( dump_mptr )        // - dump member pointer analysis
toggle_pick( dump_mtokens )     // - dump macro structures
toggle_pick( dump_names )       // - dump names table at end of compilation
toggle_pick( print_ptree )      // - dump "pretty" parse trees rather than raw
toggle_pick( extref )           // - dump special external reference processing
toggle_pick( callgraph )        // - dump call graph analysis
toggle_pick( cdopt )            // - dump cdopt algorithm progress
toggle_pick( dump_data_dtor )   // - dump data destruction information
toggle_pick( dump_stab )        // - dump state tables
toggle_pick( dump_blk_posn )
toggle_pick( dump_auto_rel )
toggle_pick( dump_cg )          // - dump codegen calls
toggle_pick( callgraph_scan )
toggle_pick( auxinfo )          // - dump FEAuxInfo requests
toggle_pick( genned )
toggle_pick( ppexpn )
toggle_pick( dump_hash )        // - dump hashtab statistics for scopes
toggle_pick( dump_dups )        // - dump resolution of PT_DUP's
toggle_pick( no_mem_cleanup )   // - don't force memory cleanup after n allocs
toggle_pick( browse )           // - do browse-info dump
toggle_pick( dump_using_dir )   // - dump using directives
toggle_pick( dump_member_ptr )  // - dump member pointer conversion analysis
toggle_pick( dump_vftables )    // - dump vftable construction
toggle_pick( browse_emit )      // - do browse-emit dump
toggle_pick( browse_read )      // - do browse-read dump
toggle_pick( parser_states )    // - dump parser state numbers
toggle_pick( member_inst )      // - print template member instantiations
toggle_pick( templ_inst )       // - template instantiations
toggle_pick( templ_spec )       // - template specializations
toggle_pick( templ_function )   // - template functions
