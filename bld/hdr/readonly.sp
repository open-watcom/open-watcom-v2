:: system header files are not included in .autodepend records
:segment !QNX
#if !defined(_ENABLE_AUTODEPEND)
  #pragma read_only_file;
#endif
:endsegment
