#ifndef _AUTH_IDENTITY_DEFINED
#define _AUTH_IDENTITY_DEFINED

/* Authentication identity flags */
#define SEC_WINNT_AUTH_IDENTITY_ANSI    0x00000001L
#define SEC_WINNT_AUTH_IDENTITY_UNICODE 0x00000002L

/* Authentication identity */
typedef struct _SEC_WINNT_AUTH_IDENTITY_A {
    unsigned char   *User;
    unsigned long   UserLength;
    unsigned char   *Domain;
    unsigned long   DomainLength;
    unsigned char   *Password;
    unsigned long   PasswordLength;
    unsigned long   Flags;
} SEC_WINNT_AUTH_IDENTITY_A;
typedef SEC_WINNT_AUTH_IDENTITY_A   *PSEC_WINNT_AUTH_IDENTITY_A;
typedef struct _SEC_WINNT_AUTH_IDENTITY_W {
    unsigned short  *User;
    unsigned long   UserLength;
    unsigned short  *Domain;
    unsigned long   DomainLength;
    unsigned short  *Password;
    unsigned long   PasswordLength;
    unsigned long   Flags;
} SEC_WINNT_AUTH_IDENTITY_W;
typedef SEC_WINNT_AUTH_IDENTITY_W   *PSEC_WINNT_AUTH_IDENTITY_W;
#ifdef UNICODE
typedef SEC_WINNT_AUTH_IDENTITY_W   SEC_WINNT_AUTH_IDENTITY;
typedef PSEC_WINNT_AUTH_IDENTITY_W  PSEC_WINNT_AUTH_IDENTITY;
#else
typedef SEC_WINNT_AUTH_IDENTITY_A   SEC_WINNT_AUTH_IDENTITY;
typedef PSEC_WINNT_AUTH_IDENTITY_A  PSEC_WINNT_AUTH_IDENTITY;
#endif

#endif /* _AUTH_IDENTITY_DEFINED */
