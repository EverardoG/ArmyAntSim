from sys import exit
from os import system
import argparse

# Parse the commandline for an optional top directory to store results of these tests in
parser = argparse.ArgumentParser(description="Sweep sigma for IROS results.")
parser.add_argument("-d","--directory", help="Directory where results are saved.")
args = parser.parse_args()

# Set up the variables we'll be sweeping

# sigmas = [0.0, 0.5, 1.0, 1.5, 2.0, 2.5, 3.0, 3.5, 4.0, 4.5, 5.0, 5.5, 6.0, 6.5, 7.0, 7.5, 8.0, 8.5, 9.0, 9.5, 10.0]
sigmas = list(range(11))
terrains = ["pit", "island", "terrace", "step_down"]
num_trials = 20

# Set up the root directory for this experiment
root_directory = ""
if args.directory:
    root_directory += args.directory
else:
    root_directory = "results/"
root_directory += "sweep_iros_terrains_sigma_"+str(sigmas[0])+"_to_"+str(sigmas[-1])+"/"

# Create a list of commands to sweep the variables.
command_list = []

random_seed = 0
for sigma in sigmas:
    for terrain in terrains:
        for num_trial in range(num_trials):
            command = "/home/egonzalez/ArmyAntSim/build/ArmyAntSim" # Built simulator to run
            command += " -y " + terrain             # Which terrain to use
            command += " --dynamic_speed 1"         # Turn dynamic flipping speed on
            command += " --use_delay 1"             # Use time delay between spawning robots
            command += " --smart_dissolution 1"     # Use goal-based dissolution rules
            command += " --robot_delay 8"           # seconds between spawning robots
            command += " --gaussian_delay 0"        # Don't use gaussian delay for robot spawn delay
            command += " --infinite_robots 1"       # Use however many robots are necessary for bridge formation
            command += " --limit_angle 0.1"         # Set limit angle (radians) before robot is allowed to grab
            command += " --enable_visualization 0"  # Turn off visualizer
            command += " --p1 20"                   # Gain for robot control
            command += " --rs " + str(random_seed)       # Random seed for random goal perturbation
            command += " --torque 10"               # torque for flipping
            command += " --sg " + str(sigma)             # sigma value for noisy goal estimates
            command += " --bridge_delay 1.0"        # Delay in bridge state before going back to walking state
            command += " --cp slowdown_away_from_goal"  # Control policy to use
            command += " --max_dissolution_time " + str(60*20) # Give structures a maximum of 20 minutes to dissolve
            command += " --file_path " + root_directory + terrain + "_" + str(sigma) + "/" + str(num_trial) + "/"  # Where to save results
            command_list.append(command)
            random_seed += 1                        # Increment random seed counter

# Execute the commands to run the sweep
for command in command_list:
    if (system(command) == 2):
        exit()