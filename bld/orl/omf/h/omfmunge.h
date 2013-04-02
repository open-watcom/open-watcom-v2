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
* Description:  OMF munging public routines.
*
****************************************************************************/


#ifndef OMF_MUNGE_INCLUDED
#define OMF_MUNGE_INCLUDED

#include "omfglbl.h"
#include "omftype.h"

extern orl_return       OmfAddLIData( omf_file_handle ofh, int is32,
                                      omf_idx seg, orl_sec_offset offset,
                                      omf_bytes buffer, omf_rec_size len, int comdat );

extern orl_return       OmfAddLEData( omf_file_handle ofh, int is32,
                                      omf_idx seg, orl_sec_offset offset,
                                      omf_bytes buffer, omf_rec_size len, int comdat );

extern orl_return       OmfAddLName( omf_file_handle ofh, char *buffer, omf_string_len len, omf_rectyp typ );

extern orl_return       OmfAddExtName( omf_file_handle ofh, char *buffer, omf_string_len len, omf_rectyp typ );

extern orl_return       OmfAddFixupp( omf_file_handle ofh, int is32, int mode,
                                      int location, orl_sec_offset offset,
                                      int fmethod, omf_idx fidx, int tmethod,
                                      omf_idx tidx, orl_sec_offset disp );

extern orl_return       OmfAddBakpat( omf_file_handle ofh, uint_8 loctype,
                                      orl_sec_offset location, omf_idx segidx,
                                      omf_idx symidx, orl_sec_offset disp );

extern orl_return       OmfAddExtDef( omf_file_handle ofh, omf_string_struct *name, omf_rectyp typ );

extern orl_return       OmfAddComDat( omf_file_handle ofh, int is32, int flags,
                                      int attr, int align,
                                      orl_sec_offset offset, omf_idx seg,
                                      omf_idx group, omf_frame frame,
                                      omf_idx name, omf_bytes buffer, omf_rec_size len,
                                      omf_rectyp typ );

extern orl_return       OmfAddSegDef( omf_file_handle ofh, int is32,
                                      orl_sec_alignment align, int combine,
                                      int use32, int max, orl_sec_frame frame,
                                      orl_sec_size size, omf_idx name,
                                      omf_idx class );

extern orl_return       OmfAddPubDef( omf_file_handle ofh, int is32,
                                      omf_idx group, omf_idx seg,
                                      omf_frame frame, char *buffer, omf_string_len len,
                                      orl_sec_offset offset, omf_rectyp typ );

extern orl_return       OmfAddGrpDef( omf_file_handle ofh, omf_idx name, omf_idx *segs, int size );

extern orl_return       OmfModEnd( omf_file_handle ofh );

extern orl_return       OmfAddComment( omf_file_handle ofh, uint_8 class, uint_8 flags, omf_bytes buff, omf_rec_size len );

extern orl_return       OmfAddLineNum( omf_sec_handle sh, unsigned_16 line, unsigned_32 offset );

extern omf_sec_handle   OmfFindSegOrComdat( omf_file_handle ofh, omf_idx seg, omf_idx comdat_lname );

extern omf_string_struct *OmfGetLName( omf_sec_handle lnames, omf_idx idx );

extern omf_string_struct *OmfGetLastExtName( omf_file_handle ofh );

extern orl_return       OmfExportSegmentContents( omf_sec_handle sh );

extern orl_return       OmfTheadr( omf_file_handle ofh );

#endif
