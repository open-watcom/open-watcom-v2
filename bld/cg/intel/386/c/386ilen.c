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
* Description:  Instuction size calculation and NOP padding.
*
****************************************************************************/


#include "cgstd.h"
#include "cg.h"
#include "ocentry.h"
#include "model.h"


static byte NopList1[] = {
    6,
    0x8d,0x80,0x00,0x00,0x00,0x00,  // lea     eax,+00000000H[eax]
    0x8d,0x40,0x00,                 // lea     eax,+00H[eax]
    0x8b,0xc9,                      // mov     ecx,ecx
    0x8d,0x44,0x20,0x00,            // lea     eax,+00H[eax+no_index_reg]
    0x8d,0x40,0x00,                 // lea     eax,+00H[eax]
    0x8b,0xc0,                      // mov     eax,eax
    0x90                            // nop
};

static byte NopList2[] = {
    6,
    0x8d,0x92,0x00,0x00,0x00,0x00,  // lea     edx,+00000000H[edx]
    0x8d,0x52,0x00,                 // lea     edx,+00H[edx]
    0x8b,0xdb,                      // mov     ebx,ebx
    0x8d,0x54,0x22,0x00,            // lea     edx,+00H[edx+no_index_reg]
    0x8d,0x52,0x00,                 // lea     edx,+00H[edx]
    0x8b,0xd2,                      // mov     edx,edx
    0x90                            // nop
};

byte *NopLists[] = { NopList1, NopList2 };

static  byte    InsSize[ 6 ][ OC_DEST_FAR+1 ] = {
/*      OC_DEST_SHORT   OC_DEST_NEAR    OC_DEST_CHEAP   OC_DEST_FAR */
{       0,              6,              7,              8 },    /* CALL,16 */
{       0,              5,              6,              7 },    /* CALL,32 */
{       2,              6,              0,              8 },    /* JMP,16 */
{       2,              5,              0,              7 },    /* JMP,32 */
{       2,              7,              0,              0 },    /* JCOND,16 */
{       2,              6,              0,              0 }     /* JCOND,32 */
};


extern  int     OptInsSize( oc_class class, oc_dest_attr attr )
/**************************************************************
    return the object code size of a given call/branch with a
    given attribute (short/near/far)
*/
{
    int         i;

    i = 0;
    switch( class ) {
    case OC_CALL:
        break;
    case OC_JMP:
        i = 2;
        break;
    case OC_JCOND:
        i = 4;
        break;
    }
    if( _IsTargetModel( USE_32 ) ) ++i;
    return( InsSize[ i ][ attr ] );
}
