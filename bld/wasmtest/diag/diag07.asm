.model small

.data

data1 db 15 dup( 0 )
data2 db 0

.code

    mov  bl,[si+data2-(data1+di)]
    mov  [si+data2-(data1+bx)],bl
    mov  bl,[si+data2-data1-di]
    mov  [si+data2-data1-bx],bl

END
