"""Note: This only redoes the experiments with low k values
"""

import pprint
import argparse
from os import system
from typing import List
import yaml

PP = pprint.PrettyPrinter(indent=4)

# Parse the commandline for the top directory to store results of these tests in
parser = argparse.ArgumentParser(description="Run a sweep of the control parameters on the simulator.")
parser.add_argument("input_yaml", help="Path to yaml file that contains a list of incomplete result directories")
parser.add_argument("output_directory", help="Directory where results are saved.")
parser.add_argument("terrain", help="Terrain to use for experiments")
args = parser.parse_args()

def load_k_offset_from_yaml(yamlpath: str)->List[str]:
    with open(yamlpath, 'r') as stream:
        data_loaded = yaml.safe_load(stream)
    return data_loaded["incomplete_dirs"]

# Specify the directories with incomplete results
incomplete_dirs = load_k_offset_from_yaml(args.input_yaml)

# print(type(dirs))
# print(dirs)
# import sys
# sys.exit(0)

# Grab k and offset pairs that need to be run
# pairs is a list of (k, offset) tuples
pairs = []
for dir in incomplete_dirs:
    # print("----")
    # print(incomplete_dirs)
    folder_name = dir.split('/')[-1]
    # print(folder_name)
    folder_as_list = folder_name.split('_')
    # print(folder_as_list)
    k = folder_as_list[1]
    # print(k)
    offset = folder_as_list[-1]
    # print(offset)
    pairs.append((k,offset))

# print("pairs")
PP.pprint(pairs)
# import sys; sys.exit(0)
# Create a list of commands to sweep the variables. 3 experiments per each combination.
command_list = []
for pair in pairs:
    k = pair[0]
    offset = pair[1]
    command = "/home/egonzalez/ArmyAntSim/stable_build/ArmyAntSim"
    command += " -y " + args.terrain
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
    command += " --file_path " + args.output_directory + "k_" + str(k)[:4] + "_offset_" + str(offset)[:4] + "/"
    command_list.append(command)

PP.pprint(command_list)

# Execute the commands to run the sweep
for command in command_list:
    print("Running " + command)
    system(command)
