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
* Description:  Error messages
*
****************************************************************************/

//Fatal Errors
PICK( ERR_PATH_MAX, "The length of the file path is too long" )
PICK( ERR_OPEN, "Cannot open file" )
PICK( ERR_OPENIMG, "Cannot open image file" )
PICK( ERR_READ, "Cannot read from file" )
PICK( ERR_WRITE, "Cannot write to file" )
PICK( ERR_EOF, "Unexpected end of file" )
PICK( ERR_T_CONV, "Text conversion error" )
PICK( ERR_SYNTAX, "Syntax error" )
PICK( ERR_COUNTRY, "Invalid country code or codepage" )
PICK( ERR_LANG, "Invalid language code" )
PICK( ERR_DOCBODY, "Missing :userdoc or :euserdoc" )
PICK( ERR_DOCLARGE, "Too many unique words--the document is too big" )
PICK( ERR_DOCSMALL, "The document has no words" )
PICK( ERR_LARGETOC, "The document has too many pages" )
PICK( ERR_LARGEINDEX, "The document has too many index or icmd entries" )
PICK( ERR_INVISIBLETOC, "The document has no visible table of contents entries" )
PICK( ERR_BADTOKEN, "Internal error--unrecognized token" )
PICK( ERR_LAST, "Last fatal error code" )
/*
Fatal errors
001 Not enough memory               // Explanation: SYSTEM ERROR. Close some applications to free some memory.
002 Cannot free memory              // Explanation: SYSTEM ERROR. System could not free memory.
003 Cannot open file                // Explanation: SYSTEM ERROR. Filename or path is incorrect, file doesn't exist, or other system problem.
004 Cannot read file                // Explanation: SYSTEM ERROR. Source file may be corrupted.
005 Cannot write to a file          // Explanation: SYSTEM ERROR. File system is full, out of disk space, diskette is write protected, etc.
006 Cannot reopen. File is already opened   // Explanation: SYSTEM ERROR. 
007 Unexpected end of file          // Explanation: This may be caused by an ending tag not being found, a corrupted or truncated source file, or a control-Z character found before the true end of file.
008 Invalid country code, or codepage
009 Invalid language code
010 Invalid document body           // Explanation: No :userdoc./:euserdoc. match.
011 Cannot create panel(s)
012 Document is too big - unique words exceed 16 000
013 Document has no vocabulary
014 Document has no visible table of contents entry
015 Text conversion error - text too long
*/

//Class 1 Errors
PICK( ERR1_CMDNOTDEF, "This command is not defined" )
PICK( ERR1_TAGNOTDEF, "This tag is not defined" )
PICK( ERR1_ATTRNOTDEF, "This attribute is not defined for this tag" )
PICK( ERR1_NOATTR, "Attributes are not allowed for this tag--probably missing '.' to end the tag" )
PICK( ERR1_TAGCONTEXT, "The tag, word, or command is not allowed in this context" )
PICK( ERR1_TAGSYNTAX, "Syntax error in tag" )
PICK( ERR1_HEADTEXT, "Ignoring text before the :h1. tag" )
PICK( ERR1_TAILTEXT, "Ignoring text after the :euserdoc. tag" )
PICK( ERR1_HEADNEST, "Heading levels are not in consecutive order" )
PICK( ERR1_NOREFID, "The required refid attribute is missing" )
PICK( ERR1_NORES, "Unable to find the resource identifier for this reference" )
PICK( ERR1_NODDFRES, "The required resource identifier is missing" )
PICK( ERR1_NOID, "Unable to find the id or name for this reference" )
PICK( ERR1_NOFNID, "The required id for this footnote is missing" )
PICK( ERR1_NOLIST, "Ignoring an :li. that is not part of a list" )
PICK( ERR1_NOENDLIST, "The required end the list is missing" )
PICK( ERR1_DLHEADMATCH, "A 'ddhd' tag must be preceded by a 'dthd' tag" )
PICK( ERR1_DLDTDDMATCH, "A 'dd' tag must be preceded by a 'dt' tag" )
PICK( ERR1_EXTFILESLARGE, "Too many external files have been linked to" )
PICK( ERR1_NOFILENAME, "The required graphics file name is not present" )
PICK( ERR1_BADFMT, "wipfc does not support this graphics file format" )
PICK( ERR1_HIDERES, "Cannot hide a header with the 'res' attribute set" )
PICK( ERR1_NOCOLS, "No valid 'cols' have been specified" )
PICK( ERR1_TABLEWIDTH, "The total width of the table is >250 characters" )
PICK( ERR1_TABLETEXT, "Ignoring text before :c. tag" )
PICK( ERR1_TABLECELLTAG, "Ignoring invalid tag in table cell" )
PICK( ERR1_TABLECELLTEXTWIDTH, "Text in this table column is too long, truncating" )
PICK( ERR1_TABLECELLCOUNTHIGH, "Ignoring extra table columns in this row" )
PICK( ERR1_TABLECELLCOUNTLOW, "Not enough columns have been specified for this row" )
PICK( ERR1_TABLEELINK, "The required :elink. tag is missing, but has been appended to this column" )
PICK( ERR1_LARGEPAGE, "This page has too many elements (words, punctuation, etc.)" )
PICK( ERR1_LAST, "Last level 1 error code" )
/*
Class1 Errors
101 Tag not defined
102 Attribute not defined
103 Duplicate tag in tag file
104 Invalid tag syntax
105 Illegal context for tag         // Explanation: Tags are not properly matched, a tag is used incorrectly, or a tag is placed incorrectly. 
106 List start tag missing - tag ignored
107 List end tag not matched - tag ignored
108 Ignoring unmatched tag
109 No id for this reference
110 No id for this footnote
111 No res for this reference
112 No text found in tag
113 Duplicate text in tag
114 Definition term or header not matched
115 A DT tag is not defined
116 A PT tag is not defined
117 Missing hypertext information
118 Cannot hide parent head level   // Explanation: Preceding head level must be hidden. 
119 Page is too big                 // Explanation: Panel is too big. Maximum size is 16 000 words and punctuation marks. (Note maximum size is language dependent.) 
120 Duplicate root word             // In isyn
121 Ignoring text before :h1. tag
122 Ignoring text before :c. tag
123 Duplicate tag in source file
124 Invalid head level              // Explanation: Head levels are not in consecutive order.
125 Invalid tag in footnote
126 Invalid bitmap format           // Explanation: File is not a valid PM format bitmap file. 
127 No valid COLS specification was given
128 Ignoring invalid tag in table cell
129 Extra cells will be placed in next table row
130 Missing ELINK tag inserted at end of table cell
131 Total table width exceeds limit of 250 characters
132 Truncating table entry          // This is a warning to tell the user when the text for a table cell is too long for the size of the cell. 
*/

