.386
_DATA segment use32 'data'
    foo LABEL fword
    dd 0
    dw 0
_DATA ends

_KYB segment use32 'data'
    bar LABEL fword
    dd 0
    dw 0
_KYB ends


if 0
_TEXT segment use16 'code'
    assume es:_DATA, cs:_TEXT
    call foo
_TEXT ends
endif

_THIRTY_TWO segment use32 'code'
    assume fs:_KYB, es:_DATA, ds:error, cs:_THIRTY_TWO
    call foo
    call bar
_THIRTY_TWO ends

end
