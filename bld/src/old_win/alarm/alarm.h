typedef enum {                          /* for foreground or background pen */
    FOREGROUND,
    BACKGROUND,
    NUMBER_OF_PENS
} pen_type;

typedef enum {
    HOUR_TENS,
    HOUR_ONES,
    MIN_TENS,
    MIN_ONES,
    SEC_TENS,
    DIGITS_WITHOUT_SECONDS = SEC_TENS,
    SEC_ONES,
    DIGITS_WITH_SECONDS
} digit_index;

typedef enum {
    TOP_DOT,
    BOT_DOT,
    NUMBER_OF_DOTS
} dot_index;

typedef enum {
    SEGMENT_0,
    SEGMENT_1,
    SEGMENT_2,
    SEGMENT_3,
    SEGMENT_4,
    SEGMENT_5,
    SEGMENT_6,
    NUMBER_OF_SEGMENTS
} segment_index;

typedef enum {
    COLON_0,
    COLON_1,
    COLON_2,
    NUMBER_OF_COLONS
} colon_index;

#ifdef __WINDOWS_386__
#define _EXPORT
#else
#define _EXPORT __export
#endif

#define MINIMUM_HEIGHT                  18  /* minimum digit height in pixels */

#define PORTION_OF_SPACE_FOR_SEGMENT    5   /* size of segment (1/5 of digit) */
#define PORTION_FOR_SEGMENT_GAP         25  /* gap between segments (1/25) */
#define PORTION_FOR_COLON               4   /* size saved for ":" (1/4) */

#define UNUSED_PORTION_OF_SCREEN_HEIGHT 8   /* 1/8 of screen height unused */
#define UNUSED_PORTION_OF_SCREEN_WIDTH  16  /* 1/16 of screen width unused */

#define UNUSED_PORTION_OF_DIGIT_SPACE   6   /* 1/6 of digit space is unused */

#define NUMBER_OF_COLONS                3   /* there are 3 colons in the clock */

#define DIGIT_HEIGHT_SCALE              2   /* digit width is 2/3 its height */
#define DIGIT_WIDTH_SCALE               3

typedef unsigned pixels;

typedef struct {                        /* an (x,y) screen position */
        pixels          x;              /* -- top left is (0,0) */
        pixels          y;
} position;

typedef struct {                        /* start/end points of a line segment */
        position        start;
        position        end;
} seg_position;

typedef struct {                        /* info about one digit segment */
        BOOL            on;             /* is it painted on the screen? */
        seg_position    position;       /* where is it */
} segment;

typedef struct {                        /* info about one clock digit */
        int             value;          /* what number is it? */
        segment         segment[ NUMBER_OF_SEGMENTS ]; /* each segment */
} digit;

typedef struct {                        /* info about one "dot" of a colon */
        pen_type        pen;            /* is it on (FORE) or off (BACK) */
        position        top_left;       /* where is top left corner */
        position        bot_rite;       /* where is bottom right corner */
} colon_part;

typedef struct {                        /* info about one colon */
        colon_part      dot[NUMBER_OF_DOTS];
} colon;

#define MENU_ABOUT              1       /* define our menu items */
#define MENU_DISPLAY_SECONDS    2
#define MENU_SUPRESS_SECONDS    3
#define MENU_SET_ALARM          4
#define MENU_RUN_CLOCK          5
#define MENU_12_HOUR_CLOCK      6
#define MENU_24_HOUR_CLOCK      7

#define ONE_SECOND              1000    /* one second is 1000 milliseconds */

#define BUFLEN                  30      /* length of buffer reqd by MessageBox */

#define BLANK_SPACE_VALUE       10      /* this digit will draw as a blank */

#define TIMER_ID                1       /* id to use for our timer */
