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


#include "wpch.hpp"
#include "ppops.hpp"

struct StringPair {
    char const  *sysName;
    int         userName;
};

static const int        numOps = 45;

static StringPair OpTable[numOps] = {
    { ".@aa", 1 },
    { ".@ab", 2 },
    { ".@ac", 3 },
    { ".@ad", 4 },
    { ".@ae", 5 },
    { ".@af", 6 },
    { ".@ag", 7 },
    { ".@ah", 8 },
    { ".@ai", 9 },
    { ".@aj", 10 },
    { ".@ak", 11 },
    { ".@ct", CTOR_NAME },
    { ".@cv", CONV_NAME },
    { ".@da", 12 },
    { ".@dl", 13 },
    { ".@dt", DTOR_NAME },
    { ".@na", 14 },
    { ".@nw", 15 },
    { ".@oa", 16 },
    { ".@ob", 17 },
    { ".@oc", 18 },
    { ".@od", 19 },
    { ".@oe", 20 },
    { ".@of", 21 },
    { ".@og", 22 },
    { ".@oh", 23 },
    { ".@oi", 24 },
    { ".@oj", 25 },
    { ".@ok", 26 },
    { ".@ol", 27 },
    { ".@om", 28 },
    { ".@on", 29 },
    { ".@oo", 30 },
    { ".@op", 31 },
    { ".@oq", 32 },
    { ".@or", 33 },
    { ".@os", 34 },
    { ".@ot", 35 },
    { ".@ou", 36 },
    { ".@ra", 37 },
    { ".@rb", 38 },
    { ".@rc", 39 },
    { ".@rd", 40 },
    { ".@re", 41 },
    { ".@rf", 42 }
};

char const *OpNames[] = {
    NULL,
    "operator =",
    "operator *=",
    "operator +=",
    "operator -=",
    "operator /=",
    "operator %=",
    "operator >>=",
    "operator <<=",
    "operator &=",
    "operator |=",
    "operator ^=",
    "operator delete[]",
    "operator delete",
    "operator new[]",
    "operator new",
    "operator >>",
    "operator <<",
    "operator !",
    "operator []",
    "operator ->",
    "operator *",
    "operator ++",
    "operator --",
    "operator -",
    "operator +",
    "operator &",
    "operator ->*",
    "operator /",
    "operator %",
    "operator ,",
    "operator ()",
    "operator ~",
    "operator ^",
    "operator |",
    "operator &&",
    "operator ||",
    "operator ==",
    "operator !=",
    "operator <",
    "operator <=",
    "operator >",
    "operator >="
};

int NameOfOp( char const *sysName )
/*********************************/
{
    int         result;
    int         middle;
    int         left;
    int         right;
    int         comparison;

    result = 0;
    left = 0;
    right = numOps-1;
    while( left <= right ){
        middle = (left+right)/2;
        comparison = strcmp( OpTable[middle].sysName, sysName );
        if( comparison == 0 ){
            result = OpTable[middle].userName;
            break;
        } else if( comparison < 0 ){
            left = middle+1;
        } else {
            right = middle-1;
        }
    }

    return result;
}
