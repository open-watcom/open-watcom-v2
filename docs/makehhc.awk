BEGIN {
    print "<HTML><BODY>"
    print "<OBJECT TYPE=\"text/site properties\">"
    print "<PARAM NAME=\"ImageType\" VALUE=\"Folder\">"
    print "</OBJECT>"
    print "<UL>"
    level = 1
}

/<H. ID=\"(.*)\"> (.*) <\/H.>/ {
    newlevel = $0
    gsub( /<H/, "", newlevel )
    gsub( / ID=\"(.*)\" (.*) <\/H.>/, "", newlevel )
    file = $0
    gsub( /<H. ID=\"/, "", file )
    gsub( /\"> (.*) <\/H.>/, "", file )
    if( file == "Index_of_Topics" ) {
        next
    }
    title = $0
    gsub( /<H. ID=\"(.*)\"> /, "", title )
    gsub( / <\/H.>/, "", title )
    gsub( /\&nbsp\;/, "", title )
    newlevel = newlevel + 0
    if( newlevel < level ) {
        while( newlevel < level ) {
            print "</UL>"
            level--
        }
    } else if( newlevel > level ) {
        print "<UL>"
        level = newlevel
    }
    print "<LI><OBJECT TYPE=\"text/sitemap\">"
    print "<PARAM NAME=\"Name\" VALUE=\"" title "\">"
    print "<PARAM NAME=\"Local\" VALUE=\"" file ".htm\">"
    print "</OBJECT>"
}

END {
    while( level > 0 ) {
        print "</UL>"
        level--
    }
    print "</BODY></HTML>"
}
