#
#   PASS2.AWK  : produce a .fi file from a .h file
#
BEGIN {
    indent = "\t";
    in_structure = "";
    in_fn_proto = 0;
    init_special_types();
    init_types();

    print "c$noreference"
}

#!processed && /\/\* XLATOFF \*\// {
    ## grab XLATOFF comments before the comment handler
    #for(;;) {
   #getline;
   #if( match( $0, /\/\* XLATON \*\// ) ) break;
    #}
    #processed = 1;
#}

!processed && /^[ \t]*\/\*/ {
    # handle comments
    for(;;) {
   print "c " $0;
   if( match( $0, /\*\// ) ) break;
   getline;
    }
    processed = 1;
}

!processed && /^[ \t]*\r*$/ {
    # handle blank lines
    print;
    processed = 1;
}


function convert_int( str,  tmp ) {

    if( sub( "\)$", "", str ) ) {
   sub( "^\(", "", str );
    }
    sub( "^\([A-Za-z0-9_]+\)", "", str );
    gsub( /[UL]/, "", str );
    if( substr( str, 1, 2 ) == "0x" || substr( str, 1, 2 ) == "0X" ) {
   tmp = "00000000" substr( str, 3 );
   return "'" substr( tmp, length( tmp ) - 7 ) "'x";
    } else {
   return str;
    }
}

!processed && /^[ \t]*#define[ \t]+[A-Za-z_0-9]+[ \t]+\(?(\([A-Za-z0-9_]+\))?-?[0-9xXA-Fa-f]+U?L?U?L?\)?[ \t]*(\/\*.*\*\/)?[ \t]*\r*$/ {
    # handle #define const integer
    if( !( $2 in conv_to_int || $2 in is_a_reference ) ) {
   n = match( $0, /[ \t]*\/\*.*\*\/[ \t]*\r*$/ );
   if( n ) {
       x = substr( $0, n );
       sub( /\/\*/, "!", x );
       sub( /\*\//, "", x );
       print indent "integer\t", $2, x;
   } else {
       print indent "integer\t", $2;
   }
   print indent "parameter (" $2 " = " convert_int( $3 ) ")";
   processed = 1;
    }
}

!processed && /^[ \t]*#define[ \t]+[A-Za-z_0-9]+[ \t]+\(?[ \t]*[A-Za-z_][A-Za-z_0-9]*[ \t]*\)?[ \t]*(\/\*.*\*\/[ \t]*)?\r*$/ {
    # handle #define const other_const
    if( !( $2 in conv_to_int || $2 in is_a_reference ) ) {
   n = match( $0, /[ \t]*\/\*.*\*\/[ \t]*\r*$/ );
   if( n ) {
       x = substr( $0, n );
       sub( /\/\*/, "!", x );
       sub( /\*\//, "", x );
       print indent "integer\t", $2, x;
   } else {
       print indent "integer\t", $2;
   }
   print indent "parameter (" $2 " = " $3 ")";
   processed = 1;
    }
}

!processed && /^[ \t]*#define[ \t]+[A-Za-z_0-9]+[ \t]+\(?[ \t]*[A-Za-z_][A-Za-z_0-9]*[ \t]*\+[ \t]*[0-9xXA-Fa-f]+U?L?U?L?[ \t]*\)?[ \t]*(\/\*.*\*\/[ \t]*)?[ \t]*\r*$/ {
    # handle #define const other_const + integer
    if( !( $2 in conv_to_int || $2 in is_a_reference ) ) {
   n = match( $0, /[ \t]*\/\*.*\*\/[ \t]*\r*$/ );
   if( n ) {
       x = substr( $0, n );
       sub( /\/\*/, "!", x );
       sub( /\*\//, "", x );
       print indent "integer\t", $2, x;
   } else {
       print indent "integer\t", $2;
   }
   if( index( $0, "(" ) ) {
       x = substr( $0, index( $0, "(" ) + 1 );
       x = substr( x, 1, index( x, "+" ) - 1 );
       gsub( /[ \t\r]+/, "", x );
   } else {
       x = $3;
       sub( /\+.*/, "", x );
   }
   y = substr( $0, index( $0, "+" ) + 1 );
   sub( /\)/, "", y );
   sub( /\/\*.*\*\//, "", y );
   gsub( /[ \t\r]+/, "", y );
   print indent "parameter (" $2 " = " x " + " convert_int( y ) ")";
   processed = 1;
    }
}

!processed && /^[ \t]*#define[ \t]+[A-Za-z_0-9]+[ \t]+\(([ \t]*[A-Za-z0-9_]+[ \t]*\|)+[ \t]*[A-Za-z0-9_]+[ \t]*\)[ \t]*(\/\*.*\*\/)?[ \t]*\r*$/ {

    # handle #define const ( x | y | z )
    if( !( $2 in conv_to_int || $2 in is_a_reference ) ) {
   x = substr( $0, index( $0, "(" ) + 1 );
   x = substr( x, 1, index( x, ")" ) - 1 );
   gsub( /\|/, ".or.", x );
   print indent "integer\t" $2;
   print indent "parameter (" $2 " = " x ")";
   processed = 1;
    }
}

!processed && /^[ \t]*#define[ \t]+[A-Za-z_0-9]+[ \t]+\(([ \t]*[A-Za-z0-9_]+[ \t]*\+)+[ \t]*[A-Za-z0-9_]+[ \t]*\)[ \t]*(\/\*.*\*\/)?[ \t]*\r*$/ {

    # handle #define const ( x + y + z )
    if( !( $2 in conv_to_int || $2 in is_a_reference ) ) {
   x = substr( $0, index( $0, "(" ) + 1 );
   x = substr( x, 1, index( x, ")" ) - 1 );
   print indent "integer\t" $2;
   print indent "parameter (" $2 " = " x ")";
   processed = 1;
    }
}

