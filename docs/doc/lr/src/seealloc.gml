.* ALLOCATION FAMILY
.if &farfnc eq 0 .do begin
.seelist &*1 calloc
.seelist &*1 free
.seelist &*1 malloc
.seelist &*1 realloc
.seelist &*1 sbrk
.seelist &*1 _smalloc
.do end
.el .do begin
.seelist &*1 calloc Functions
.seelist &*1 _expand Functions
.seelist &*1 free Functions
.seelist &*1 halloc hfree
.seelist &*1 malloc Functions
.seelist &*1 _msize Functions
.seelist &*1 realloc Functions
.seelist &*1 sbrk
.seelist &*1 _smalloc
.do end
