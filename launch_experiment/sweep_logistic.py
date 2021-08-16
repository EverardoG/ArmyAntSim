from numpy import linspace, pi
from os import system
import argparse

# Parse the commandline for an optional top directory to store results of these tests in
parser = argparse.ArgumentParser(description="Run a sweep of the control parameters on the simulator.")
parser.add_argument("-d","--directory", help="Directory where results are saved.")
args = parser.parse_args()

# args.directory gets the directory

# Set up the variables we'll be sweeping
gain_start = -1
gain_end = -5
gain_increment = -0.5
num_gains = round((gain_end-gain_start)/gain_increment + 1)
gain_vector = linspace(gain_start, gain_end, num_gains)
print(gain_vector)

# Set up the root directory for this experiment
root_directory = ""
if args.directory:
    root_directory += args.directory
else:
    root_directory = "results/"
root_directory += "sweep_kp_" + str(gain_start)[:4] + "_to_" + str(gain_end)[:4] + "_robot_speed_" + str(offset_start)[:4] + "_to_" + str(offset_end)[:4] + "/"

# Create a list of commands to sweep the variables. 3 experiments per each combination.
command_list = []
for gain in gain_vector:
    for offset in offset_vector:
        for i in range(3):
            command = "./build/ArmyAntSim"
            command += " -y cliff"
            command += " --dynamic_speed 1"
            command += " --use_delay 0"
            command += " --smart_dissolution 1"
            command += " --robot_distance 2"
            command += " --infinite_robots 1"
            command += " --enable_visualization 0"
            command += " --robot_speed " + str(offset)
            command += " --kp " + str(gain)
            command += " --file_path " + root_directory + "kp_" + str(gain)[:4] + "_robot_speed_" + str(offset)[:4] + "/" + str(i) + "/"
            command_list.append(command)

# Execute the commands to run the sweep
for command in command_list:
    system(command)