!processed && /^[ \t]*#define[ \t]+[A-Za-z_0-9]+[ \t]+\".*\"[ \t]*(\/\*.*\*\/)?[ \t]*\r*$/ {
    # handle #define const string_const
    if( !( $2 in conv_to_int || $2 in is_a_reference ) ) {
   x = substr( $0, index( $0, "\"" ) + 1 );
   x = substr( x, 1, index( x, "\"" ) - 1 );
   n = match( $0, /[ \t]*\/\*.*\*\/[ \t]*\r*$/ );
   if( n ) {
       y = substr( $0, n );
       sub( /\/\*/, "!", y );
       sub( /\*\//, "", y );
       print indent "character*" ( length( x ) + 1 ) "\t", $2, y;
   } else {
       print indent "character*" ( length( x ) + 1 ) "\t", $2;
   }
   print indent "parameter (" $2 " = '" x "'//char(0))";
   processed = 1;
    }
}

!processed && /^[ \t]*#(define|ifdef|undef|ifndef)[ \t]+[A-Za-z0-9_]+[ \t]*(\/\*.*\*\/[ \t]*)?\r*$/ {
    /* handle some preprocessor directives */
    sub( /#/, "", $1 );
    print "c$" $1, $2;
    processed = 1;
}

!processed && /^[ \t]*#include[ \t]+[."<>A-Za-z0-9_]+[ \t]*(\/\*.*\*\/[ \t]*)?\r*$/ {
    /* handle some preprocessor directives */
    gsub( /[<\">]/, "", $2 );
    sub( /\.h/, ".fi", $2 );
    print "c$" substr( $1, 2 ), $2;
    processed = 1;
}

!processed && /^[ \t]*#(endif|else)[ \t]*(\/\*.*\*\/[ \t]*)?\r*$/ {
    print "c$" substr( $1, 2 );
    processed = 1;
}

!processed && /^[ \t]*#pragma.*$/ {
    processed = 1;
}

