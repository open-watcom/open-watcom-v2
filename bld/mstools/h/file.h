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


#ifndef _FILE_H
#define _FILE_H


/*
 * Defines related to various types of files.
 */
#define TYPE_DEFAULT_FILE       0
#define TYPE_C_FILE             1
#define TYPE_CPP_FILE           2
#define TYPE_DEF_FILE           3
#define TYPE_LIB_FILE           4
#define TYPE_OBJ_FILE           5
#define TYPE_RC_FILE            6
#define TYPE_RES_FILE           7
#define TYPE_RBJ_FILE           8
#define TYPE_RS_FILE            9
#define TYPE_EXP_FILE           10

#define TYPE_INVALID_FILE       (-1)


/*
 * Function prototypes.
 */
extern void     AddFile( int type, const char *filename );
extern char *   GetNextFile( int *typeBuf, int type, ... );

extern void     SetDefaultFile( int type, const char *name );
extern void     AllowTypeFile( int type, ... );


#endif
