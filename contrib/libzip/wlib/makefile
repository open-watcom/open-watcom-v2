CC = wcc386
CFLAGS = -zq -wx -oaxt -I"../../zlib"

.c : ../lib

.c.obj :
    $(CC) $(CFLAGS) $<

OBJS = &
    zip_add.obj &
    zip_close.obj &
    zip_delete.obj &
    zip_dirent.obj &
    zip_entry_free.obj &
    zip_entry_new.obj &
    zip_err_str.obj &
    zip_error.obj &
    zip_error_get.obj &
    zip_error_get_sys_type.obj &
    zip_error_strerror.obj &
    zip_error_to_str.obj &
    zip_fclose.obj &
    zip_file_error_get.obj &
    zip_file_get_offset.obj &
    zip_file_strerror.obj &
    zip_fopen.obj &
    zip_fopen_index.obj &
    zip_fread.obj &
    zip_free.obj &
    zip_get_num_files.obj &
    zip_get_name.obj &
    zip_name_locate.obj &
    zip_new.obj &
    zip_open.obj &
    zip_rename.obj &
    zip_replace.obj &
    zip_source_buffer.obj &
    zip_source_file.obj &
    zip_source_filep.obj &
    zip_source_free.obj &
    zip_source_function.obj &
    zip_source_zip.obj &
    zip_set_name.obj &
    zip_stat.obj &
    zip_stat_index.obj &
    zip_strerror.obj &
    zip_unchange.obj &
    zip_unchange_all.obj &
    zip_unchange_data.obj

libzip.lib : $(OBJS)
    wlib -q $@ $<

clean : .symbolic
    rm -f *.obj
    rm -f *.lib
