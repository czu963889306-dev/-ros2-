# copied from ament_package/template/prefix_level/setup.bash

AMENT_SHELL=bash

# source setup.sh from same directory as this file
AMENT_CURRENT_PREFIX=$(builtin cd "`dirname "${BASH_SOURCE[0]}"`" && pwd)
# trace output
if [ -n "$AMENT_TRACE_SETUP_FILES" ]; then
  echo "# . \"$AMENT_CURRENT_PREFIX/setup.sh\""
fi
. "$AMENT_CURRENT_PREFIX/setup.sh"
