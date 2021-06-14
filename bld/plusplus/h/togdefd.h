/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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
pick( dump_tokens )     // - dump tokens as they are scanned
pick( dump_scopes )     // - dump scopes at end of compilation
pick( dump_emit_ic )    // - dump IC codes as they are emitted
pick( dump_exec_ic )    // - dump IC codes as they are executed
pick( dump_parse )      // - dump shift/reduce actions
pick( dump_ptree )      // - dump parse trees before/after analysis
pick( dump_types )      // - dump types as they are built
pick( dump_memory )     // - dump unfreed memory at end of compilation
pick( ic_trace_on )     // - inject a "start trace" ic code
pick( ic_trace_off )    // - inject an "end trace" ic code
pick( dump_noformat )   // - dump types as raw memory rather than formatted
pick( dump_rank )       // - dump ranking info during overloads
pick( dump_init )       // - dump data initialization steps
pick( dump_cdopt )      // - dump cdopt caches at end of compilation
pick( dump_labels )     // - dump front-end label processing
pick( dump_mptr )       // - dump member pointer analysis
pick( dump_mtokens )    // - dump macro structures
pick( dump_names )      // - dump names table at end of compilation
pick( print_ptree )     // - dump "pretty" parse trees rather than raw
pick( extref )          // - dump special external reference processing
pick( callgraph )       // - dump call graph analysis
pick( cdopt )           // - dump cdopt algorithm progress
pick( dump_data_dtor )  // - dump data destruction information
pick( dump_stab )       // - dump state tables
pick( dump_blk_posn )
pick( dump_auto_rel )
pick( dump_cg )         // - dump codegen calls
pick( callgraph_scan )
pick( auxinfo )         // - dump FEAuxInfo requests
pick( genned )
pick( ppexpn )
pick( dump_hash )       // - dump hashtab statistics for scopes
pick( dump_dups )       // - dump resolution of PT_DUP's
pick( no_mem_cleanup )  // - don't force memory cleanup after n allocs
pick( browse )          // - do browse-info dump
pick( dump_using_dir )  // - dump using directives
pick( dump_member_ptr ) // - dump member pointer conversion analysis
pick( dump_vftables )   // - dump vftable construction
pick( browse_emit )     // - do browse-emit dump
pick( browse_read )     // - do browse-read dump
pick( parser_states )   // - dump parser state numbers
pick( member_inst )     // - print template member instantiations
pick( templ_inst )      // - template instantiations
pick( templ_spec )      // - template specializations
pick( templ_function )  // - template functions
