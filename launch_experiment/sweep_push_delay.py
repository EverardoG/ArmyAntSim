from numpy import linspace
from os import system
from datetime import datetime

# Set up the result folder
now = str(datetime.now())
now = now.replace("-", "_")
now = now.replace(" ","_")
experiment_name = "push_sweep"
result_path = "./experiments/" + now  + "_" + experiment_name + "/"
print(result_path)

# Run the simulation for different values of push_delays
push_delays = linspace(0,4,5)
for push_delay in push_delays:
    system("./build/ArmyAntSim -y cliff" + " --push_delay " + str(push_delay) + " --file_path " + result_path + " --fixed_speed 0")