        .386
.model	FLAT
_TEXT	SEGMENT 'CODE'

EXTRN	_testdata:DWORD
PUBLIC	testfunc_

testfunc_ PROC NEAR
	lea	eax, _testdata
	ret
testfunc_ ENDP

_TEXT	ENDS
END
