#
#   PASS1.AWK  : get all typing information from a headerfile
#
BEGIN {
    indent = "    ";
    init_special_types();
    init_types();
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
   if( match( $0, /\*\// ) ) break;
   getline;
    }
    processed = 1;
}

function add_a_struct( name,   x ) {

    gsub( /\r+/, "", name );
    if( !( name in is_a_struct ) ) {
   is_a_struct[ name ] = 1;
   print indent "is_a_struct[ \"" name "\" ] = 1;"
   x = toupper( name );
   if( x != name ) {
       is_a_struct[ x ] = 1;
       print indent "is_a_struct[ \"" x "\" ] = 1;"
   }
    }
}


!processed && /^[ \t]*typedef[ \t]+_Packed[ \t]+struct[ \t]+_[A-Za-z0-9_]+[ \t]*{?([ \t]*\/\*.*\*\/[ \t]*)?\r*$/ {
    add_a_struct( substr( $4, 2 ) );
    processed = 1;
}

!processed && /^[ \t]*typedef[ \t]+struct[ \t]+_[A-Za-z0-9_]+[ \t]*{?([ \t]*\/\*.*\*\/[ \t]*)?\r*$/ {
    add_a_struct( substr( $3, 2 ) );
    processed = 1;
}

!processed && /^[ \t]*typedef[ \t]+struct[ \t]+[A-Za-z0-9_]+[ \t]*{?([ \t]*\/\*.*\*\/[ \t]*)?\r*$/ {
    add_a_struct( $3 );
    processed = 1;
}

!processed && /^[ \t]*struct[ \t]+_[A-Za-z0-9_]+[ \t]*{?([ \t]*\/\*.*\*\/[ \t]*)?\r*$/ {
    add_a_struct( substr( $2, 2 ) );
    processed = 1;
}

!processed && /^[ \t]*struct[ \t]+[A-Za-z0-9_]+[ \t]*{?([ \t]*\/\*.*\*\/[ \t]*)?\r*$/ {
    add_a_struct( $2 );
    processed = 1;
}

function add_a_reference( name,   x ) {

    if( !( name in conv_to_int ) && !( name in is_a_reference ) ) {
   is_a_reference[ name ] = 1;
   print indent "is_a_reference[ \"" name "\" ] = 1;"
   x = toupper( name );
   if( x != name ) {
       is_a_reference[ x ] = 1;
       print indent "is_a_reference[ \"" x "\" ] = 1;"
   }
    }
}

!processed && /^[ \t]*}[ \t]*[A-Za-z0-9_]+,[ \t]+\*[A-Za-z0-9_]+;/ {
    x = substr( $3, 2 );
    sub( /;.*/, "", x );
    add_a_reference( x );
    processed = 1;
}

!processed && /^[ \t]*}[ \t]*[A-Za-z0-9_]+,[ \t]+\*[ \t]+[A-Za-z0-9_]+;/ {
    sub( /;.*/, "", $4 );
    add_a_reference( $4 );
    processed = 1;
}

!processed && /^[ \t]*typedef[ \t]+([A-Za-z0-9_]+[ \t]+)?((FAR|NEAR)[ \t]+)?\*[ \t]*[A-Za-z0-9_]+;/ {
    # handle:
    # typedef FOO *BAR;
    x = substr( $0, index( $0, "*" ) + 1 );
    x = substr( x, 1, index( x, ";" ) - 1 );
    gsub( /[ \t\r]/, "", x );
    add_a_reference( x );
    processed = 1;
}

!processed && /^[ \t]*typedef[ \t]+[A-Za-z0-9_]+[ \t]+[A-Za-z0-9_]+;/ {

    # handle typedef BASE NEW_TYPE;
    if( $2 in is_a_reference ) {
   sub( /;.*/, "", $3 );
   if( !( $3 in conv_to_int ) ) {
       conv_to_int[ $3 ] = 4;
       print indent "conv_to_int[ \"" $3 "\" ] = 4;";
   }
   processed = 1;
    } else if( $2 in conv_to_int ) {
   sub( /;.*/, "", $3 );
   if( !( $3 in conv_to_int ) ) {
       conv_to_int[ $3 ] = conv_to_int[ $2 ];
       print indent "conv_to_int[ \"" $3 "\" ] = " conv_to_int[ $2 ] ";";
   }
   processed = 1;
    }
}

!processed && /^[ \t]*typedef[ \t].*\r*$/ {

    if( processed == 0 ) {
   print "## " $0
    }
}

{
    processed = 0;
}
