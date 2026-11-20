	.386p

load_pe_app:
	mov	bptr _app_type,3
	mov	ax,3004h		; "app exec format not supported"
	jmp	file_error
