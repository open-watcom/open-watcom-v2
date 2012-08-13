
comm	comm1:word

DGROUP		GROUP	_DATA

_DATA		SEGMENT	BYTE PUBLIC USE16 'DATA'
_DATA		ENDS

_TEXT		SEGMENT	dword PUBLIC USE16 'CODE'
		ASSUME CS:_TEXT, DS:DGROUP, SS:DGROUP

		EXTRN	word1:word

comm	comm2:word

		EXTRN	word2:word

comm	comm3:word

		EXTRN	word3:word

         mov ax,comm1
         mov ax,comm2
         mov ax,comm3
         mov ax,word1
         mov ax,word2
         mov ax,word3

_TEXT		ENDS
		END
