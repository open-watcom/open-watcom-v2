.386
.model small
extern foo:word
.data
bar = 2 + 2 + foo + 4
.code
end
