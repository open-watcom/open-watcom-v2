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


#ifdef __cplusplus

extern void MapFrameToPC( IRemoteStackFrame * frame, addr48_ptr *addr );
extern void     GetPCFromThread( IRemoteThread *, addr48_ptr * );

extern "C" {
#endif

typedef enum {
    OBJECT_CODE,
    OBJECT_IDUNNO
} ObjType;

extern unsigned ReadMemory( addr48_ptr *, void *, unsigned );
extern unsigned WriteMemory( addr48_ptr *, void *, unsigned );
extern void     GetPC( addr48_ptr * );
extern void InitMappings();
extern void FiniMappings();
extern IUnknown *FindObject( addr48_ptr *addr, ObjType *objtype );

#ifdef __cplusplus
}
#endif
