NR == FNR {
    ref[$0] = 1
    next
}

/__FD/ {
    cur[$0] = 1
}

END {
    err = 0

    for (s in ref) {
        if (!(s in cur)) {
            print "missing:", s
            err = 1
        }
    }

    for (s in cur) {
        if (!(s in ref)) {
            print "unexpected:", s
            err = 1
        }
    }

    exit err
}
