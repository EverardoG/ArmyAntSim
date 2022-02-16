from os import system
from time import time

run_id = "".join(str(time()).split("."))

command = "/home/egonzalez/ArmyAntSim/build/ArmyAntSim"
command += " -y pit"             # Use the island terrain
command += " --dynamic_speed 1"     # Use a dynamic speed rather than fixed speed
command += " --use_delay 0"         # Do not use a time delay to spawn robots. Instead wait a set distance, then spawn the next robot
command += " --smart_dissolution 1" # Dissolve the structure once a robot reaches the goal
command += " --robot_distance 5"    # Distance between robots spawning
# command += " --infinite_robots 1"   # Whether to use an infinite number of robots: 1-yes, 0-no
command += " --number_robots 1"        # Spawn X robots
command += " --enable_visualization 1"  # Turn on visualizer
command += " --p1 5"               # Controller param 1
# command += " --p2 0.5"              # Controller param 2
# command += " --p3 2.3621052631578947"   # Controller param 3
command += " --torque 10"           # Max torque each flippybot has
command += " --sg 1.0" # Add noise to goal position estimate
command += " --cp dynamic_reactivebuild_no_comms"          # Control policy
command += " --file_path /home/egonzalez/data/temp/results_" + run_id + "/" # Where to save results

system(command)
