# Primary state definitions as depicted in:
# http://design.ros2.org/articles/node_lifecycle.html

# These are the primary states. State changes can only be requested when the
# node is in one of these states.

# Indicates state has not yet been set.
uint8 PRIMARY_STATE_UNKNOWN = 0

# This is the life cycle state the node is in immediately after being
# instantiated.
uint8 PRIMARY_STATE_UNCONFIGURED = 1

# This state represents a node that is not currently performing any processing.
uint8 PRIMARY_STATE_INACTIVE = 2

# This is the main state of the node's life cycle. While in this state, the node
# performs any processing, responds to service requests, reads and processes
# data, produces output, etc.
uint8 PRIMARY_STATE_ACTIVE = 3

# The finalized state is the state in which the node ends immediately before
# being destroyed.
uint8 PRIMARY_STATE_FINALIZED = 4

# Temporary intermediate states. When a transition is requested, the node
# changes its state into one of these states.

# In this transition state the node's onConfigure callback will be called to
# allow the node to load its configuration and conduct any required setup.
uint8 TRANSITION_STATE_CONFIGURING = 10

# In this transition state the node's callback onCleanup will be called to clear
# all state and return the node to a functionally equivalent state as when
# first created.
uint8 TRANSITION_STATE_CLEANINGUP = 11

# In this transition state the callback onShutdown will be executed to do any
# cleanup necessary before destruction.
uint8 TRANSITION_STATE_SHUTTINGDOWN = 12

# In this transition state the callback onActivate will be executed to do any
# final preparations to start executing.
uint8 TRANSITION_STATE_ACTIVATING = 13

# In this transition state the callback onDeactivate will be executed to do any
# cleanup to start executing, and reverse the onActivate changes.
uint8 TRANSITION_STATE_DEACTIVATING = 14

# This transition state is where any error may be cleaned up.
uint8 TRANSITION_STATE_ERRORPROCESSING = 15

# The state id value from the above definitions.
uint8 id

# A text label of the state.
string label
