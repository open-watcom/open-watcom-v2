.* ------- 32-bit Windows NT
.sr bldsys = 'Windows NT GUI'
.sr bldos  = 'Windows NT'
.if '&lang' eq 'FORTRAN 77' .do begin
.sr bldswt = '&sw.bw'
.do end
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.sr bldswt = '&sw.bw &sw.bt=windows'
.do end
.sr bldnam = 'nt_win'
.sr bldmsg = 'a Windows NT windowed executable'
.sr bldexe = '.exe'
.sr wclcmd = &wclcmd32.
.sr wclcmdup = &wclcmdup32.
.im pgblddw
