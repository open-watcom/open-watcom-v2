typedef struct copy_entry {
    struct copy_entry   *next;
    char                src[_MAX_PATH];
    char                dst[_MAX_PATH];
} copy_entry;

typedef struct include {
    struct include      *prev;
    FILE                *fp;
    unsigned            skipping;
    unsigned            ifdefskipping;
    unsigned            lineno;
    char                name[_MAX_PATH];
    char                cwd[_MAX_PATH];
    copy_entry          *reset_abit;
} include;

extern include          *IncludeStk;
extern void             ResetArchives( copy_entry * );
