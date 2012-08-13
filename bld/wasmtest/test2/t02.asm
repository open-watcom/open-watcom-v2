assume cs:don
public cde
andrew group don,patrick,daniel
extrn foo:byte, poo:word
don segment para public 'data'
        woo dt ?
        big db 21+1,22,23,24,25
patrick segment word public 'data'
abc dw 4 dup (1,2,3,4)
patrick ends
don ends
don segment para public 'data'
        cde dw ?
flora segment word public 'code'
        assume ss:don
        mov bx,cde
flora ends
daniel segment word public 'data'
daniel ends
don ends
end
