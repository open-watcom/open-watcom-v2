dseg segment 'data'
assume cs:dseg
foo dw ?
dseg ends
cseg segment 'code'
assume ss:dseg
jmp foo
cseg ends
end
