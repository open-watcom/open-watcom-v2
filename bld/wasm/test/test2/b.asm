.model small
.data
.code
extrn bar:far
public foo
foo: xor ax, ax
     call bar
end
