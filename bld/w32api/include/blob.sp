/* Blob */
#ifndef _BLOB_DEFINED
#define _BLOB_DEFINED
typedef struct tagBLOB {
    ULONG   cbSize;
    BYTE    *pBlobData;
} BLOB;
typedef BLOB    *LPBLOB;
#endif /* _BLOB_DEFINED */
