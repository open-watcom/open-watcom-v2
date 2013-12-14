BEGIN {
    print "[OPTIONS]"
    print "Binary TOC=Yes"
    print "Compatibility=1.1 or later"
    print "Contents file=" hbook ".hhc"
    print "Create CHI file=Yes"
    print "Default Window=Main"
    print "Full-text search=Yes"
    print "Index file=" hbook ".hhk"
    print "Language=0x409 English (United States)"
}

/<TITLE> (.*) <\/TITLE>/ {
    title = $0
    gsub( /<TITLE> /, "", title )
    gsub( / <\/TITLE>/, "", title )
    print "Title=" title
    print ""
    print "[WINDOWS]"
    print "Main=\"" title "\",\"" hbook ".hhc\",\"" hbook ".hhk\",,,,,,,0x2420,,0x60300e,,,,,,,,0"
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

END {
    print ""
    print "[ALIAS]"
    print "#include " hbook ".hha"
    print ""
    print "[MAP]"
    print "#include " hbook ".h"
}
