include watcom.mk

SAMPLE = sample

SRCDIRS = $(GUI_DIR)/sample/c

INCDIRS = $(GUI_DIR)/h
INCDIRS += $(GUI_DIR)/sample/h
INCDIRS += $(GUI_DIR)/ui/h
INCDIRS += $(H_DIR)
INCDIRS += $(UTILS_DIR)
INCDIRS += $(WATCOMH)

objects = guimem.o dlgstat.o dlgdynam.o dlgtest.o sampdef.o sample.o

LIBDIRS += $(GUI_DIR)/$(OBJDIR)
LIBDIRS += $(UI_DIR)/$(OBJDIR)
LIBDIRS += $(UTILS_DIR)/$(OBJDIR)
LIBDIRS += $(WCLIB_DIR)/$(OBJDIR)
LIBDIRS += $(WRES_DIR)/$(OBJDIR)
LIBDIRS += /usr/lib

LIBS += -lgui
LIBS += -lutils
LIBS += -lui
LIBS += -lwres
LIBS += -lwatcom
LIBS += -lcurses

vpath %.c $(SRCDIRS)
vpath %.h $(INCDIRS)


all	: $(SAMPLE)

sample	: $(objects) $(GUI_DIR)/$(OBJDIR) $(UI_DIR)/$(OBJDIR)
	$(CC) -o $@ $(CFLAGS) $(objects) $(LIBS)

sample.o	: sample.c sample.h dlgstat.h dlgdynam.h

sampdef.o	: sampdef.c sample.h 

dlgtest.o	: dlgtest.c dlgtest.h dlgstat.h sample.h

dlgdynam.o	: dlgdynam.c dlgdynam.h dlgstat.h sample.h

dlgstat.o	: dlgstat.c dlgstat.h sample.h
