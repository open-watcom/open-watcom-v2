BEGIN {
    printf( "#ifdef __cplusplus\n" );
    printf( "extern \"C\" {\n" );
    printf( "#endif\n" );
    printf( "int	x;\n" );
    num = 1;
}
{
    printf( "extern void p%4.4d( void );\n", num );
    printf( "#pragma aux p%4.4d = ", num );
    if( target == "K3D" ) {
        printf( "\".586p\" \".K3D\" \".387\" " );
    } else if( target == "MMX" ) {
        printf( "\".586p\" \".MMX\" \".387\" " );
    } else if( target == "XMM" ) {
        printf( "\".686p\" \".XMM\" \".387\" " );
    } else if( target == "XMM2" ) {
        printf( "\".686p\" \".XMM2\" \".387\" " );
    } else if( target == "XMM3" ) {
        printf( "\".686p\" \".XMM3\" \".387\" " );
    } else {
        printf( "\".486p\" \".387\" " );
    }
    printf( "\"%s\"\n", $0 );
    printf( "void r%4.4d( void ) { p%4.4d(); }\n", num, num );
    num = num + 1;
}
END {
    printf( "#ifdef __cplusplus\n" );
    printf( "}\n" );
    printf( "#endif\n" );
}
