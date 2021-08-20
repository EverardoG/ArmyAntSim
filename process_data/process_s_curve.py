import argparse
import os
from utils import get_metrics_from_folder
from pathlib import Path
import numpy as np
import matplotlib.pyplot as plt
from matplotlib import cm

# Parse the commandline for an optional top directory to store results of these tests in
parser = argparse.ArgumentParser(description="Process the sweep from the simulator.")
parser.add_argument("-d","--directory", help="Directory where results are saved.")
args = parser.parse_args()

ks = []
offsets = []
metrics_dict = {}

for root, dirs, files in os.walk(args.directory):
    if "_bridge.txt" in files and "_result.txt" in files:
        folder = root.split("/")[-1]
        k = folder.split("_")[1]
        offset = folder.split("_")[-1]
        if k not in ks:
            ks.append(k)
        if offset not in offsets:
            offsets.append(offset)
        metrics_dict[(k, offset)] = get_metrics_from_folder(Path(root))

# print(metrics_dict)

ks_f = [float(k) for k in ks]
offsets_f = [float(offset) for offset in offsets]
ks_f.sort()
offsets_f.sort()

r_shape = (len(offsets_f), len(ks_f))
k_array = np.full(r_shape, ks_f)
offset_array = np.full((r_shape[-1], r_shape[0]), offsets_f).T

metric_names = ["formation_time","average_travel_time", "num_robots", \
    "box_width", "box_height", "c_ratio"]
metrics_np_dict = {}
for metric_name in metric_names:
    metrics_np_dict[metric_name] = np.zeros(r_shape)
    for k_count, k in enumerate(ks):
        for o_count, offset in enumerate(offsets):
            metrics_np_dict[metric_name][o_count, k_count] = \
                metrics_dict[(k, offset)][metric_name]

for metric_name in metric_names:
    fig, ax = plt.subplots(subplot_kw={"projection": "3d"})
    surf = ax.plot_surface(k_array, offset_array, metrics_np_dict[metric_name], cmap=cm.cool)
    ax.set_xlabel("Scaling factor")
    ax.set_ylabel("Offset")
    plt.title(metric_name)
    plt.show()
