.func begin
.funct   freopen_s   TR 24731
.funct_w _wfreopen_s
.func gen
#include <stdio.h>
#define __STDC_WANT_LIB_EXT1__ 1
errno_t  freopen_s( FILE * restrict * restrict newstreamptr,
                    const char * filename,
                    const char * restrict mode,
                    FILE * restrict stream );
.ixfunc2 '&StrIo' &func
.if &'length(&wfunc.) ne 0 .do begin
errno_t _wfreopen_s( FILE * restrict * restrict newstreamptr,
                     const wchar_t * restrict filename,
                     const wchar_t * restrict mode,
                     FILE * restrict stream );
.ixfunc2 '&StrIo' &wfunc
.ixfunc2 '&Wide' &wfunc
.do end
.synop end
.*
.rtconst begin
None of
.arg newstreamptr
.ct ,
.arg mode
.ct , and
.arg stream
shall be a null pointer.
If there is a runtime-constraint violation, &func neither attempts to close any file
associated with
.arg stream
nor attempts to open a file. Furthermore, if
.arg newstreamptr
is not a null pointer, &func sets
.arg *newstreamptr
to the null pointer.
.rtconst end
.*
.desc begin
The &func function opens the file whose name is the string pointed to by
.arg filename
and associates the stream pointed to by
.arg stream
with it. The
.arg mode
argument has the same meaning as in the fopen_s function (including the mode's effect
on exclusive access and file permissions).
If
.arg filename
is a null pointer,the &func function attempts to change the mode of
the
.arg stream
to that specified by
.arg mode
,as if the name of the file currently associated with
the stream had been used. It is implementation-defined which changes of mode are
permitted (if any), and under what circumstances.
The &func function first attempts to close any file that is associated with
.arg stream
.ct .li .
Failure to close the file is ignored. The error and end-of-file indicators for the stream are
cleared.
If the file was opened successfully, then the pointer to FILE pointed to by
.arg newstreamptr
will be set to the value of stream. Otherwise, the pointer to FILE
pointed to by
.arg newstreamptr
will be set to a null pointer.
.if &'length(&wfunc.) ne 0 .do begin
.np
The &wfunc function is identical to &func except that it accepts
wide-character string arguments for
.arg filename
and
.arg mode
.ct .li .
.do end
.desc end
.*
.return begin
The &func function returns zero if it opened the file. If it did not open the file or
there was a runtime-constraint violation, &func returns a non-zero value.
.return end
.see begin
.seelist freopen_s _dos_open fclose fcloseall fdopen fopen fopen_s freopen
.seelist freopen_s _fsopen _grow_handles _hdopen open _open_osfhandle
.seelist freopen_s _popen sopen
.see end
.exmp begin
#define __STDC_WANT_LIB_EXT1__ 1
#include <stdio.h>

void main()
{
    errno_t rc;
    FILE    *fp;
    int     c;
.exmp break
    rc = freopen_s( &fp, "file", "r", stdin );
    if( rc == 0 ) {
      while( (c = fgetc( fp )) != EOF )
        fputchar(c);
      fclose( fp );
    }
}
.exmp end
.classt
.system
