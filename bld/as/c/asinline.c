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


#include "as.h"

#ifdef _STANDALONE_
#error For inline-assembler only!
#endif

extern void AsmError( char * );
extern void AsLexerFini( void );
extern int asyyparse( void );


char *AsmInStr;


extern void AsmInit( void ) {
//***************************

    SymInit();
    InsInit();
    DirInit();
    ObjInit();
}

extern void AsmFini( void ) {
//***************************

    ObjFini();
    DirFini();
    InsFini();
    SymFini();
}

extern int AsmLine( char *in_str ) {
//***********************************
    int ret;

    AsmInStr = in_str;
    if( !asyyparse() ) {
        ret = FALSE;
    } else {
        ret = TRUE;
    }
    AsLexerFini();
    return( ret );
}
