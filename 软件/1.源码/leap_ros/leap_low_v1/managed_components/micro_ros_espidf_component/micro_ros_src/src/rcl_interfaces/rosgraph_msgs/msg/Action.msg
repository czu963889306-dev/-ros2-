# Describes a single Action endpoint, which may be a Server or Client

# Fully qualified name of the Action
string name

# An action is actually a composition of the following fundamental ROS entities
Service send_goal
Service get_result
Service cancel_goal
Topic feedback
Topic status
