:set symbol="machsys" value="DOS".
:set symbol="isbn"    value="1-55094-036-8"
:cmt. :INCLUDE file='BANNER'.
:INCLUDE file='FMTMACRO'.
:INCLUDE file='SYMBOLS'.
:INCLUDE file='CMANMAC'.
:INCLUDE file='LIBFUNS'.
:INCLUDE file='LIBLIST'.
:GDOC.
.*
:FRONTM.
:BODY.
.sepsect WATCOM C Library Reference
.if '&machsys' ne 'TEST' .do begin
:INCLUDE file='CLIBOVER'.
.do end
.if '&machsys' eq 'DOS' .do begin
:INCLUDE file='CLIBDOS'.
.do end
:INCLUDE file='CLIBFUNS'.
:set symbol="headtxt0$" value=''.
:set symbol="headtxt1$" value=''.
:cmt. :INCLUDE file='CREFUNS'.
:cmt. .if '&machsys' eq 'DOS' .do begin
:cmt. :INCLUDE file='CSUMFUNS'.
:cmt. .do end
:cmt. .sepsect Appendices
:cmt. :APPENDIX.
:cmt. :INCLUDE file='CLIBFUNC'.
:BACKM.
:cmt. :INDEX.
:eGDOC.
