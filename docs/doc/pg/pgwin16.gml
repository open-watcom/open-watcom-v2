.* ------- 16-bit Windows
.sr sw = '-'
.sr bldsys = '16-bit Windows 3.x'
.sr bldos  = 'Windows 3.x'
.if '&lang' eq 'FORTRAN 77' .do begin
.sr bldswt = '&sw.win'
.do end
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.sr bldswt = '&sw.bt=windows'
.do end
.sr bldnam = 'windows'
.sr bldmsg = 'a Windows 16-bit executable'
.sr bldexe = '.exe'
.sr wclcmd = &wclcmd16.
.sr wclcmdup = &wclcmdup16.
.im pgbldwin
