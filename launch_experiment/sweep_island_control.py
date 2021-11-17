from numpy import linspace, pi, hstack
from os import system
import argparse

# Parse the commandline for an optional top directory to store results of these tests in
parser = argparse.ArgumentParser(description="Run a sweep of the control parameters on the simulator.")
parser.add_argument("-d","--directory", help="Directory where results are saved.")
args = parser.parse_args()

# args.directory gets the directory

# Set up the variables we'll be sweeping
# num_ks = 10
k_start = 0.1
k_mid = 2.5
k_end = 20
k_vector = hstack((linspace(k_start, k_mid, 15), linspace(k_mid, k_end, 10)))
# islandLeft is 15.3, islandRight is 26.52
num_offsets = 20
offset_start = 0
offset_end = (26.52 - 15.3)/2
offset_vector = linspace(offset_start, offset_end, num_offsets)

print(k_vector)
print(offset_vector)

print("ks:")
for k in k_vector: print(str(k))
print("offsets:")
for o in offset_vector: print(str(o))

# Set up the root directory for this experiment
root_directory = ""
if args.directory:
    root_directory += args.directory
else:
    root_directory = "results/"
root_directory += "sweep_k_" + str(k_start)[:4] + "_to_" + str(k_end)[:4] + "_robot_speed_" + str(offset_start)[:4] + "_to_" + str(offset_end)[:4] + "/"

# Create a list of commands to sweep the variables. 3 experiments per each combination.
command_list = []
for k in k_vector:
    for offset in offset_vector:
        command = "/home/egonzalez/ArmyAntSim/build/ArmyAntSim"
        command += " -y flat"
        command += " --dynamic_speed 1"
        command += " --use_delay 0"
        command += " --smart_dissolution 1"
        command += " --robot_distance 2"
        command += " --infinite_robots 1"
        command += " --enable_visualization 0"
        command += " --p1 " + str(k)
        command += " --p2 0.5"
        command += " --p3 " + str(offset)
        command += " --torque 10"
        command += " --cp s_curve"
        command += " --file_path " + root_directory + "k_" + str(k)[:4] + "_offset_" + str(offset)[:4] + "/"
        command_list.append(command)

# Execute the commands to run the sweep
for command in command_list:
    system(command)
