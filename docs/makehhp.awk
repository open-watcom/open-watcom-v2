BEGIN {
    print "[OPTIONS]"
    print "Binary TOC=Yes"
    print "Compatibility=1.1 or later"
    if( length( hhcfile ) > 0 ) {
        print "Contents file=" hhcfile
    }
    print "Create CHI file=Yes"
    print "Default Window=Main"
    print "Full-text search=Yes"
    if( length( hhkfile ) > 0 ) {
        print "Index file=" hhkfile
    }
    print "Language=0x409 English (United States)"
}

/<TITLE> (.*) <\/TITLE>/ {
    title = $0
    gsub( /<TITLE> /, "", title )
    gsub( / <\/TITLE>/, "", title )
    print "Title=" title
    print ""
    print "[WINDOWS]"
    if( length( hhcfile ) > 0 ) {
        hhcfile = "\"" hhcfile "\""
    }
    if( length( hhkfile ) > 0 ) {
        hhkfile = "\"" hhkfile "\""
    }
    print "Main=\"" title "\"," hhcfile "," hhkfile ",,,,,,,0x2420,,0x60300e,,,,,,,,0"
    print ""
    print "[FILES]"
}

/<H. ID=\"(.*)\">/ {
    file = $0
    gsub( /<H. ID=\"/, "", file )
    gsub( /\"> (.*) <\/H.>/, "", file )
    if( file == "Index_of_Topics" ) {
        next
    }
    print file ".htm"
}
