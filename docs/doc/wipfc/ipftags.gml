.*
.chap *refid='ipftags' Using IPF Tags
.*
.np
Source files for the &ipfname consist of text (words, whitespace, punctuation, and entity references), 
markup tags, and commands. Each markup tags is described in detail elsewhere (:HDREF refid='ipftref'.), 
as are the commands (:HDREF refid='ipfcmds'.).
.np
Tags come in four flavors:
.autopoint
.point
Tags that appear in the document header and control how the document behaves.
.point
Tags that establish blocks. These tags can contain other tags as well as text.
.point
Tags that are in-line and contain text or other in-line tags.
.point
Formatting tags that can appear anywhere, but are usually in-line.
.endpoint
.*
.section Text
.ix 'text'
.*
.np
Text consists of words, whitespace, punctuation, and entity references (symbols).
.*
.beglevel
.*
.section Words
.ix 'words'
.ix 'Text' 'words'
.np
Words consist of letters and/or numbers (ABC123 is a single word, for example). Which letters are valid 
depends upon the current locale. For the default locale (en_US, code page 850), the upper and lower 
case ASCII letters are allowed (A-Z, a-z). Other letters can be included via entity references.
.*
.section Whitespace
.ix 'whitespace'
.ix 'Text' 'whitespace'
.*
.np
Whitespace consists of spaces, tabs, and new-line characters. Usually, single whitespace characters
are not significant, but multiple whitespace characters are. Tab characters are not expanded, so count 
as a single whitespace character. New-lines are generally ignored.
.np
Some tags create blocks where some or all forms of whitespace become significant. In these cases, new-line 
characters will cause line breaks and the text will be shown as you have formatted it. If the block 
is monospaced, spaces as well as new-lines will be significant.
.*
.section Punctuation
.ix 'punctuation'
.ix 'Text' 'punctuation'
.np
Punctuation is not part of a word. 'ABC123' is a single word, but 'ABC-123' is actually two words 
('ABC' and '123') and a punctuation ('-').
.np
Some forms of punctuation are used to delimit tags or entity references. When a colon (':') or ampersand
('&') begin a word, that word is treated as a tag or entity reference, respectively. Likewise, a 
period ('.') ends both tags and entity references. If you need to use such constructs, replace the 
punctuation character with an entity reference ('&amp.colon.', '&amp.amp.', and '&amp.per.').
.*
.section Entity References (Symbols)
.ix 'symbols'
.ix 'Entity references'
.ix 'Text' 'symbols'
.ix 'Text' 'entity references'
.*
.np
Entities are symbolic references to single characters. During processing of the source file, valid 
entity references are removed and the matching single character is substituted. This allows you to
include special characters that are outside the normal character set (for example, characters with 
diacriticals) in words.
.np
Unless the entity reference is a punctuation symbol, a string of ...-word-entity-word-... (in any 
order) is considered to be a single word.
.np
Unrecognized entity references will cause &ipfcmd to generate a warning. Note that all entity references
are case sensitive: &amp.Alpha. and &amp.alpha. are two different things.
.*
.endlevel
.*
.section Tag Syntax
.ix 'Tags' 'syntax'
.*
.np
All tags begin with a colon (':') and end with a period ('.'). In addition, many tags have
flags or attribute=value pairs that appear inside the tag between the tag name and the end-of-tag
delimiter ('.'). If you forget the end-of-tag delimiter, the numerous warnings that &ipfcmd generates 
will be sure to let you know.
.*
.section Tag Attributes
.ix 'Tags' 'attributes'
.*
.np
A tag may have one or more
.keyword attributes.
An attribute contains additional information that a tag needs. The attribute has a name, and may also 
have a value or keyword assigned to it. 
.np
For example, heading tags have a 'res' attribute that specifies a window identifier (a target for hyper-linked
jumps). 
.code begin
&colon.h1 res=001.How to make Popcorn
.code end
.np
In this case, res is assigned the value 001.
.np
Notice that attributes are part of the tag, and the end-of-tag delimiter always follows the last attribute.
.np
Many tags have multiple attributes. If you like, you can place them on different lines or all on a single line.
If you place them on different lines, however, the attribute and its assigned value must be on the same line.
Spaces are allowed between the attribute, '=', and the value. If spaces are required within the value, the 
value must be enclosed in matching quotation marks ('', or "", but not '" or "').
.np
Optional attributes all have default values and do not need to be specified unless you need to change the default. 
Other attributes are required, and &ipfcmd will issue a warning if they are not specified.
.np
Attributes may be specified in any order within a tag. 
.np
Some attributes may not have a value, in which case the presence of the attribute simply acts as a boolean flag. 
For those attributes that do have values, values can be numeric, text strings (with spaces, if quoted), or 
keywords.

For example, the color tag specifies the foreground and/or background color for text. Only certain colors 
are supported, and each has a keyword:
:SL.
:LI.default
:LI.blue
:LI.red
:LI.pink
:LI.green
:LI.cyan
:LI.yellow
:LI.neutral
:LI.brown
:LI.darkgray
:LI.darkblue
:LI.darkred
:LI.darkpink
:LI.darkgreen
:LI.darkcyan
:LI.black
:LI.palegray
:ESL.
.*
.section End Tags
.ix 'Tags' 'ending'
.*
.np
Some tags require matching end tags to form an enclosing block of code. The end tag has the same 
name as the tag prefixed with the letter 'e'. For example, if "tag" had an end tag, it 
would be "etag".
.*
.section Nesting Tags
.ix 'Tags' 'nesting'
.*
.np
Most tags can be arbitrarily nested within other tags. However, some tags can 
.em only
be nested inside other tags. For example, the list-item tag (li) can only appear between 
opening and closing list tags.
.np
In addition, there are a few tags that restrict the kinds of elements that they can contain. If you 
try to include a tag where it doesn't belong, &ipfcmd will issue a warning, the tag will be ignored, 
and the result will not be what you hoped for!
.*
.section Tags Requiring Text
.ix 'Tags' 'and text'
.*
.np
Some tags (for example, heading tags) have text associated with them. This text can appear either 
immediately after the end-of-tag delimiter ('.') or on the next line. Note, however, that in this 
case the end-of-tag delimiter must be immediately followed by a new-line character. No other form 
of whitespace is allowed.
.*
.section *refid=units Units of Measure
.np
Several tags have attributes that are measurements of one sort or another (window origin and extent, 
for example). Dimensional measurements can be one of three types:
.keyword absolute, relative,
or 
.keyword dynamic.
.np
.keyword Absolute
measurements consist of a number followed by a letter that indicates the unit of measurement.
The units are:
.begbull $compact
.bull
c: the average character width of the default system font.
.bull
x: pixels.
.bull
p: typographical points (about 1/72 inch).
.endbull
.np
.keyword Relative
measurements consists of a number followed by the percent sign (%). The values is a percentage of the 
parent window width or height.
.np 
.keyword Dynamic 
measurements consist of a keyword. The actual value is computed at run-time based on the current size 
and position of the parent window. The keywords for x-axis values are:
.begbull $compact
.bull
left: flush left in the parent window
.bull
center: centered in the parent window
.bull
right: flush right in the parent window
.endbull
.np
The keywords for y-axis values are:
.begbull $compact
.bull
top: at the top of the parent window
.bull
center: centered in the parent window
.bull
bottom: at the bottom of the parent window
.endbull 
.np
Note that the coordinate system for all window measurements has its origin at the lower left corner. 
Increasingly positive values represent movement upward and rightward.

