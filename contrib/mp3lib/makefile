CC = wcl386

#
# Flags:
#
# DEBUG_CHECK     Extra checking of bitstream data
# OUTPUT_SOUND    Write sound data to /dev/dsp
# OUTPUT_RAW      Write sound data to <filename>.raw
# OUTPUT_DBG      Write clear-text debug dumps to stdout

CFLAGS = -zq -wx -e15 -onaxt -DOUTPUT_WAV -6r -d2

MP3_OBJS = bitstrm.obj decoder.obj huffman.obj hufftab.obj &
	maindec.obj synthtab.obj 

.c.obj : .autodepend
        $(CC) -c $(CFLAGS) $<

all: mp3decode.exe mp3play.exe #mp3plw.exe

mp3decode.exe: $(MP3_OBJS) main.obj outfile.obj
	$(CC) $(CFLAGS) -fe=mp3decode $<

mp3play.exe: maindart.c $(MP3_OBJS)
	$(CC) $(CFLAGS) -bt=os2 -I"$(%WATCOM)/h/os2" -fe=mp3play $< mmpm2.lib

mp3plw.exe: mainds.c $(MP3_OBJS)
	$(CC) $(CFLAGS) -bt=nt -I"$(%WATCOM)/h/nt" -fe=mp3plw $<

clean: .symbolic
	-rm -f *.obj TAGS *.wav *.bin

realclean: clean
	-rm -f mp3decode mp3dec_opt *.pdf *.ps bcmp mcmp *.bit

ctags: .symbolic
	ctags -qx *.c *.h
