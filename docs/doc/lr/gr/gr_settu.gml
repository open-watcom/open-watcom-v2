.gfunc settextcursor
.if '&lang' eq 'C' .do begin
short _FAR _settextcursor( short cursor );
.do end
.el .do begin
integer*2 function _settextcursor( cursor )
integer*2 cursor
.do end
.gfuncend
.desc begin
The &func &routine sets the attribute, or shape, of the cursor in text modes.
:CMT.The cursor shape is set using the BIOS Set Cursor Size function.
The argument
.arg cursor
specifies the new cursor shape.
The cursor shape is selected by specifying the top and bottom rows
in the character matrix.
The high byte of
.arg cursor
specifies the top row of the cursor;
the low byte specifies the bottom row.
.np
Some typical values for
.arg cursor
are:
.millust begin
Cursor          Shape

.if '&lang' eq 'C' .do begin
0x0607          normal underline cursor
0x0007          full block cursor
0x0407          half-height block cursor
0x2000          no cursor
.do end
.el .do begin
'0607'x         normal underline cursor
'0007'x         full block cursor
'0407'x         half-height block cursor
'2000'x         no cursor
.do end
.millust end
.desc end
.return begin
The &func &routine returns the previous cursor shape when the shape
is set successfully; otherwise, (-1) is returned.
.return end
.see begin
.seelist &function. _gettextcursor _displaycursor
.see end
.grexam begin eg_gettu.&langsuff
.grexam end
.class &grfun
.system
