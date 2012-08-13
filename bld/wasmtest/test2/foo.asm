.model small
.data
    foo db "this is a string with a ; in it "
    bar db "this is a string with no semi in it "
.code
    xor ax, ax
end
