_TEXT segment byte public 'code'
includelib abcd.lib

assume cs:_text
assume es:_text
assume es:far_text
extrn nearlabel:near
extrn farlabel:far
extrn hug:word

NearProc PROC far c,
abc:word
local jfk[5]:word,
jap:fword
mov ax, abc
mov ax, es:hug
;es:
mov ax, hug
call FarProc
ret 5
NearProc        ENDP

;abc dw ?

_TEXT ends

FAR_TEXT segment byte public 'far_code'
assume cs:far_text

FarProc proc far c, pig:word, gik:vararg
local abc:word
mov ax,pig
mov bx,bx
mov dx,gik
ret
FarProc endp

far_text ends


end
