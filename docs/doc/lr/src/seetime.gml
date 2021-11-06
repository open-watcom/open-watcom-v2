.* TIME FAMILY  with Safer C
.seelist asctime Functions asctime_s clock ctime Functions ctime_s
.seelist difftime gmtime gmtime_s
.if '&machsys' eq 'NEC' .do begin
.seelist jasctime jctime
.do end
.seelist localtime localtime_s mktime strftime time tzset
