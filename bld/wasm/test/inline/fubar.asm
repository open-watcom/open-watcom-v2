.386p
.model small
.data
x dd 1234
.code
    push 0fH
    push 7fH
    push 0ffH
    push 7fffffffH
    push 0ffffffffH
end
