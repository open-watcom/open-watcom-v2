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


#include "incl1.h"  // Include 1
#include <incl2.h>  // Include 2
#include "incl3.h"  // Include 3

#define INT int  /* Comment 1 */
/* Comment 2 */

#define TYPE INT
#define DECLARE_C   TYPE /* comment 3 */     c // comment 4
int a; DECLARE_C; TYPE b;

int another_and_last_declaration;
#include <incl1.h>

#define VOID void
#define DECL_D                  int unsigned  long   d1 ,d2;
int errorInclMain2 =;errorInclMain2;
struct s1_name {
    int unsigned i;
    float f;
    struct inside_s1_name {
        long char ch;
        VOID x1;
        VOID x2;
        DECL_D
    } inside_s1;
    struct s_name inside_s2;
//    float f2 = 1.12*23 >> 2 +   23 & (!!~23) ^ 9 - -1
//          + 23;  float f3, f4;

}  s1;

struct s2;
struct {
    int i;
} s3;
