.* ------- 32-bit Windows
.sr sw = '-'
.sr bldsys = '32-bit Windows 3.x'
.sr bldos  = 'Windows 3.x'
.if '&lang' eq 'FORTRAN 77' .do begin
.sr bldswt = '&sw.bw'
.do end
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.sr bldswt = '&sw.bw &sw.bt=windows'
.do end
.sr bldnam = 'win386'
.sr bldmsg = 'a Windows 32-bit executable'
.sr bldexe = '.rex'
.sr wclcmd = &wclcmd32.
.sr wclcmdup = &wclcmdup32.
.im pgblddw
