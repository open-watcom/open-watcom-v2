.dm hint begin
.   .if '&*' eq 'begin' .do begin
.   .   :P.
.   .   .cp &WDWlvl
.   .   .se lmargin=&sysin+1
.   .   .bx on &lmargin &rmargin
.   .   .se lmargin=&sysin+2
.   .   .in +2
.   .   :SF font=3.Hint::eSF.
.  .do end
.  .el .if '&*' eq 'end' .do begin
.   .   .in -2
.   .   .bx off
.  .do end
.  .el .ty *** hint macro error ***
.dm hint end
.*
.dm remark begin
.  .if '&*' eq 'begin' .do begin
.   .   :P.
.   .   .cp &WDWlvl
.   .   .se lmargin=&sysin+1
.   .   .bx on &lmargin &rmargin
.   .   .se lmargin=&sysin+2
.   .   .in +2
.   .   :SF font=3.Note::eSF.
.  .do end
.  .el .if '&*' eq 'end' .do begin
.   .   .in -2
.   .   .bx off
.  .do end
.  .el .ty *** remark macro error ***
.dm remark end
.*
.dm warn begin
.  .if '&*' eq 'begin' .do begin
.   .   :P.
.   .   .cp &WDWlvl
.   .   .se lmargin=&sysin+1
.   .   .bx on &lmargin &rmargin
.   .   .se lmargin=&sysin+2
.   .   .in +2
.   .   :SF font=3.WARNING!:eSF.
.  .do end
.  .el .if '&*' eq 'end' .do begin
.   .   .in -2
.   .   .bx off
.  .do end
.  .el .ty *** warn macro error ***
.dm warn end
.*