//Class 2 Errors
PICK( ERR2_VALUE, "Invalid or missing attribute value" )
PICK( ERR2_SYMBOL, "Invalid symbol (entity reference or .nameit expansion)" )
PICK( ERR2_NEST, "Invalid tag nesting" )
PICK( ERR2_FNIDX, "Footnote cannot be indexed" )
PICK( ERR2_FONTS, "Too many (> 14) fonts have been used" )
PICK( ERR2_TEXTTOOLONG, "Title or index text is too long" )
PICK( ERR2_SUBNOTGLOBAL, "Subindexes of a global index must be global, too" )
PICK( ERR2_INOTEXT, "An index entry requires text" )
PICK( ERR2_LAST, "Last level 2 error code" )
/*
Class2 Errors
201 Invalid tag
202 Invalid attribute
203 Invalid symbol                  // Explanation: Invalid APS symbol; period missing after the APS symbol, symbol specified is not in the APSYMBOL.APS file, invalid APSYMBOL.APS file.
204 Invalid macro
205 Text too long in tag            // Explanation: Heading and index tags have a maximum of 150 characters. 
206 Token is bigger than expected.  // Explanation: Maximum length of token is 255 characters. This error could be caused by a missing end period or quote character. 
207 Invalid attribute value
208 Missing tag
209 Attribute not matched
210 Text too long in macro expansion // Explanation: Maximum 255 characters. 
211 Total number of fonts exceeds the limit of 14
212 Sub index cannot be global without global main index
213 Invalid nest
*/

//Class 3 Errors
PICK( ERR3_DUPID, "This id or name is already in use" )
PICK( ERR3_DUPRES, "This res number is already in use" )
PICK( ERR3_DUPSYMBOL, "Redefinition of .nameit symbol" )
PICK( ERR3_DUPSYN, "This synonym set is already defined" )
PICK( ERR3_NOSYN, "This synonym set is not defined" )
PICK( ERR3_NOBUTTON, "Control group references an undefined button" )
PICK( ERR3_MIXEDUNITS, "Cannot mix dynamic and absolute units" )
PICK( ERR3_FNNOAUTO, "Footnotes cannot be opened automatically" )
PICK( ERR3_FNNOSPLIT, "Footnote cannot be opened from a split window" )
PICK( ERR3_LAST, "Last level 3 error code" )
/*
Class3 Errors
301 Ignoring attribute
302 Duplicate ID                    // Explanation: Cannot specify the same ID in the same panel or index. 
303 Duplicate symbol in symbol file
304 Duplicate res number
305 Parent panel cannot have its own text
306 Missing panel text in head level tag
307 Missing footnote text in :fn. tag

*/

