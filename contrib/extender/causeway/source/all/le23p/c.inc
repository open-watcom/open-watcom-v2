;*******************************************************************************
;PSP selector
;*******************************************************************************
	externdef __psp:word


;*******************************************************************************
;Command line arguments
;*******************************************************************************
	externdef __argv:dword


;*******************************************************************************
;Allocate some memory.
;
;Usage: malloc(int bytes);
;
;Returns:
;
;EAX	- memory allocated, zero if failed.
;
;*******************************************************************************
	externdef malloc:near
	externdef malloc_:near


;*******************************************************************************
;Re-allocate some memory.
;
;Usage: realloc(void *oldmem, int bytes);
;
;Returns:
;
;EAX	- new memory address, zero if failed.
;
;*******************************************************************************
	externdef realloc:near
	externdef realloc_:near


;*******************************************************************************
;Release some memory.
;
;Usage: free(void *memory);
;
;Returns:
;
;*******************************************************************************
	externdef free:near
	externdef free_:near


;*******************************************************************************
;Copy string.
;
;Usage: strcpy(char *dest, char *source);
;
;Returns:
;
;EAX	- char *dest
;
;*******************************************************************************
	externdef strcpy:near
	externdef strcpy_:near


;*******************************************************************************
;Append a string.
;
;Usage: strcat(char *dest, char *source);
;
;Returns:
;
;EAX	- char *dest
;
;*******************************************************************************
	externdef strcat:near
	externdef strcat_:near


;*******************************************************************************
;Get length of a string.
;
;Usage: strlen(char *source);
;
;Returns:
;
;EAX	- string length
;
;*******************************************************************************
	externdef strlen:near
	externdef strlen_:near


;*******************************************************************************
;Open a file.
;
;Usage: open(char *filename, int mode, [int cmode]);
;
;filename	- name of file to open.
;mode	- mode to open file in.
;cmode	- only needed for O_CREAT, defines files attributes.
;
;Returns:
;
;EAX	- file handle (zero if open failed).
;
;*******************************************************************************
	externdef open:near
	externdef open_:near
O_RDONLY	equ 0000h	; open for read only
O_WRONLY	equ 0001h	; open for write only
O_RDWR	equ 0002h	; open for read and write
O_APPEND	equ 0010h	; writes done at end of file
O_CREAT 	equ 0020h	; create new file
O_TRUNC 	equ 0040h	; truncate existing file
O_NOINHERIT	equ 0080h	; file is not inherited by child process
O_TEXT	equ 0100h	; text file
O_BINARY	equ 0200h	; binary file
O_EXCL	equ 0400h	; exclusive open

S_IRWXU 	equ 0700h	; read/write,execute,search
S_IRUSR 	equ 0400h	; read



;*******************************************************************************
;Close a file.
;
;Usage: close(int handle);
;
;handle	- handle of file to close.
;
;Returns:
;
;EAX	- zero for success, -1 for failure.
;
;*******************************************************************************
	externdef close:near
	externdef close_:near


;*******************************************************************************
;Set file pointer.
;
;Usage: lseek(int handle, int offset, int mode);
;
;handle	- file handle
;offset	- offset to set.
;mode	- seek mode.
;
;Returns:
;
;EAX	- new position.
;
;*******************************************************************************
	externdef lseek:near
	externdef lseek_:near
SEEK_SET	equ	0
SEEK_CUR	equ	1
SEEK_END	equ	2


;*******************************************************************************
;Read from a file.
;
;Usage: read(int handle, void *buffer, int bytes);
;
;handle	- file handle
;buffer	- buffer to read to
;bytes	- bytes to read.
;
;Returns:
;
;EAX	- number of bytes read, -1 on error.
;
;*******************************************************************************
	externdef read:near
	externdef read_:near


;*******************************************************************************
;Write to a file.
;
;Usage: write(int handle, void *buffer, int bytes);
;
;handle	- file handle
;buffer	- buffer to write to
;bytes	- bytes to write.
;
;Returns:
;
;EAX	- number of bytes written, -1 on error.
;
;*******************************************************************************
	externdef write:near
	externdef write_:near


	externdef kbhit:near
	externdef kbhit_:near

