/^diag/d
/^.*Error! /s/^.*Error! //
/^.*Warning! /s/^.*Warning! //
/^blank delimited error message/d
/^Compiling generated function:/d
/^End of file/d
/^at least one error/d
/^\*!/d
/^ /d
/^\\/d
/' in '/s/' in '/_IN_/g
/function's/s/function's/functions/g
/'/s/'[^']*'/''/g
s/ $//
