BEGIN {
    print "<HTML><BODY><UL>"
}

function transsym( str ) {
    targ = str
    gsub( / /, "_", targ )
    gsub( /-/, "M", targ )
    gsub( /\//, "D", targ )
    gsub( /\+/, "P", targ )
    gsub( /\[/, "U", targ )
    gsub( /\]/, "V", targ )
    gsub( /</, "X", targ )
    gsub( />/, "Y", targ )
    gsub( /\=/, "E", targ )
    gsub( /{/, "_", targ )
    gsub( /}/, "_", targ )
    gsub( /,/, "_", targ )
    gsub( /\#/, "_", targ )
    gsub( /\?/, "_", targ )
    gsub( /\./, "_", targ )
    gsub( /\|/, "_", targ )
    gsub( /\"/, "_", targ )
    gsub( /'/, "_", targ )
    gsub( /:/, "_", targ )
    gsub( /\(/, "L", targ )
    gsub( /\)/, "R", targ )
    gsub( /\*/, "T", targ )
    gsub( /\\/, "B", targ )
    gsub( /%/, "_", targ )
    gsub( /~/, "_", targ )
    gsub( /@/, "A", targ )
    gsub( /\$/, "_", targ )
    gsub( /&/, "_", targ )
    gsub( /!/, "_", targ )
    gsub( /\r/, "", targ )
    return targ
}

function fname() {
    if( length( sectid ) ) {
        fn = sectid
    } else {
        fn = prefix file
    }
    return fn
}

/\.helppref / {
    prefix = $0
    gsub( /\.helppref /, "", prefix )
    if( length( prefix ) )
       prefix = transsym( prefix ) "_"
}

/\.ixchap / {
    sectid = ""
    file = $0
    gsub( /\.ixchap /, "", file )
    file = transsym( file )
}

/\.ixsect / {
    sectid = ""
    file = $0
    gsub( /\.ixsect /, "", file )
    file = transsym( file )
}

/\.ixsectid / {
    sectid = $0
    gsub( /\.ixsectid /, "", sectid )
    gsub( /\r/, "", sectid )
}

/\.ixline '(.*)' '(.*)'/ {
    name1 = $0
    gsub( /\.ixline '/, "", name1 )
    gsub( /' '(.*)'[\r]?/, "", name1 )
    name2 = $0
    gsub( /\.ixline '(.*)' '/, "", name2 )
    gsub( /'[\r]?/, "", name2 )
    print "<LI><OBJECT TYPE=\"text/sitemap\">"
    print "<PARAM NAME=\"Name\" VALUE=\"" name1 "\">"
    print "<PARAM NAME=\"See also\" VALUE=\"" name1 "\">"
    print "</OBJECT>"
    print "<UL>"
    print "<LI><OBJECT TYPE=\"text/sitemap\">"
    print "<PARAM NAME=\"Name\" VALUE=\"" name2 "\">"
    print "<PARAM NAME=\"Local\" VALUE=\"" fname() ".htm\">"
    print "</OBJECT>"
    print "</UL>"
    next
}

/\.ixline '(.*)'/ {
    name = $0
    gsub( /\.ixline /, "", name )
    gsub( /'[\r]?/, "", name )
    print "<LI><OBJECT TYPE=\"text/sitemap\">"
    print "<PARAM NAME=\"Name\" VALUE=\"" name "\">"
    print "<PARAM NAME=\"Local\" VALUE=\"" fname() ".htm\">"
    print "</OBJECT>"
}

END {
    print "</UL></BODY></HTML>"
}
