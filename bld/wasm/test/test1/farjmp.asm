.386
_text1 segment use32 'code'

jmp far ptr foo
jmp far ptr bar

_text1 ends

_text2 segment use16 'code'

foo: nop
jmp far ptr bar

_text2 ends

_text3 segment use32 'code'

bar: nop
jmp far ptr foo

_text3 ends
end
