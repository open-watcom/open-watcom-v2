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


/*  evaluate location  expr*/

typedef enum {
    DR_LOC_NONE,
    DR_LOC_REG,
    DR_LOC_ADDR
} dr_loc_kind;

typedef struct {
    dr_loc_kind (*init)( void *, uint_32 * );                            //Init Stk
    int (*ref)( void *d, uint_32 ref, uint_32 size, dr_loc_kind kind );  //grab ref, size of kind
    int (*dref)( void *d, uint_32 *, uint_32, uint_32 );                 //dref addr
    int (*drefx)( void *d, uint_32 *, uint_32, uint_32, uint_16 );       //dref addr seg
    int (*frame)( void *d, uint_32 * );                                  //get frame val
    int (*reg)( void *d, uint_32 *, uint_16 );                           //get reg val
    int (*acon)( void *d, uint_32 *, bool );                             //address constant
    int (*live)( void *d, uint_32 * );                                   //get pc line range
}dr_loc_callbck_def;

typedef dr_loc_callbck_def const dr_loc_callbck;

extern int DRLocationAT( dr_handle              var,
                         dr_loc_callbck  *callbck,
                         void                  *d  );
extern int DRParmEntryAT( dr_handle              var,
                          dr_loc_callbck *callbck,
                          void                  *d  );
extern int DRLocBasedAT( dr_handle              var,
                         dr_loc_callbck  *callbck,
                         void                  *d  );
extern int DRRetAddrLocation( dr_handle             var,
                              dr_loc_callbck *callbck,
                              void                 *d  );
extern int DRSegLocation( dr_handle                 var,
                          dr_loc_callbck  *callbck,
                          void                     *d  );
extern dr_handle DRStringLengthAT( dr_handle str );
