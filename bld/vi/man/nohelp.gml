.dm H_0 begin
.if '&*id.' eq '' .do begin
    :H0.&*
.do end
.el .do begin
    :H0 id='&*id.'.&*
.do end
.dm H_0 end

.dm H_1 begin
.if '&*id.' eq '' .do begin
    :H1.&*
.do end
.el .do begin
    :H1 id='&*id.'.&*
.do end
.dm H_1 end

.dm H_2 begin
.if '&*id.' eq '' .do begin
    :H2.&*
.do end
.el .do begin
    :H2 id='&*id.'.&*
.do end
.dm H_2 end

.dm H_3 begin
.if '&*id.' eq '' .do begin
    :H3.&*
.do end
.el .do begin
    :H3 id='&*id.'.&*
.do end
.dm H_3 end

.dm H_4 begin
.if '&*id.' eq '' .do begin
    :H4.&*
.do end
.el .do begin
    :H4 id='&*id.'.&*
.do end
.dm H_4 end

.dm H_5 begin
.if '&*id.' eq '' .do begin
    :H5.&*
.do end
.el .do begin
    :H5 id='&*id.'.&*
.do end
.dm H_5 end
