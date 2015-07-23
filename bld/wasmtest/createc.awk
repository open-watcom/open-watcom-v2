BEGIN {
    # Basic sanity check on input
    if( OUTFILE == "" ) {
        printf( "OUTFILE variable must be set!\n" ) > "/dev/stderr"
        exit 1
    }
    printf( "#ifdef __cplusplus\n" ) > OUTFILE
    printf( "extern \"C\" {\n" ) > OUTFILE
    printf( "#endif\n" ) > OUTFILE
    printf( "int        x;\n" ) > OUTFILE
    num = 1;
}
{
    printf( "extern void p%4.4d( void );\n", num ) > OUTFILE
    printf( "#pragma aux p%4.4d = ", num ) > OUTFILE
    if( target == "K3D" ) {
        printf( "\".586p\" \".K3D\" \".387\" " ) > OUTFILE
    } else if( target == "MMX" ) {
        printf( "\".586p\" \".MMX\" \".387\" " ) > OUTFILE
    } else if( target == "XMM" ) {
        printf( "\".686p\" \".XMM\" \".387\" " ) > OUTFILE
    } else if( target == "XMM2" ) {
        printf( "\".686p\" \".XMM2\" \".387\" " ) > OUTFILE
    } else if( target == "XMM3" ) {
        printf( "\".686p\" \".XMM3\" \".387\" " ) > OUTFILE
    } else {
        printf( "\".486p\" \".387\" " ) > OUTFILE
    }
    printf( "\"%s\"\n", $0 ) > OUTFILE
    printf( "void r%4.4d( void ) { p%4.4d(); }\n", num, num ) > OUTFILE
    num = num + 1;
}
END {
    printf( "#ifdef __cplusplus\n" ) > OUTFILE
    printf( "}\n" ) > OUTFILE
    printf( "#endif\n" ) > OUTFILE
}
