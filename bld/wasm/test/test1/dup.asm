.386
.model small
.data
    q db 4 dup( 3 dup ( 1, 2 ) )

    r db 4 dup( 3 dup ( 1, 2 ), 2 dup ( 4, 5 ) )
    
    s db 0 dup( 27 )
end
