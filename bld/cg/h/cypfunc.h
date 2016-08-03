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


extern  void        *CypCopy( const void *, void *, uint );
extern  void        *CypFill( void *, uint, unsigned char );
extern  uint        CypLength( const char *);
extern  bool        CypEqual( const void *, const void *, uint );

#if defined(__WATCOMC__) && defined( _M_IX86 )

#if defined(__FLAT__) || defined(__SMALL__) || defined(__MEDIUM__)
    #define _SAVES  "push    es"
    #define _RESES  "pop     es"
    #define _SETES  "push    ds" \
                    "pop     es"
    #define __ES
    #define __DS
#else
    #define _SAVES
    #define _RESES
    #define _SETES
    #define __ES    es
    #define __DS    ds
#endif

#pragma aux CypCopy = \
        _SAVES \
        _SETES \
        "rep movsb" \
        _RESES \
        parm routine [__DS esi] [__ES edi] [ecx] \
        value [__ES edi]

#pragma aux CypFill = \
        _SAVES \
        _SETES \
        "rep stosb" \
        _RESES \
        parm routine [__ES edi] [ecx] [al] \
        value [__ES edi]

#pragma aux CypLength = \
        _SAVES \
        _SETES \
        "xor    eax,eax" \
        "xor    ecx,ecx" \
        "dec    ecx" \
        "repne scasb" \
        "not    ecx" \
        "dec    ecx" \
        _RESES \
        parm routine [__ES edi] \
        value [ecx] \
        modify[eax]

#pragma aux CypEqual = \
        _SAVES \
        _SETES \
        "xor    eax,eax" \
        "repe cmpsb" \
        "jne short L1" \
        "inc    eax" \
    "L1:" \
        _RESES \
        parm routine [__DS esi] [__ES edi] [ecx] \
        value [al]

#endif
