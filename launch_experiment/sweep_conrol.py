from numpy import linspace, pi
from os import system
# from datetime import datetime
import argparse

# Parse the commandline for an optional top directory to store results of these tests in
parser = argparse.ArgumentParser(description="Run a sweep of the control parameters on the simulator.")
parser.add_argument("-d","--directory", help="Directory where results are saved.")
args = parser.parse_args()

# args.directory gets the directory

# Set up the variables we'll be sweeping
gain_start = 0.05
gain_end = 0.5
gain_increment = 0.05
num_gains = round((gain_end-gain_start)/gain_increment + 1)
gain_vector = linspace(gain_start, gain_end, num_gains)

offset_start = 1.0/4.0 * pi
offset_end = 4.0 * pi
offset_increment = 1.0/4.0 * pi
num_offsets = round((offset_end-offset_start)/offset_increment + 1)
offset_vector = linspace(offset_start, offset_end, num_offsets)

# Set up the root directory for this experiment
root_directory = ""
if args.directory:
    root_directory += args.directory

# Sweep the variables. 3 experiments per each combination.
for gain in gain_vector:
    for offset in offset_vector:
        for i in range(3):
            command = "./build/ArmyAntSim -y cliff --dynamic_speed 1"
            command += " --robot_speed " + str(offset)[:3]
            command += " --kp " + str(gain)[:3]
            command += " --folder_path " + root_directory + "sweep_kp_" + str(gain_start)[:3] + "_to_" + str(gain_end)[:3] + "_robot_speed_" + str(offset_start)[:3] + "_to_" + str(offset_end)[:3]
            command += " --file_path kp_" + str(gain)[:3] + "_robot_speed_" + str(offset)[:3]
            print(command)
            # system(command)



# # Set up the result folder
# now = str(datetime.now())
# now = now.replace("-", "_")
# now = now.replace(" ","_")
# experiment_name = "push_sweep"
# result_path = "./experiments/" + now  + "_" + experiment_name + "/"
# print(result_path)

# # Run the simulation for different values of push_delays
# push_delays = linspace(0,4,5)
# for push_delay in push_delays:
#     system("./build/ArmyAntSim -y cliff" + " --push_delay " + str(push_delay) + " --file_path " + result_path + " --fixed_speed 0")