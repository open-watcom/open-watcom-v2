 mov word ptr x,cs
 mov cs,word ptr x
 mov word ptr x,ds
 mov ds,word ptr x
 mov word ptr x,es
 mov es,word ptr x
 mov word ptr x,fs
 mov fs,word ptr x
 mov word ptr x,gs
 mov gs,word ptr x
 mov word ptr x,ss
 mov ss,word ptr x
 push cs
 push ds
 push es
 push fs
 push gs
 push ss
 pop ds
 pop es
 pop fs
 pop gs
 pop ss
