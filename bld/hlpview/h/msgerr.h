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


//
//  msgerr.h    --errors for the Watcom .HLP compiler.
//

_MSGERR( USAGE, "Usage: whc [/q] {Project Filename|RTF filename}\n" ),
_MSGERR( BAD_EXT, "Project filenames can't end in \".HLP\" or \".PH\".\n" ),

_MSGERR( PROGRAM_STOPPED, "The program was terminated abnormally.\n" ),

_MSGERR( UNKNOWN_IMAGE, "The image file %s (at line %d of %s) is of an unknown format.\n" ),

_MSGERR( MEM_ERR, "Can't allocate memory!\n" ),
_MSGERR( BOUND_ERR, "BUG: An internal array bounds error has occured.\n" ),
_MSGERR( BTREE_ERR, "BUG: An internal b-tree error has occured.\n" ),
_MSGERR( FILE_ERR, "Unable to open file %s.\n" ),
_MSGERR( INCLUDE_ERR, "Unable to include file %s (from line %d of %s).\n" ),
_MSGERR( SETFONT_ERR, "BUG: Attempted to jump to a non-existent font number.\n" ),

_MSGERR( HLP_NOTOPIC, "The topic %s was referenced but not defined.\n" ),
_MSGERR( HLP_ATTR, "Internal error while handling text attributes.\n" ),
_MSGERR( HLP_BADIMAGE, "The image file %s is not in a recognized format.\n" ),

_MSGERR( HPJ_NOTSECTION, "Expecting a section header at line %d of %s.\n" ),
_MSGERR( HPJ_BADSECTION, "Unknown section header at line %d of %s.\n" ),
_MSGERR( HPJ_NOARG, "\"= <argument>\" is required at line %d of %s.\n" ),
_MSGERR( HPJ_BADDIR,    "Bad directory name: %s\n" ),
_MSGERR( HPJ_BADINCLUDE,    "Bad #include directive at line %d of %s.\n" ),
_MSGERR( HPJ_INC_JUNK,  "Bad text at line %d of MAP file %s.\n" ),
_MSGERR( HPJ_NOFILES,   "No RTF files were specified, can't continue.\n" ),
_MSGERR( HPJ_RUNONCOMMENT, "A comment (at line %d of %s) never ended.\n" ),
_MSGERR( HPJ_INCOMPLETEWIN, "Incomplete window definition at line %d of %s.\n" ),
_MSGERR( HPJ_LONGWINNAME, "The window name at line %d of %s is too long and will be truncated.\n" ),
_MSGERR( HPJ_WINBADPARAM, "Window parameters must lie between 0 and 1023. (line %d of %s)\n" ),
_MSGERR( HPJ_WINBADCOLOR, "Window color values must be between 0 and 255. (line %d of %s)\n" ),

_MSGERR( SYS_NOCONTENTS,    "The topic defined by the CONTENTS instruction was never defined!\n" ),
_MSGERR( FONT_SAMENUM,  "Two fonts with the number %d have been defined.\n" ),
_MSGERR( FONT_SAMENAME, "Two fonts with the name %s have been defined.\n" ),
_MSGERR( FONT_NEWFAM,   "The font %s has been defined differently in two RTF files.\n" ),
_MSGERR( FONT_NONUM,    "A \\f command at line %d of %s does not have a font number.\n" ),
_MSGERR( FONT_CUTOFF,   "A font definition at line %d of %s is incomplete.\n" ),
_MSGERR( FONT_UNKNOWN,  "Font number %d is not defined: line %d of %s.\n" ),

_MSGERR( TTL_TOOLATE,   "The title at line %d of %s must be declared at the start of its topic.\n" ),
_MSGERR( MAC_TOOLATE,   "The topic macro at line %d of %s must be declared at the start of its topic.\n" ),

_MSGERR( BAD_RTF,   "The file %s is not a proper RTF file.\n" ),
_MSGERR( PROBLEM_RTF,   "The file %s contained errors.\n" ),
_MSGERR( RTF_BADCOMMAND, "Bad RTF command at line %d of %s.\n" ),
_MSGERR( RTF_UNKNOWN,    "Unknown command \\%s at line %d of %s.\n" ),
_MSGERR( RTF_BADEOF,    "Unexpected EOF in file %s.\n" ),

_MSGERR( RTF_HEADER,    "The file %s does not have a correct RTF header.\n" ),
_MSGERR( RTF_RTF,   "The file %s does not begin with the \\rtf command.\n" ),
_MSGERR( RTF_CHARSET,   "The file %s does not specify a known character set.\n" ),

_MSGERR( RTF_NOFONT,    "The RTF file does not have a font table.\n" ),
_MSGERR( RTF_LATEFONT,  "The font table must be specified at the beginning of an RTF file.\n" ),
_MSGERR( RTF_NOTEXT,    "The RTF file did not contain any text!\n" ),
_MSGERR( RTF_LATEKEEPN, "The \\keepn command at line %d of %s does not precede all of the text on the page.\n" ),
_MSGERR( RTF_EXTRATEXT, "Text was found beyond the end of the \\rtf block in file %s.\n" ),
_MSGERR( RTF_NOSUCHIMAGE,"Can't find image file %s (at line %d of %s).\n" ),
_MSGERR( RTF_USEDBADIMAGE, "The image file %s (at line %d of %s) is not in a recognized format.\n" ),

_MSGERR( RTF_NOARG, "The \\%s command at line %d of %s requires an argument.\n" ),

_MSGERR( TOP_TOOLARGE,  "Fatal error:  a paragraph exceeded 4K in size. Break your paragraphs into smaller units.\n" ),
_MSGERR( TOP_BADARG,    "Ignoring out-of-range spacing attribute %d (at line %d of %s).\n" ),
_MSGERR( TOP_BADTAB,    "Tab stop value %d (at line %d of %s) is out of range and will be ignored.\n" ),
_MSGERR( TOP_TWOBROWSE, "Two \'browse identifiers\' (%s and %s) were given for one topic.\n" ),
_MSGERR( TOP_NOWIN, "The window type %s is not defined in the project file (line %d of %s).\n" ),
_MSGERR( CON_BAD,   "Bad context string (%s) at line %d of %s.\n" ),
_MSGERR( CON_MISSING,   "A context string was expected at line %d of %s.\n" ),
_MSGERR( CON_BADCHAR,   "A context string at line %d of %s contains an unallowed character.\n" ),
_MSGERR( CON_NONUM, "Context string %s (at line %d of %s) has no context number.\n" ),

// The last entry is a terminator, please leave it in.
_MSGERR( NO_ERROR, "" )
