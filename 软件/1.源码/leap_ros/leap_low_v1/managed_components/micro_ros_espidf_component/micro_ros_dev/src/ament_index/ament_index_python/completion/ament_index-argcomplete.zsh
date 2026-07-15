# Copyright 2019 Open Source Robotics Foundation, Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

autoload -U +X compinit && compinit
autoload -U +X bashcompinit && bashcompinit

# Get this scripts directory
__ament_index_completion_dir=${0:a:h}
# Just source the bash version, it works in zsh too
source "$__ament_index_completion_dir/ament_index-argcomplete.bash"
# Cleanup
unset __ament_index_completion_dir
