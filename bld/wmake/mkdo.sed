s/^\	//
/%null/d
s/%stop/goto done/
s/%quit/goto done/
$a\
:done
