{
    line = $0;
    gsub( /\$\$BLD_VER\$\$/, bld_ver, line );
    print line;
}
