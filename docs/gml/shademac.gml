.dm shade begin
.   .if '&*' eq 'begin' .do begin
.   .	.br
.   .	:SF font=13.~n:eSF.
.   .	:LQ.
.   .do end
.   .el .if '&*' eq 'end' .do begin
.   .	:SF font=13.~n:eSF.
.   .	:eLQ.
.   .do end
.dm shade end
.*
.dm xtshade begin
:SF font=13.~n:eSF.
.dm xtshade end
.*
.dm xtfig begin
:SF font=14.~n:eSF.
.dm xtfig end
.*
.dm xtslim begin
:SF font=15.~n:eSF.
.dm xtslim end
