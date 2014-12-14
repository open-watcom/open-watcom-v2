.* ALLOCATION FAMILY
.if &farfnc eq 0 .do begin
.seelist calloc
.seelist free
.seelist malloc
.seelist realloc
.seelist sbrk
.do end
.el .do begin
.seelist calloc Functions
.seelist _expand Functions
.seelist free Functions
.seelist halloc hfree
.seelist malloc Functions
.seelist _msize Functions
.seelist realloc Functions
.seelist sbrk
.do end
