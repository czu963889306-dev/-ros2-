# Default values for transitions as described in:
# http://design.ros2.org/articles/node_lifecycle.html

# Reserved [0-9], publicly available transitions.
# When a node is in one of these primary states, these transitions can be
# invoked.

# This transition will instantiate the node, but will not run any code beyond
# the constructor.
uint8 TRANSITION_CREATE = 0

# The node's onConfigure callback will be called to allow the node to load its
# configuration and conduct any required setup.
uint8 TRANSITION_CONFIGURE = 1

# The node's callback onCleanup will be called in this transition to allow the
# node to load its configuration and conduct any required setup.
uint8 TRANSITION_CLEANUP = 2

# The node's callback onActivate will be executed to do any final preparations
# to start executing.
uint8 TRANSITION_ACTIVATE = 3

# The node's callback onDeactivate will be executed to do any cleanup to start
# executing, and reverse the onActivate changes.
uint8 TRANSITION_DEACTIVATE = 4

# This signals shutdown during an unconfigured state, the node's callback
# onShutdown will be executed to do any cleanup necessary before destruction.
uint8 TRANSITION_UNCONFIGURED_SHUTDOWN  = 5

# This signals shutdown during an inactive state, the node's callback onShutdown
# will be executed to do any cleanup necessary before destruction.
uint8 TRANSITION_INACTIVE_SHUTDOWN = 6

# This signals shutdown during an active state, the node's callback onShutdown
# will be executed to do any cleanup necessary before destruction.
uint8 TRANSITION_ACTIVE_SHUTDOWN = 7

# This transition will simply cause the deallocation of the node.
uint8 TRANSITION_DESTROY = 8

# Reserved [10-69], private transitions
# These transitions are not publicly available and cannot be invoked by a user.
# The following transitions are implicitly invoked based on the callback
# feedback of the intermediate transition states.
uint8 TRANSITION_ON_CONFIGURE_SUCCESS = 10
uint8 TRANSITION_ON_CONFIGURE_FAILURE = 11
uint8 TRANSITION_ON_CONFIGURE_ERROR = 12

uint8 TRANSITION_ON_CLEANUP_SUCCESS = 20
uint8 TRANSITION_ON_CLEANUP_FAILURE = 21
uint8 TRANSITION_ON_CLEANUP_ERROR = 22

uint8 TRANSITION_ON_ACTIVATE_SUCCESS = 30
uint8 TRANSITION_ON_ACTIVATE_FAILURE = 31
uint8 TRANSITION_ON_ACTIVATE_ERROR = 32

uint8 TRANSITION_ON_DEACTIVATE_SUCCESS = 40
uint8 TRANSITION_ON_DEACTIVATE_FAILURE = 41
uint8 TRANSITION_ON_DEACTIVATE_ERROR = 42

uint8 TRANSITION_ON_SHUTDOWN_SUCCESS = 50
uint8 TRANSITION_ON_SHUTDOWN_FAILURE = 51
uint8 TRANSITION_ON_SHUTDOWN_ERROR = 52

uint8 TRANSITION_ON_ERROR_SUCCESS = 60
uint8 TRANSITION_ON_ERROR_FAILURE = 61
uint8 TRANSITION_ON_ERROR_ERROR = 62

# Reserved [90-99]. Transition callback success values.
# These return values ought to be set as a return value for each callback.
# Depending on which return value, the transition will be executed correctly or
# fallback/error callbacks will be triggered.

# The transition callback successfully performed its required functionality.
uint8 TRANSITION_CALLBACK_SUCCESS = 97

# The transition callback failed to perform its required functionality.
uint8 TRANSITION_CALLBACK_FAILURE = 98

# The transition callback encountered an error that requires special cleanup, if
# possible.
uint8 TRANSITION_CALLBACK_ERROR = 99

##
## Fields
##

# The transition id from above definitions.
uint8 id

# A text label of the transition.
string label
