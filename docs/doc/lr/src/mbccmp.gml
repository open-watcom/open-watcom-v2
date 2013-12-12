.func _mbccmp _fmbccmp
#include <mbstring.h>
int _mbccmp( const unsigned char *s1,
             const unsigned char *s2 );
.ixfunc2 '&Multibyte' &func
.if &farfnc eq 1 .do begin
int _fmbccmp( const unsigned char __far *s1,
              const unsigned char __far *s2 );
.ixfunc2 '&Multibyte' &ffunc
.do end
.synop end
.desc begin
The &func function compares one multibyte character from
.arg s1
to one multibyte character from
.arg s2
.ct .li .
.im farparm
.desc end
.return begin
.if &farfnc eq 0 .do begin
The &func function returns
.do end
.el .do begin
The &func and &ffunc functions return
.do end
the following values.
.begnote
.termhd1 Value
.termhd2 Meaning
.note < 0
multibyte character at
.arg s1
less than multibyte character at
.arg s2
.note 0
multibyte character at
.arg s1
identical to multibyte character at
.arg s2
.note > 0
multibyte character at
.arg s1
greater than multibyte character at
.arg s2
.endnote
:CMT. .np
:CMT. On an error,
:CMT. .kw _NLSCMPERROR
:CMT. is returned (defined in
:CMT. .hdrfile string.h
:CMT. and
:CMT. .hdrfile mbstring.h
:CMT. .ct ).
.return end
.see begin
.im seembc &function.
.see end
.exmp begin
#include <stdio.h>
#include <mbctype.h>
#include <mbstring.h>

unsigned char mb1[2] = {
    0x81, 0x43
};

unsigned char mb2[2] = {
    0x81, 0x42
};

void main()
  {
    int     i;

    _setmbcp( 932 );
    i = _mbccmp( mb1, mb2 );
    if( i < 0 )
        printf( "Less than\n" );
    else if( i == 0 )
        printf( "Equal to\n" );
    else
        printf( "Greater than\n" );

  }
.exmp output
Greater than
.exmp end
.class ANSI
.system
