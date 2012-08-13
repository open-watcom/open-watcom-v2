.386
.model small
.code
foo: jmp word ptr foo
     jmp dword ptr foo
     jmp fword ptr foo
     jmp near ptr foo
     jmp far  ptr foo
end
