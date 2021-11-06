.if '&isbn' ne '&amp.isbn' .do begin
.if &'length(&isbn.) ne 0 .do begin
.np
ISBN &isbn
.do end
.do end
