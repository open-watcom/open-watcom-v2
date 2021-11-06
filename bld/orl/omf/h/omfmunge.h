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
* Description:  OMF munging public routines.
*
****************************************************************************/


#ifndef OMF_MUNGE_INCLUDED
#define OMF_MUNGE_INCLUDED

#include "omfglbl.h"
#include "omftype.h"

extern orl_return       OmfAddLName( omf_file_handle ofh, const char *buffer, omf_string_len len );

extern orl_return       OmfAddLIData( omf_file_handle ofh, bool is32,
                                      omf_idx seg, omf_sec_offset offset,
                                      omf_bytes buffer, omf_rec_size len, bool comdat );

extern orl_return       OmfAddLEData( omf_file_handle ofh, bool is32,
                                      omf_idx seg, omf_sec_offset offset,
                                      omf_bytes buffer, omf_rec_size len, bool comdat );

extern orl_return       OmfAddFixupp( omf_file_handle ofh, bool is32, bool mode,
                                      omf_fix_loc fix_loc, omf_sec_offset offset,
                                      ORL_STRUCT( omf_thread_fixup ) *fthread,
                                      ORL_STRUCT( omf_thread_fixup ) *tthread,
                                      omf_sec_addend disp );

extern orl_return       OmfAddBakpat( omf_file_handle ofh, unsigned_8 loctype,
                                      omf_sec_offset location, omf_idx segidx,
                                      omf_idx symidx, omf_sec_addend disp );

extern orl_return       OmfAddExtDef( omf_file_handle ofh, const char *name, omf_string_len len, omf_rectyp typ );

extern orl_return       OmfAddComDat( omf_file_handle ofh, bool is32, int flags,
                                      int attr, int align,
                                      omf_sec_offset offset, omf_idx seg,
                                      omf_idx group, omf_frame frame,
                                      omf_idx name, omf_bytes buffer, omf_rec_size len,
                                      omf_rectyp typ );

extern orl_return       OmfAddSegDef( omf_file_handle ofh, bool is32, orl_sec_alignment align,
                                      int combine, bool use32, bool max_size, bool abs_frame,
                                      omf_frame frame, omf_sec_size size, omf_idx name, omf_idx class );

extern orl_return       OmfAddPubDef( omf_file_handle ofh, bool is32,
                                      omf_idx group, omf_idx seg,
                                      omf_frame frame, const char *buffer, omf_string_len len,
                                      omf_sec_offset offset, omf_rectyp typ );

extern orl_return       OmfAddGrpDef( omf_file_handle ofh, omf_idx name, omf_idx *segs, unsigned num_segs );

extern orl_return       OmfModEnd( omf_file_handle ofh );

extern orl_return       OmfAddComment( omf_file_handle ofh, unsigned_8 class, unsigned_8 flags, omf_bytes buff, omf_rec_size len );

extern orl_return       OmfAddLineNum( omf_sec_handle sh, unsigned_16 line, unsigned_32 offset );

extern omf_sec_handle   OmfFindSegOrComdat( omf_file_handle ofh, omf_idx seg, omf_idx comdat_lname );

extern omf_string       OmfGetLName( omf_sec_handle lnames, omf_idx idx );

extern omf_string       OmfGetLastExtName( omf_file_handle ofh );

extern orl_return       OmfExportSegmentContents( omf_sec_handle sh );

extern orl_return       OmfTheadr( omf_file_handle ofh );

#endif
