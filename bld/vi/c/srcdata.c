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


#include <stdio.h>
#include "vi.h"
#include "source.h"

/*
 * language tokens
 */
char near SourceTokens[] = {
"EXPR\0"
"LABEL\0"
"IF\0"
"QUIF\0"
"ELSEIF\0"
"ELSE\0"
"ENDIF\0"
"LOOP\0"
"ENDLOOP\0"
"ENDWHILE\0"
"WHILE\0"
"UNTIL\0"
"BREAK\0"
"CONTINUE\0"
"GOTO\0"
"ASSIGN\0"
"RETURN\0"
"FOPEN\0"
"FREAD\0"
"FWRITE\0"
"FCLOSE\0"
"INPUT\0"
"ATOMIC\0"
"GET\0"
"VBJ__\0"
"NEXTWORD\0"
"\0"
};

char near StrTokens[] = {
"SUBSTR\0"
"STRLEN\0"
"STRCHR\0"
"\0"
};

char *ErrorTokens;
int *ErrorValues;
vars *VarHead,*VarTail;
long CurrentSrcLabel;
int CurrentSrcLine;
char *CurrentSrcData;
