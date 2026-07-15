# copied from ament_package/template/isolated_prefix_level/local_setup.zsh

AMENT_SHELL=zsh

# source local_setup.sh from same directory as this file
AMENT_CURRENT_PREFIX=$(builtin cd -q "`dirname "${(%):-%N}"`" > /dev/null && pwd)

# function to convert array-like strings into arrays
# to wordaround SH_WORD_SPLIT not being set
ament_zsh_to_array() {
  local _listname=$1
  local _dollar="$"
  local _split="{="
  local _to_array="(\"$_dollar$_split$_listname}\")"
  eval $_listname=$_to_array
}

# trace output
if [ -n "$AMENT_TRACE_SETUP_FILES" ]; then
  echo "# . \"$AMENT_CURRENT_PREFIX/local_setup.sh\""
fi
. "$AMENT_CURRENT_PREFIX/local_setup.sh"
