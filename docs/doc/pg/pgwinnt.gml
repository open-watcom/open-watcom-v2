.if '&lang' eq 'FORTRAN 77' .do begin
.* no GUI support in FORTRAN 77
.do end
.el .do begin
.* ------- 32-bit Windows NT
.sr bldsys = 'Windows NT GUI'
.sr bldos  = 'Windows NT'
.if '&lang' eq 'FORTRAN 77' .do begin
.sr bldswt = ''
.do end
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.sr bldswt = '&sw.bt=nt '
.do end
.sr bldnam = 'nt_win'
.sr bldmsg = 'a Windows NT windowed executable'
.sr bldexe = '.exe'
.sr wclcmd = &wclcmd32.
.sr wclcmdup = &wclcmdup32.
.im pgbldwin
.do end
