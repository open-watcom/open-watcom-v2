.pp
The graphics library can display text in three different ways.
.autonote
.note
The
.kw _outtext
and
.kw _outmem
&routines can be used in any video mode.
However, this variety of text can be displayed in only one size.
.note
The
.kw _grtext
&routine displays text as a sequence of line segments, and can be drawn
in different sizes, with different orientations and alignments.
.note
The
.kw _outgtext
&routine displays text in the currently selected font.
Both bit-mapped and vector fonts are supported; the size and type of text
depends on the fonts that are available.
.endnote
