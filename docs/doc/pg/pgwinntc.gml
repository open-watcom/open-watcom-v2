.* ------- 32-bit Windows NT Character-mode
.sr bldsys = 'Windows NT Character-mode'
.sr bldos  = 'Windows NT'
.if '&lang' eq 'FORTRAN 77' .do begin
.sr bldswt = ''
.do end
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.sr bldswt = '&sw.bt=nt'
.do end
.sr bldnam = 'nt'
.sr bldmsg = 'a Windows NT Character-mode executable'
.sr bldexe = '.exe'
.sr wclcmd = &wclcmd32.
.sr wclcmdup = &wclcmdup32.
.im pgbldwin
