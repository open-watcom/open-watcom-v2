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


#define G_GROUP1        G_RC
#define G_GROUP2        G_WORDR1
#define G_GROUP3        G_R1
#define G_GROUP4        G_1SHIFT
#define G_GROUP5        G_RR1
#define G_GROUP6        G_LEA
#define G_GROUP7        G_SEGR1
#define G_GROUP8        G_RRFBIN
#define G_GROUP9        G_MFLD
#define G_OTHER         G_REPOP

#define FIRST_REDUCT    (G_UNKNOWN+1)

#define _GenIs8087( g )  ( ( (g) >= FIRST_8087_1 && (g) <= LAST_8087_1 ) \
                        || ( (g) >= FIRST_8087_2 && (g) <= LAST_8087_2 ) )
