#define MEM_DECOMMIT         0x4000  // Comment 1
#define MEM_RELEASE          0x8000     // Comment 2
#define MEM_FREE            0x10000     /* Comment 3 */ /* Comment 4
#define MEM_MAPPED_COPIED   0x80000     
#define MEM_TOP_DOWN       0x100000     */
#define MEM_LARGE_PAGES  0x20000000     //
#define MEM_DOS_LIM      0x40000000     


#define FILE_WRITE_ATTRIBUTES     ( 0x0100 )

#define FILE_ALL_ACCESS (STANDARD_RIGHTS_REQUIRED | SYNCHRONIZE | 0x1FF)


#define FILE_GENERIC_READ         (STANDARD_RIGHTS_READ     |/* Comment 5*/\
                                   FILE_READ_DATA           |\
                                   FILE_READ_ATTRIBUTES     |\
                                   FILE_READ_EA             |\
                                   SYNCHRONIZE)


#define FILE_GENERIC_WRITE        (STANDARD_RIGHTS_WRITE    |\
                                   FILE_WRITE_DATA          |\
                                   FILE_WRITE_ATTRIBUTES    |\
                                   FILE_WRITE_EA            |\
                                   FILE_APPEND_DATA         |\
                                   SYNCHRONIZE)


#define FILE_GENERIC_EXECUTE      (STANDARD_RIGHTS_EXECUTE  |\
                                   FILE_READ_ATTRIBUTES     |\
                                   FILE_EXECUTE             |\
                                   SYNCHRONIZE)

#define DELETE                           (0x00010000L)
#define READ_CONTROL                     (0x00020000L)
#define STANDARD_RIGHTS_READ             (READ_CONTROL)
//
