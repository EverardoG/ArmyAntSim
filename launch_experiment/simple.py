from os import system
from time import time

run_id = "".join(str(time()).split("."))

command = "/home/egonzalez/ArmyAntSim/build/ArmyAntSim"
command += " -y island"
command += " --dynamic_speed 1"
command += " --use_delay 0"
command += " --smart_dissolution 1"
command += " --robot_distance 2"
command += " --infinite_robots 1"
command += " --enable_visualization 0"
command += " --p1 0.1"
command += " --p2 0.5"
command += " --p3 0.88"
command += " --torque 10"
command += " --file_path /home/egonzalez/data/temp/results_" + run_id + "/"

system(command)
