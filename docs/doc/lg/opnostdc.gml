.*
.*
.option NOSTDCALL
.*
.np
The "NOSTDCALL" option specifies that the characters unique to the
.kw __stdcall
calling convention be trimmed from all of the symbols that are
exported from the DLL being created.
The format of the "NOSTDCALL" option (short form "NOSTDC") is as
follows.
.mbigbox
    OPTION NOSTDCALL
.embigbox
.np
Considering the following declarations.
.exam begin
short PASCAL __export Function1( short var1,
                                 long varlong,
                                 short var2 );

short PASCAL __export Function2( long varlong,
                                 short var2 );
.exam end
.np
Under ordinary circumstances, these
.kw __stdcall
symbols are mapped to "_Function1@12" and "_Function2@8" respectively.
The "@12" and "@8" reflect the number of bytes in the argument list
(short is passed as int).
When the "NOSTDCALL" option is specified, these symbols are stripped
of the "_" and "@xx" adornments.
Thus they are exported from the DLL as "Function1" and "Function2".
.np
This option makes it easier to access functions exported from DLLs,
especially when using other software languages such as FORTRAN which
do not add on the
.kw __stdcall
adornments.
.remark
Use the "IMPLIB" option to create an import library for the DLL which
can be used with software languages that add on the
.kw __stdcall
adornments.
.eremark
