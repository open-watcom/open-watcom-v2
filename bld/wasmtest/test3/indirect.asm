.386

.model small

.data

extrn STUBS_START:BYTE

.code

	jmp [dword ptr STUBS_START + 42]
	jmp ds:[dword ptr STUBS_START + 42]
	jmp cs:[dword ptr STUBS_START + 42]
	jmp [dword ptr ds:STUBS_START + 42]
	jmp [dword ptr cs:STUBS_START + 42]

end
