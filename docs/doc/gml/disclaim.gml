.if '&isbn' ne '&amp.isbn' .do begin
.if '&isbn' ne '' .do begin
.np
ISBN &isbn
.do end
.do end
