..dm boxdef begin
.  .keep begin
.  ..sr rdist  = &SYSIN.+1
.  ..sr boxcmd = &rdist.
.  ..sr tabcmd = ''
..dm boxdef end
.*
..dm boxcol begin
.  ..sr *tab   = ''
.  ..sr *width = &*1
.  ..if &*0 = 1 ..th ..do begin
.  .  ..if &rdist. ne 0 ..th ..do begin
.  .  .  ..sr *tab = &rdist.+1
.  .  ..do end
.  .  ..sr rdist = &*width.-1
.  ..do end
.  ..el ..if &*0 = 2 ..th ..do begin
.  .  ..if '&*2' = 'c' ..th ..do begin
.  .  .  ..sr *centre = (&*width.+1)/2
.  .  .  ..sr *tab    = &rdist.+&*centre.
.  .  .  ..sr *tab    = '&*tab.c'
.  .  .  ..sr rdist   = &*width.-&*centre.
.  .  ..do end
.  .  ..el ..do begin
.  .  .  ..ty Boxcol 2nd parameter error in "&*"
.  .  ..do end
.  ..do end
.  ..el ..do begin
.  .  ..ty Boxcol parameter error in "&*"
.  ..do end
.  ..sr boxcmd = '&boxcmd. +&*width.'
.  ..if '&*tab.' ne '' ..th ..do begin
.  .  ..if '&tabcmd.' eq '' ..th ..do begin
.  .  .  ..sr tabcmd = '&*tab.'
.  .  ..do end
.  .  ..el ..do begin
.  .  .  ..sr tabcmd = '&tabcmd. +&*tab.'
.  .  ..do end
.  ..do end
..dm boxcol end
.*
..dm boxbeg begin
:CMT. .  ..br
:CMT. .  ..li Final settings
:CMT. .  ..br
:CMT. .  ..li ..tb &tabcmd
:CMT. .  ..br
:CMT. .  ..li ..bx &boxcmd
:CMT. .  ..br
.  ..tb     &tabcmd
.  ..tb set &tabchar
.  ..bx on  &boxcmd
..dm boxbeg end
.*
..dm boxline begin
.  ..bx
..dm boxline end
.*
..dm boxend begin
.  ..bx off
.  ..tb
.  ..tb set
.  ..sr boxcmd off
.  ..sr tabcmd off
.  ..sr rdist  off
.  .keep end
.dm boxend end
