.ix 'escape sequences'
.ix 'non-graphic characters' 'escape sequences'
..sk 1 c
.if &e'&nobox eq 0 .do begin
.boxdef
..if '&format' eq '7x9' ..th ..do begin
.  .boxcol  8 c
.  .boxcol 21
..do end
..el ..do begin
.  .boxcol  8 c
.  .boxcol 25
..do end
.boxbeg
$Escape$
$Sequence$ Meaning
.boxline
.monoon
$\a$:HP0.Causes an audible or visual alert:eHP0.
$\b$:HP0.Back up one character:eHP0.
$\f$:HP0.Move to the start of the next page:eHP0.
$\n$:HP0.Move to the start of the next line:eHP0.
$\r$:HP0.Move to the start of the current line:eHP0.
$\t$:HP0.Move to the next horizontal tab:eHP0.
$\v$:HP0.Move to the next vertical tab:eHP0.
.monooff
.boxend
.do end
.el .do begin
.millust begin
Escape
Sequence    Meaning
--------    -------------------------------------
\a          Causes an audible or visual alert
\b          Back up one character
\f          Move to the start of the next page
\n          Move to the start of the next line
\r          Move to the start of the current line
\t          Move to the next horizontal tab
\v          Move to the next vertical tab
.millust end
.do end
