noinst_PROGRAMS=buffadd open name_locate fread
noinst_HEADERS=mkname.h
EXTRA_PROGRAMS=ziptest deltest

EXTRA_DIST=broken.zip test.zip

buffadd_SOURCES=	buffadd.c
fread_SOURCES=		fread.c mkname.c
name_locate_SOURCES=	name_locate.c mkname.c
open_SOURCES=		open.c mkname.c

deltest_SOURCES=	deltest.c
ziptest_SOURCES=	ziptest.c

TESTS=buffadd open name_locate fread
TESTS_ENVIRONMENT=	SRCDIR=${srcdir}

AM_CPPFLAGS=-I${top_srcdir}/lib
LDADD=../lib/libzip.la
