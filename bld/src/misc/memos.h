/* This structure is for an individual line in a memo.
 */
typedef struct text_line {
    struct text_line *  next;
    char                text[1];
} TEXT_LINE;

/* This structure is the head of an individual memo.
 */
typedef struct memo_el {
    struct memo_el *    prev;
    struct memo_el *    next;
    TEXT_LINE *         text;
    char                date[9];
} MEMO_EL;
