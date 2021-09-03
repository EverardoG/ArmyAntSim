import argparse
import os
from typing import Dict, List
from utils import get_metrics_from_folder, METRICS
from pathlib import Path
from functools import cmp_to_key
import numpy as np
import matplotlib.pyplot as plt
from matplotlib import cm
import pprint

PP = pprint.PrettyPrinter(indent=4)

def create_surface_plots(ks,offsets):
    ks_f = [float(k) for k in ks]
    offsets_f = [float(offset) for offset in offsets]
    ks_f.sort()
    offsets_f.sort()

    r_shape = (len(offsets_f), len(ks_f))
    k_array = np.full(r_shape, ks_f)
    offset_array = np.full((r_shape[-1], r_shape[0]), offsets_f).T

    # metric_names = ["formation_time","average_travel_time", "num_robots", \
    #     "box_width", "box_height", "c_ratio"]
    metrics_np_dict = {}
    for metric_name in METRICS:
        metrics_np_dict[metric_name] = np.zeros(r_shape)
        for k_count, k in enumerate(ks):
            for o_count, offset in enumerate(offsets):
                metrics_np_dict[metric_name][o_count, k_count] = \
                    metrics_dict[(k, offset)][metric_name]

    for metric_name in METRICS:
        fig, ax = plt.subplots(subplot_kw={"projection": "3d"})
        surf = ax.plot_surface(k_array, offset_array, metrics_np_dict[metric_name], cmap=cm.cool)
        ax.set_xlabel("Scaling factor")
        ax.set_ylabel("Offset")
        plt.title(metric_name)
        plt.show()

def plot_metric(metric_name:str, metrics_dict: Dict, ks: List, offsets: List):
    # Sort the offsets and ks by size
    ks = sorted(ks, key=cmp_to_key(lambda item1, item2: float(item1) - float(item2)))
    offsets = sorted(offsets, key=cmp_to_key(lambda item1, item2: float(item1) - float(item2)))
    # Grab all of the ks and offsets with successful bridge formation
    offset_to_metric = {}
    good_ks_dict = {}
    for offset in offsets:
        good_ks = []
        good_metrics = []
        for k in ks:
            if (k, offset) in list(metrics_dict.keys()):
                good_ks.append(float(k))
                good_metrics.append(metrics_dict[(k, offset)][metric_name])
        offset_to_metric[offset] = good_metrics
        good_ks_dict[offset] = good_ks

    # Plot all of the ks and offsets with successful bridge formation
    cmap = cm.get_cmap('viridis')
    color_indicies = np.linspace(0.25, 0.75, len(offsets))
    colors = [cmap(color_index) for color_index in color_indicies]
    for offset, color in zip(offsets, colors):
        plt.plot(good_ks_dict[offset], offset_to_metric[offset],'.:', color = color)
        # print("good_ks_dict[offset]", good_ks_dict[offset])
    plt.title("Results for "+metric_name)
    plt.legend(["Offset: "+ str(float(offset)/1.02) for offset in offsets])
    plt.xlabel("K")
    plt.ylabel(metric_name)
    plt.show()


# Parse the commandline for an optional top directory to store results of these tests in
parser = argparse.ArgumentParser(description="Process the sweep from the simulator.")
parser.add_argument("directory", help="Directory where results are saved.")
parser.add_argument("-m", "--metric", help="Metric to plot")
args = parser.parse_args()

ks = []
offsets = []
metrics_dict = {}
# bad_vals = []
bad_dirs = []
for root, dirs, files in os.walk(args.directory):
    if "_bridge.txt" in files and "_result.txt" in files:
        folder = root.split("/")[-1]
        k = folder.split("_")[1]
        offset = folder.split("_")[-1]
        if k not in ks:
            ks.append(k)
        if offset not in offsets:
            offsets.append(offset)
        # try:
        metrics_dict[(k, offset)] = get_metrics_from_folder(Path(root))
        # except FileNotFoundError:
        #     bad_dirs.append(root)
        #     bad_vals.append((k,offset))
    else:
        bad_dirs.append(root)

PP.pprint(bad_dirs)
# print(bad_vals)

if args.metric is None:
    for metric_name in METRICS:
        plot_metric(metric_name, metrics_dict, ks, offsets)
else:
    plot_metric(args.metric, metrics_dict, ks, offsets)
