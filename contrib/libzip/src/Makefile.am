AM_CPPFLAGS = -include ${top_srcdir}/config.h
bin_PROGRAMS=zipcmp zipmerge ziptorrent

zipcmp_CPPFLAGS=-I${top_srcdir}/lib
zipcmp_LDADD=${top_builddir}/lib/libzip.la
zipmerge_CPPFLAGS=-I${top_srcdir}/lib
zipmerge_LDADD=${top_builddir}/lib/libzip.la
ziptorrent_CPPFLAGS=-I${top_srcdir}/lib
ziptorrent_LDADD=${top_builddir}/lib/libzip.la

EXTRA_DIST=	CMakeLists.txt getopt.c getopt.h
