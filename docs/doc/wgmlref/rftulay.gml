:P.
Information about the document style is not specified when a
GML document file is created.
Style information such as indentation on the first line of a paragraph
does not change the paragraph into a different
type of document entity.
Examples of information considered part of the document style are:
:I1.document style
:I1.style
:ol.
:li.The maximum number of lines on a page.
:li.The number of spaces to indent the first line in a paragraph.
:li.The maximum number of characters on a line, including the space
    characters.
:li.The number of lines to leave between paragraphs.
:li.Justify text by adding space between words.
:eol.
:P.
With &WGML., the document style
is specified in the
:I1.layout
:HP2.layout:eHP2.
section.
A document is produced by associating a layout with the document source
when it is processed by &WGML..
By specifying different layouts,
the document style can be changed without modifying the document text.
:NOTE.The layout determines how many columns of text are on an output
page.
Widows and most document elements are placed on the next available
column when the current column is full.
For the purposes of this book, a one column layout is assumed.
