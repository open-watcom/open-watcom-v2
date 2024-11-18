/* NT thread information block */
typedef struct _NT_TIB {
    struct _EXCEPTION_REGISTRATION_RECORD   *ExceptionList;
    PVOID           StackBase;
    PVOID           StackLimit;
    PVOID           SubSystemTib;
    union {
        PVOID   FiberData;
        ULONG   Version;
    };
    PVOID           ArbitraryUserPointer;
    struct _NT_TIB  *Self;
} NT_TIB;
typedef NT_TIB  *PNT_TIB;

/* NT thread information block (32-bit version) */
typedef struct _NT_TIB32 {
    ULONG   ExceptionList;
    ULONG   StackBase;
    ULONG   StackLimit;
    ULONG   SubSystemTib;
    union {
        ULONG   FiberData;
        ULONG   Version;
    };
    ULONG   ArbitraryUserPointer;
    ULONG   Self;
} NT_TIB32;
typedef NT_TIB32    *PNT_TIB32;

/* NT thread information block (64-bit version) */
typedef struct _NT_TIB64 {
    ULONG64 ExceptionList;
    ULONG64 StackBase;
    ULONG64 StackLimit;
    ULONG64 SubSystemTib;
    union {
        ULONG64 FiberData;
        ULONG   Version;
    };
    ULONG64 ArbitraryUserPointer;
    ULONG64 Self;
} NT_TIB64;
typedef NT_TIB64    *PNT_TIB64;
