from numpy import linspace
from os import system
from datetime import datetime

# Run the simulation for different values of push_delays
push_delays = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10]
for push_delay in push_delays:
    # Set up the result folder
    # now = str(datetime.now())
    # now = now.replace("-", "_")
    # now = now.replace(" ","_")
    experiment_name = str(push_delay) + "_push_delay"
    result_path = "experiments/push_sweep"+str(push_delay)+"/"
    print("Saving results to " + result_path)

    system("./build/ArmyAntSim -y cliff" + " --push_delay " + str(push_delay) + " --file_path " + result_path + " --file_name " + experiment_name + " --dynamic_speed 1 --infinite_robots 1 --smart_dissolution 1 --enable_visualization 0")