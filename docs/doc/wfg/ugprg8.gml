
<beginning of file>
* This is our first FORTRAN example program

      PRINT *, 'This is a FORTRAN program which I changed'
      PRINT *, 'to illustrate the editor
      END
<end of file>




run
.if '&machine' eq '80386' .do begin
no_name.for(4): *ERR* SX-21 column 15, no closing quote on literal string
.do end
.el .do begin
*ERR* SX-23 line 4, column 15, no closing quote on literal string
.do end
.ra <hold>
