.sk 2
.if '&format' eq '8.5x11' .do begin
:graphic depth='3.68i' xoff='-2.25i' yoff='0.0i' scale=50 file='ltning.eps'.
:DOCNUM.Version &book_ver.
:graphic depth='1.5i' xoff='-1.6i' yoff='0.0i' scale=120 file='owlogo.eps'.
.do end
.el .do begin
:graphic depth='3.68i' xoff='-1.9i' yoff='0.0i' scale=50 file='ltning.eps'.
:DOCNUM.Version &book_ver.
:graphic depth='1.5i' xoff='-1.25i' yoff='0.0i' scale=120 file='owlogo.eps'.
.do end
