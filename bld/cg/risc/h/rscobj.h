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


// section layout for OWL
typedef struct section_def section_def;

struct section_def {
    section_def         *next;
    seg_id              id;
    owl_func_handle     func;
    int                 line;
    int                 start;
    owl_section_handle  owl_handle;
    int                 is_start;
};
//Linker comments
#define  COMMENTV( a )  a,sizeof( a )-1
#define COFF_DRECTVE_DEFLIB "-defaultlib:"
#define COFF_DRECTVE_STACK  "-stack"
#define COFF_DRECTVE_HEAP   "-heap"
#define COFF_DRECTVE_EXPORT "-export"

extern section_def *FindSection( seg_id id );
extern section_def *AddSection( seg_id id );
extern owl_section_handle DbgSectDefComdat( char *str );
