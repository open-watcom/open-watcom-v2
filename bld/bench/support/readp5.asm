.586
.model flat

public  ReadP5Timer

.code

; long long ReadP5Timer( void )
ReadP5Timer     proc
        rdtsc
        ret
ReadP5Timer     endp

end
