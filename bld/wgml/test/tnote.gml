.* test for &gml.note and &gml.fn tags footnote not yet supported
.se fil=&'left(&sysfnam,&'lastpos('.',&sysfnam)-1)
.if &'left('&sysversion',1) = 'O' .se lext=lay
.el .se lext=la4
.*
:layout
:cmt.   note_string = "note:"
:NOTE
        font = 2 left_indent = 0 note_string = '' pre_skip = 1 post_skip = 1
        right_indent = 0 spacing = 1
:convert file="&fil..&lext.".
:elayout
:gdoc sec='top secret'
:FRONTM
:PREFACE
:cmt BODY
.ju off
:p.
Some text with .co off.
.co off
:note.
Text on line following note tag.

:hp1.HP1 text for note in input line one.:ehp1.
More text for note in another line two
More text for note in another line three
More text for note in another line four
More text for note in another line five
More text for note in another line six
:note.Text directly on note tag.

:hp1.HP1 text for note in input line one.:ehp1.
More text for note in another line two
More text for note in another line three
More text for note in another line four
More text for note in another line five
More text for note in another line six
.co on
:p.
Some text with .co on.
:note.
Text on line following note tag.

:hp1.HP1 text for note in input line one.:ehp1.
More text for note in another line two
More text for note in another line three
More text for note in another line four
More text for note in another line five
More text for note in another line six
:note.Text directly on note tag.

:hp1.HP1 text for note in input line one.:ehp1.
More text for note in another line two
More text for note in another line three
More text for note in another line four
More text for note in another line five
More text for note in another line six
:p
Paragraph
:fn.fussnote:efn.
following note.
:egdoc
