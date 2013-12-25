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


#include "targsys.h"

#define M_CJILONG       0x08ff
#define M_CJINEAR       0x00ff
#define B_IND_RMR_CALL  0x0010
#define B_IND_RMR_JMP   0x0020

#define M_RET           0x00c2
#define B_RET_LONG      0x0008
#define B_RET_IRET      0x000C
#define B_RET_NOPOP     0x0001

#define _OutShortD( addr )      OutDataByte( addr - AskLocation() - 1 )
#define _OutFarSeg( seg )       OutDataInt( seg )
#if _TARGET & _TARG_80386
    #define _OutOpndSize            if( _IsntTargetModel( USE_32 ) ) { \
                                        OutDataByte( 0x66 ); \
                                    }
    #define _OutFarOff( off )       OutDataLong( off );
    #define _OutNearD( addr )       OutDataLong( (addr)-AskLocation()-4 );
    #define _OutFarD( seg, off )    OutDataInt( seg ); \
                                    OutDataLong( (unsigned)off );
    #define _OFFSET_PATCH           LONG_PATCH
    #define _NEAR_PATCH             (ADD_PATCH | LONG_PATCH)
#else
    #define _OutOpndSize
    #define _OutFarD( seg, off )    OutDataInt( seg ); \
                                    OutDataInt( off )
    #define _OutFarOff( off )       OutDataInt( off )
    #define _OutNearD( addr )       OutDataInt( addr - AskLocation() - 2 )
    #define _OFFSET_PATCH           WORD_PATCH
    #define _NEAR_PATCH             (ADD_PATCH | WORD_PATCH)
#endif
#define _OutCCyp                OutDataByte( 0x0e ); \
                                _OutOpndSize; \
                                OutDataByte( 0xe8 )
#define _OutCNear               _OutOpndSize; \
                                OutDataByte( 0xe8 )
#define _OutJNear               _OutOpndSize; \
                                OutDataByte( 0xe9 )
#define _OutCFar                _OutOpndSize; \
                                OutDataByte( 0x9a )
#define _OutJFar                _OutOpndSize; \
                                OutDataByte( 0xea )
#define _OutJShort              OutDataByte( 0xeb )
#define _OutJCond( cond )       OutDataByte( 0x70 + cond )
#define _OutJCondNear( cond )   _OutOpndSize; \
                                OutDataByte( 0x0f ); \
                                OutDataByte( 0x80 + cond )

#define _SHORT_PATCH            (ADD_PATCH | BYTE_PATCH)