!processed && /^[ \t]*typedef[ \t]+struct[ \t]+{/ {
    in_structure = "##unknown_struct_name##";
    print indent "structure /" in_structure "/";
    processed = 1;
}

!processed && /^[ \t]*typedef[ \t]+_Packed[ \t]+struct[ \t]+_[A-Za-z0-9_]+[ \t]*{?([ \t]*\/\*.*\*\/[ \t]*)?\r*$/ {
    gsub( /\r+/, "", $4 );
    in_structure = substr( $4, 2 );
    print indent "structure /" in_structure "/";
    processed = 1;
}

!processed && /^[ \t]*typedef[ \t]+struct[ \t]+_[A-Za-z0-9_]+[ \t]*{?([ \t]*\/\*.*\*\/[ \t]*)?\r*$/ {
    gsub( /\r+/, "", $3 );
    in_structure = substr( $3, 2 );
    print indent "structure /" in_structure "/";
    processed = 1;
}

!processed && /^[ \t]*typedef[ \t]+struct[ \t]+[A-Za-z0-9_]+[ \t]*{?([ \t]*\/\*.*\*\/[ \t]*)?\r*$/ {
    gsub( /\r+/, "", $3 );
    in_structure = $3;
    print indent "structure /" in_structure "/";
    processed = 1;
}

!processed && /^[ \t]*struct[ \t]+_[A-Za-z0-9_]+[ \t]*{?([ \t]*\/\*.*\*\/[ \t]*)?\r*$/ {
    gsub( /\r+/, "", $2 );
    in_structure = substr( $2, 2 );
    print indent "structure /" in_structure "/";
    processed = 1;
}

!processed && /^[ \t]*struct[ \t]+[A-Za-z0-9_]+[ \t]*{?([ \t]*\/\*.*\*\/[ \t]*)?\r*$/ {
    gsub( /\r+/, "", $2 );
    in_structure = $2;
    print indent "structure /" in_structure "/";
    processed = 1;
}

!processed && in_structure != "" && /^[ \t]*\{[ \t]*\r*$/ {
    processed = 1;
}

!processed && in_structure != "" && /^[ \t]*\}[ \t]+[A-Za-z0-9_]+[ \t]*;/ {
    x = $2
    sub( /;.*/, "", x );
    if( tolower( x ) == tolower( in_structure ) ) {
   print indent "end structure";
   processed = 1;
   in_structure = "";
    } else {
   print "c## tried to end '" x "'";
    }
}

!processed && in_structure != "" && /^[ \t]*\}[ \t]+[A-Za-z0-9_]+[ \t]*,[ \t]+\*[ \t]*[A-Za-z0-9_]+[ \t]*;/ {
    x = $2
    sub( /,.*/, "", x );
    if( tolower( x ) == tolower( in_structure ) ) {
   print indent "end structure";
   processed = 1;
   in_structure = "";
    } else {
   print "c## tried to end '" x "'";
    }
}

!processed && in_structure != "" && /[ \t]*}[ \t]*;/ {
    print indent "end structure";
    processed = 1;
    in_structure = "";
}

function convert_type( str ) {

    if( str in is_a_reference ) return( "integer*4" );
    if( str in is_a_struct ) return( "record /" str "/" );
    if( str in conv_to_int ) return( "integer*"conv_to_int[ str ] );
    if( substr( str, 1, 1 ) == "P" && substr( str, 2 ) in conv_to_int ) return( "integer*4" );
    return( "##" str "##" );
}

!processed && in_structure != "" && /^[ \t]*[A-Za-z0-9_]+[ \t]+([A-Za-z0-9_])+[ \t]*;/ {
    gsub( /;.*/, "", $2 );
    print indent "    " convert_type( $1 ), $2;
    processed = 1;
}

!processed && in_structure != "" && /^[ \t]*[A-Za-z0-9_]+[ \t]+([A-Za-z0-9_])+[ \t]*\[.*\];/ {
    x = substr( $0, index( $0, "[" ) );
    x = substr( x, 1, index( x, ";" ) - 1 );
    sub( /\[/, "(", x );
    sub( /\]/, ")", x );
    gsub( /\[.*/, "", $2 );
    print indent "    " convert_type( $1 ), $2 x;
    processed = 1;
}

!processed && /^[ \t]*typedef[ \t]+([A-Za-z0-9_]+[ \t]+)?((FAR|NEAR)[ \t]+)?\*[ \t]*[A-Za-z0-9_]+[ \t]*;/ {
    # delete typedef FOO *BAR;
    processed = 1;
}

!processed && /^[ \t]*typedef[ \t]+[A-Za-z0-9_]+[ \t]+[A-Za-z0-9_]+[ \t]*;/ {
    # delete typedef BASE NEW_TYPE;
    if( $2 in conv_to_int || $2 in is_a_reference) {
   processed = 1;
    }
}

function proto_one_arg( str,  a,n ) {
    n = split( str, a );
    if( n > 2 ) {
   return( "##" str "##" );
    }
    if( a[1] in is_a_reference ) {
   return( "reference" );
    }
    if( a[1] in conv_to_int ) {
   return( "value*" conv_to_int[ a[1] ] );
    }
    if( substr( a[1], 1, 1 ) == "P" && substr( a[1], 2 ) in conv_to_int ) {
   return( "reference" );
    }
    if( toupper( a[1] ) == "VOID" && ( n == 1 || substr( a[2],1,1 ) != "*" ) ) {
   return( "" );
    }
    return( "##" str "##" );
}

function proto_str( str,  args,n,i,res ) {
    sub( /\/\*.*\*\//, "", str );
    n = split( str, args, "[,\);]" );
    while( args[ n ] ~ /^[ \t]*\r*$/ ) --n;
    res = "";
    for( i = 1; i < n; ++i ) {
   res = res proto_one_arg( args[ i ] ) ", ";
    }
    res = res proto_one_arg( args[ n ] );
    return( res );
}

!processed && /^[ \t]*[A-Za-z0-9_]+[ \t]+APIENTRY(16)?[ \t]+[A-Za-z0-9_]+[ \t]*\(.*\)[ \t]*;/ {

    l = index( $0, "(" );
    r = index( $0, ")" );
    x = substr( $0, l + 1, r - l - 1 );
    sub( /\(.*\r*$/, "", $3 );
    print indent "external", $3
    if( $1 != "VOID" ) {
   print indent convert_type( $1 ), $3
    }
    print "c$pragma aux (__syscall)", $3, "parm(", proto_str( x ), ")"
    processed = 1;
}

!processed && /^[ \t]*[A-Za-z0-9_]+[ \t]+APIENTRY(16)?[ \t]+[A-Za-z0-9_]+[ \t]*\(.*,/ {

    l = index( $0, "(" );
    r = index( $0, "," );
    x = substr( $0, l + 1, r - l - 1 );
    sub( /\(.*\r*$/, "", $3 );
    print indent "external", $3
    if( $1 != "VOID" ) {
   print indent convert_type( $1 ), $3
    }
    print "c$pragma aux (__syscall)", $3, "parm(", proto_str( x ) ", \\"
    in_fn_proto = 1;
    processed = 1;
}

!processed && /^[ \t]*[A-Za-z0-9_]+[ \t]+APIENTRY(16)?[ \t]+[A-Za-z0-9_]+[ \t]*\(/ {

    sub( /\(.*\r*$/, "", $3 );
    print indent "external", $3
    if( $1 != "VOID" ) {
   print indent convert_type( $1 ), $3
    }
    in_fn_proto = 1;
    print "c$pragma aux (__syscall)", $3, "parm( \\"
    processed = 1;
}

!processed && in_fn_proto && /^[ \t]*[A-Za-z0-9_]+([ \t]+\*?[A-Za-z0-9_]+)?[ \t]*,/ {

    sub( /^[ \t]*/, "", $0 );
    sub( /,.*\r*$/, "", $0 );
    print "c" indent indent proto_str( $0 ) ", \\"
    processed = 1;
}

!processed && in_fn_proto && /^[ \t]*[A-Za-z0-9_]+([ \t]+\*?[A-Za-z0-9_]+)?[ \t]*\)[ \t]*;/ {

    sub( /^[ \t]*/, "", $0 );
    sub( /[ \t]*\)[ \t]*;.*\r*$/, "", $0 );
    print "c" indent indent proto_str( $0 ), ")"
    in_fn_proto = 0;
    processed = 1;
}

{
    if( !processed ) {
   print "c##", $0;
    } else {
   processed = 0;
    }
}

END {
    print "c$reference"
}
