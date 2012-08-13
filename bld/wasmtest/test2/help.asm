.MODEL small

sam = 1
bar = sam
foo = bar

.data

testing db "foo bar 1"
help macro x, y, z
    var1 db "&x"
    var2 db "&y"
    var3 db "&z"
    var4 db z

endm

help foo, %foo, %%foo
.code
xor ax, ax

end
