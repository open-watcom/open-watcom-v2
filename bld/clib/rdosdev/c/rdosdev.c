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
* Description:  RDOS device routines that cannot be inlined
*
****************************************************************************/

#include <memory.h>
#include <process.h>
#include "rdos.h"
#include "rdosdev.h"

#define FALSE 0

typedef struct RdosPtr48
{
    const char *offset;
    int         sel;
} TRdosPtr48;

typedef struct RdosSpawnParam
{
    TRdosPtr48 param;
    TRdosPtr48 startdir;
    TRdosPtr48 env;
    TRdosPtr48 options;
} TRdosSpawnParam;

int RdosCarryToBool();

#pragma aux RdosCarryToBool = \
    CarryToBool \
    value [eax];

void RdosBlitBase();

#pragma aux RdosBlitBase = \
    CallGate_blit;

void RdosDrawMaskBase();

#pragma aux RdosDrawMaskBase = \
    CallGate_draw_mask;

void RdosGetBitmapInfoBase();

#pragma aux RdosGetBitmapInfoBase = \
    CallGate_get_bitmap_info;

void RdosReadDirBase();

#pragma aux RdosReadDirBase = \
    CallGate_read_dir;

/*

void RdosCreateThread( void (*Startup)(void *Param), const char *Name, void *Param, int StackSize )
{
    _beginthread( Startup, Name, StackSize, Param );
}

void RdosCreatePrioThread( void (*Start)(void *Param), int Prio, const char *Name, void *Param, int StackSize )
{
    _beginthread( Start, Name, StackSize, Param );
}

*/

void RdosBlit( int SrcHandle, int DestHandle, int width, int height, int SrcX, int SrcY, int DestX, int DestY )
{
    __asm {
        mov esi,SrcX
        mov eax,SrcY
        shl eax,16
        or esi,eax
        mov edi,DestX
        mov eax,DestY
        shl eax,16
        or edi,eax
        mov eax,SrcHandle
        mov ebx,DestHandle
        mov ecx,width
        mov edx,height
    }
    RdosBlitBase();
}

void RdosDrawMask( int handle, void *mask, int RowSize, int width, int height, int SrcX, int SrcY, int DestX, int DestY )
{
    __asm {
        mov ebx,handle
        les edi,mask
        mov eax,RowSize
        mov esi,height
        mov eax,width
        shl eax,16
        or esi,eax
        mov ecx,SrcX
        mov eax,SrcY
        shl eax,16
        or ecx,eax
        mov edx,DestX
        mov eax,DestY
        shl eax,16
        or edx,eax
    }
    RdosDrawMaskBase();
}

void RdosGetBitmapInfo( int handle, int *BitPerPixel, int *width, int *height, int *linesize, void **buffer )
{
    __asm {
        mov ebx,handle
    }
    RdosGetBitmapInfoBase();
    __asm {
        push ds
        lds ebx,BitPerPixel
        movzx eax,al
        mov [ebx],eax
        lds ebx,width
        movzx ecx,cx
        mov [ebx],ecx
        lds ebx,height
        movzx edx,dx
        mov [ebx],edx
        lds ebx,linesize
        movzx esi,si
        mov [ebx],esi
        lds ebx,buffer
        mov [ebx],edi
        mov [ebx+4],es
        pop ds        
    }
}

int RdosReadDir( int Handle, int EntryNr, int MaxNameSize, char *PathName, long *FileSize, int *Attribute, unsigned long *MsbTime, unsigned long *LsbTime )
{
    int val;
    
    __asm {
        mov ebx,Handle
        mov edx,EntryNr
        mov ecx,MaxNameSize
        les edi,PathName
    }
    RdosReadDirBase();
    __asm {
        push ds
        lds esi,FileSize
        mov [esi],ecx
        movzx ebx,bx
        lds esi,Attribute
        mov [esi],ebx
        lds esi,MsbTime
        mov [esi],edx
        lds esi,LsbTime
        mov [esi],eax
        pop ds
    }
    val = RdosCarryToBool();
    return( val );
}
