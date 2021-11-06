#include "tooltips.msg"

pick( VI_BROWSE_MSG,
      "Please select a folder.",
      "Please select a folder." )

#define pick(enum,enumrc,name,namej,fname,desc,filter) pick( RC_VI_LANG_ ## enumrc, name, namej )
#include "langdef.h"
#undef pick
