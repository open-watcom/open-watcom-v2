#!/bin/sh

export OWDOSBOX=dosbox

. $OWROOT/cmnvars.sh

$OWCOVERITY_TOOL_CMD --dir cov-int $OWCOVERITY_SCRIPT
