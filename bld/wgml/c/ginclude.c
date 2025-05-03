/****************************************************************************
*
*                            Open Watcom Project
*
*  Copyright (c) 2004-2009 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  GML :include   processing
*
****************************************************************************/


#include "wgml.h"

#include "clibext.h"


/***************************************************************************/
/*  :IMBED   file='abc.gml'                                                */
/*  :INCLUDE file="abc.gml"                                                */
/*                                                                         */
/* :IMBED and :INCLUDE are eqivalent.                                      */
/*                                                                         */
/* The value of the required attribute file is used as the name of the     */
/* file to include.  The content of the included file is processed by      */
/* WATCOM Script/GML as if the data was in the original file.  This tag    */
/* provides the means whereby a document may be specified using a          */
/* collection of separate files.  Entering the source text into separate   */
/* files, such as one file for each chapter, may help in managing the      */
/* document.  if( the specified file does not have a file type, the        */
/* default document file type is used.  For example, if the main document  */
/* file is manual.doc, doc is the default document file type.  If the file */
/* is not found, the alternate extension supplied on the command line is   */
/* used.  If the file is still not found, the file type GML is used.  When */
/* working on a PC/DOS system, the DOS environment symbol GMLINC may be    */
/* set with an include file list.  This symbol is defined in the same way  */
/* as a library definition list, and provides a list of alternate          */
/* directories for file inclusion.  If an included file is not defined in  */
/* the current directory, the directories specified by the include path    */
/* list are searched for the file.  If the file is still not found, the    */
/* directories specified by the DOS environment symbol PATH are searched.  */
/*                                                                         */
/* NOTE: The attribute turns out to not, in face, be needed: the token     */
/*       after the tag is treated as a filename if it is not "file"        */
/*                                                                         */
/***************************************************************************/

extern  void    gml_include( const gmltag * entry )
{
    char    *   p;
    char    *   pa;

    (void)entry;

    *token_buf = '\0';
    p = scan_start;
    p++;
    SkipSpaces( p );
    if( *p == '.' ) {
        /* already at tag end */
    } else {
        pa = get_att_start( p );
        p = att_start;
        if( !ProcFlags.reprocess_line ) {
            if( strnicmp( "file", p, 4 ) == 0 ) {
                p += 4;
                p = get_att_value( p );
            } else {
                p = pa;                 // reset for possible file name
                p = get_tag_value( p );
            }
            if( val_start != NULL ) {
                if( val_len > _MAX_PATH - 1 )
                    val_len = _MAX_PATH - 1;
                strncpy( token_buf, val_start, val_len );
                token_buf[val_len] = '\0';
                ProcFlags.newLevelFile = 1;     // start new include level
                scan_start = scan_stop + 1;     // .. and ignore remaining line
            }
        } else {                                // wgml 4.0 appears to mark "" as the filename
            ProcFlags.newLevelFile = 1;         // start new include level
            scan_start = scan_stop + 1;         // .. and ignore remaining line
        }
    }

    return;
}

