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
* Description:  Prototypes for patch.c; the DeadBeef stuff seems to be
*               truely dead beef.
*
****************************************************************************/


#if 0
extern void DeadBeef();
#pragma aux DeadBeef = 0xDE 0xAD 0xBE 0xEF;
#define Bytes_008 DeadBeef(); DeadBeef();
#define Bytes_016 Bytes_008 Bytes_008
#define Bytes_032 Bytes_016 Bytes_016
#define Bytes_064 Bytes_032 Bytes_032
#define Bytes_128 Bytes_064 Bytes_064
#define Bytes_256 Bytes_128 Bytes_128
#define Bytes_512 Bytes_256 Bytes_256
#define PatchArea() Bytes_512 Bytes_256
#endif

typedef struct {
#ifndef NDEBUG
    use_info    useinfo;
#endif
    union {
        tn              node;
        instruction     *ins;
    } u;
    bool        in_tree;        // are we hanging in a tree somewhere?
    bool        patched;        // has someone made a node out of us yet?
} patch;

extern  patch   *BGNewPatch( void );
extern  cg_name BGPatchNode( patch *hdl, type_def *tipe );
extern  void    BGPatchInteger( patch *hdl, signed_32 value );
extern  void    BGFiniPatch( patch *hdl );
extern  an      TNPatch( tn node );
extern  tn      TGPatch( patch *hdl, type_def *tipe );
