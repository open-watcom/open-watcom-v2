# makefile for nmake

all: asetup adventure ADVTXT

asetup: asetup.o asubs.o
	f77 -o asetup asetup.o asubs.o
adventure: aamain.o asubs.o
	f77 -o adventure aamain.o asubs.o
ADVTXT: ADVDAT asetup
	asetup
