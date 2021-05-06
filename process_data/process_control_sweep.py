from matplotlib import pyplot as plt
import numpy as np
import os
import argparse
from mpl_toolkits.mplot3d import Axes3D
from matplotlib import cm

def extract_dissolution_time( result_filepath ):
    """
    For an input result_filepath to a _result.txt file,
    returns the dissolution time in seconds
    """
    f = open(result_filepath, "r")
    for line in f:
        if "Bridge formation step duration:" in line:
            dissolution_time = line.split(":")[-1].split(" ")[-2]
            return dissolution_time

    raise Exception("No dissolution time found")


# Parse the commandline for the top directory to crawl through for the results
parser = argparse.ArgumentParser(description="Crawl through results of a controller sweep and plot the results.")
parser.add_argument("-d", "--directory", help="Top level directory to search for results in.", required=True)
args = parser.parse_args()

# args.directory gets the directory

# get the file paths for all the _results.txt files
result_files = []
for root, dirs, files in os.walk(args.directory):
    if "_result.txt" in files:
        result_files.append(root + "/_result.txt")

# Create a dictionary of all result files and corresponding kp, speed, num_trail, time to dissolution (AKA time to ten robots)
results_dict = {}
kp_list = []
speed_list = []
for result_filepath in result_files:
    split_path = result_filepath.split("/")

    # Get the experiment variables
    experiment_info = split_path[-3].split("_")

    # Get the kp, speed, and experiment number
    kp = experiment_info[-4]
    speed = experiment_info[-1]
    num_trial = split_path[-2]
    dissolution_time = extract_dissolution_time(result_filepath)

    # Create a new entry
    if kp not in results_dict:
        results_dict[kp] = {}
    if speed not in results_dict[kp]:
        results_dict[kp][speed] = {}
    results_dict[kp][speed][num_trial] = dissolution_time

    # Add to the lists
    if kp not in kp_list:
        kp_list.append(kp)
    if speed not in speed_list:
        speed_list.append(speed)

def compare_num_str(item1, item2):
    if float(item1) > float(item2):
        return 1
    else:
        return -1

kp_list.sort(key=float)
speed_list.sort(key=float)

print(kp_list)
print(speed_list)

# Create a meshgrid of kp, speed, and dissolution_time
dissolution_time_grid = np.zeros((len(kp_list), len(speed_list)))
print("time grid size: ", dissolution_time_grid.shape)
for count_kp, kp in enumerate(kp_list):
    for count_speed, speed in enumerate(speed_list):
        dissolution_time_grid[count_kp, count_speed] = float(results_dict[kp][speed]["0"])
        #print("kp: ", kp, " | speed: ", speed, " | dissolution_time: ", dissolution_time_grid[count_kp, count_speed], "count_kp: ", count_kp, "count_speed: ", count_speed)
speed_list = [float(speed) for speed in speed_list]
kp_list = [float(kp) for kp in kp_list]
speed_grid, kp_grid = np.meshgrid(speed_list, kp_list)
print("kp grid size: ", kp_grid.shape)
print("speed grid size: ", speed_grid.shape)

# Plot the surface
fig, ax = plt.subplots(subplot_kw={"projection": "3d"})
surf = ax.plot_surface(kp_grid, speed_grid, dissolution_time_grid, cmap=cm.coolwarm, linewidth=0, antialiased=False)

# Add a color bar
fig.colorbar(surf, shrink=0.5, aspect=5)

plt.show()
