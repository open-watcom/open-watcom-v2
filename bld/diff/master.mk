name = diff

vpath %.c $(DIFF_DIR)

INCDIRS += $(DIFF_DIR)
LIBDIRS += $(WCLIB_DIR)/$(OBJDIR)
LIBS += -lwatcom

%.o : %.c

$(name) : $(name).c
	$(CC) $(CFLAGS) $< -o $@ $(LIBS)

