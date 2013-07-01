#include "tooltips.msg"

pick( VI_BROWSE_MSG,
      "Please select a folder.",
      "Please select a folder." )

#define pick_lang(enum,enumrc,name,namej,fname) pick( enumrc, name, namej )
#include "langdef.h"
#undef pick_lang
