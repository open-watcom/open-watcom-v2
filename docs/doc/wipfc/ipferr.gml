.*
.chap *refid='ipferr' Error Messages
.*
.np
The &ipfname generates four kinds of diagnostic messages: Errors and three levels of warnings. Despite
their name, warnings are really errors and indicate that &ipfcmd has produced a file that is probably
not exactly the way you wanted.
.*
.section Errors
.*
.np
Errors are always fatal and cause &ipfname to stop immediately.
.begnote $break
.note "The length of the file path is too long"
The maximum length of the file path depends on the operating system. You may need to re-arrange the
project's layout on disk.
.note "Cannot open file"
A source file cannot be opened, usually because it doesn't exist. Check the spelling of the file name.
.note "Cannot open image file"
A image file cannot be opened, usually because it doesn't exist. Check the spelling of the file name.
.note "Cannot read from file"
For some reason, reading the file has failed.
.note "Cannot write to file"
For some reason, writing to the file has failed. The disk might be full.
.note "Unexpected end of file"
The source file has unexpectedly ended. This can occur if you have forgotten the euserdoc tag.
.note "Text conversion error"
&ipfcmd uses wide characters internally. All text is converted from multi-byte characters to wide 
characters when read, and back to multi-byte characters when written. If a character cannot be converted
successfully, this error is generated.
.note "Syntax error"
A syntax error has occurred. Usually, this is because an end-of-tag delimiter is missing.
.note "Invalid country code or codepage"
The appropriate file of entity references cannot be found or cannot be read. Usually, this is because the 
WIPFC environment variable is pointing to the wrong place.
.note "Invalid language code"
The appropriate nls file cannot be found or cannot be read. Usually, this is because the WIPFC environment
variable is pointing to the wrong place.
.note "Missing userdoc or euserdoc"
The document is not correctly formed. The userdoc and euserdoc tags are required.
.note "Too many unique words--the document is too big"
A document can contain a maximum of 64000 unique words.
.note "The document has no words"
A document cannot be empty. Add some text.
.note "The document has too many pages"
A document cannot have more than 65535 pages.
.note "The document has too many index or icmd entries"
The document cannot have more than 65535 index entries.
.note "The document has no visible table of contents entries"
At least one of the pages must be visible. Don't hide them all.
.endnote
.*
.section Level 1 Warnings
.*
.np
Level 1 warnings are the most serious. Unless they are corrected, your help file is not likely to contain 
information presented as you intended.
.begnote $break
.note "This command is not defined"
Commands begin with a '.' in the first column of a row of text. You probably have a line beginning with a period.
.note "This tag is not defined"
You are trying to use a tag that &ipfcmd does not recognize. Try replacing the ':' character with an entity reference.
.note "This attribute is not defined for this tag"
Check the spelling of the attribute, or delete it.
.note "Attributes are not allowed for this tag--probably missing . to end the tag"
This tag has no attributes.
.note "The tag, word, or command is not allowed in this context"
This is usually because the enclosing tag can only contain specific tags or no tags at all.
.note "Syntax error in tag"
There is something wrong within the tag.
.note "Ignoring text before the h1 tag"
Text and most tags cannot appear before the first heading tag.
.note "Ignoring text after the euserdoc tag"
Text and tags cannot appear after the terminating document tag.
.note "Heading levels are not in consecutive order"
The offending heading tag is more than one level greater than the heading that contains it.
.note "The required refid attribute is missing"
A refid or res attribute is required to specify the target of a link or secondary index, but not present.
.note "Unable to find the resource identifier for this reference"
A non-existent resource id is being referenced.
.note "The required resource identifier is missing"
A ddf tag is missing a valid resource identifier.
.note "Unable to find the id or name for this reference"
The referenced id or name does not exist.
.note "The required id for this footnote is missing"
A footnote tag is missing a required id.
.note "Ignoring a list item tag that is not part of a list"
List elements can only appear within a list.
.note "The required end the list is missing"
The list has not been closed.
.note "A ddhd tag must be preceded by a dthd tag"
Definition list header tags must appear in the correct order.
.note "A dd tag must be preceded by a dt tag"
Definition list elements must appear in the correct order.
.note "Too many external files have been linked to"
A maximum of 256 external files can be linked.
.note "The required graphics file name is not present"
You have to specify the name of the bit-mapped graphics file if you expect &ipfcmd to find it.
.note "wipfc does not support this graphics file format"
Only Windows and OS/2 bmp files are supported by &ipfcmd..
.note "Cannot hide a header with the res attribute set"
Either don't hide the header, or remove the res attribute so it can't be a link target.
.note "No valid cols have been specified"
A table needs its column widths specified in advance.
.note "The total width of the table is >250 characters"
The total width of a table (including borders) cannot exceed 250 characters.
.note "Ignoring text before c tag"
Text in a table must be inside of a cell. Put it after the c tag.
.note "Ignoring invalid tag in table cell"
Table cells cannot contain other block-level tags.
.note "Text in this table column is too long, truncating"
If a single word is longer than the table cell is wide, it is truncated.
.note "Ignoring extra table columns in this row"
You have more c tags in the row than you have columns specified in the table tag.
.note "Not enough columns have been specified for this row"
You have fewer c tags in the row than you have columns specified in the table tag.
.note "The required elink tag is missing, but has been appended to this column"
You forgot to close the link, so &ipfcmd has added an elink tag at the end of the cell.
.note "This page has too many elements (words, punctuation, etc.)"
The maximum number of text elements in a single page is 64000.
.endnote
.*
.section Level 2 Warnings
.*
.np
Level 2 warnings are less serious.
.begnote $break
.note "Invalid or missing attribute value"
A required attribute is either missing, or has an invalid value.
.note "Invalid symbol (entity reference or .nameit expansion)"
The entity reference or nameit macro is not defined. Check the spelling.
.note "Invalid tag nesting"
Text hiding tags and highlighting tags cannot nest with themselves.
.note "Footnote cannot be indexed"
Footnotes cannot have in1, in2, or icmd tags.
.note "Too many (> 14) fonts have been used"
You need to user fewer fonts.
.note "Title or index text is too long"
The length of title text is limited to 47 characters; index text, to 255 characters. 
.note "Subindexes of a global index must be global, too"
If an index is global, all of its subindexes must be global, too. Set the 'global' flag. 
.note "An index entry requires text"
Text must follow an in1, in2, or icmd tag.
.endnote
.*
.section Level 3 Warnings
.*
.np
Level 3 warnings are less serious still.
.begnote $break
.note "This id or name is already in use"
If the id or name is the target of a link tag, you're not likely to get to the right place.
.note "This res number is already in use"
If the resource id is the target of a link tag, you're not likely to get to the right place.
.note "Redefinition of .nameit symbol"
A nameit symbol can only be defined once. It cannot be redefined.
.note "This synonym set is already defined"
A synonym set can only be defined once.
.note "This synonym set is not defined"
A synonym set must be defined before it can be used.
.note "Control group references an undefined button"
A push button definition is missing.
.note "Cannot mix dynamic and absolute units"
Absolute and relative units cannot be used at the same time.
.note "Footnotes cannot be opened automatically"
Footnotes are special.
.note "Footnote cannot be opened from a split window"
Footnotes are special.
.endnote

