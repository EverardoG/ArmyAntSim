import argparse
import os
from typing import Dict, List
from utils import get_metrics_from_folder, METRICS
from pathlib import Path
from functools import cmp_to_key
import numpy as np
import matplotlib.pyplot as plt
from matplotlib import cm
from matplotlib import colorbar
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

# TODO: Make it so that COB and COM plot subplots
# TODO: Make a helper function that just grabs the good values of a metric for plotting

def get_results_for_metric(metric_name: str, metrics_dict: Dict, ks: List, offsets: List):
    # Sort the offsets and ks by size
    sorted_ks = sorted(ks, key=cmp_to_key(lambda item1, item2: float(item1) - float(item2)))
    sorted_offsets = sorted(offsets, key=cmp_to_key(lambda item1, item2: float(item1) - float(item2)))
    # print(offsets)
    # Grab all of the ks and offsets with successful bridge formation
    offset_to_metric = {}
    good_ks_dict = {}
    for offset in sorted_offsets:
        good_ks = []
        good_metrics = []
        for k in sorted_ks:
            if (k, offset) in list(metrics_dict.keys()):
                # if metric_name == "dissolution_time":
                    # print("get_results_for_metric")
                    # print(metrics_dict[(k, offset)][metric_name])
                # Don't take any dissolution times or travel times below 0.0
                if metric_name not in ["dissolution_time", "travel_time"] or float(metrics_dict[(k,offset)][metric_name]) > 0.0:
                    good_ks.append(float(k))
                    good_metrics.append(metrics_dict[(k, offset)][metric_name])
        offset_to_metric[offset] = good_metrics
        good_ks_dict[offset] = good_ks
    return offset_to_metric, good_ks_dict, sorted_ks, sorted_offsets

def plot_cob_com_comparison(metrics_dict: Dict, ks: List, offsets: List, show: bool = True):
    offset_to_cob, good_ks_dict, _, sorted_offsets = get_results_for_metric("cob", metrics_dict, ks, offsets)
    offset_to_com, _, _, _ = get_results_for_metric("com", metrics_dict, ks, offsets)

    # Create two plots - one for xs and one for ys
    fig, (ax_xs, ax_ys, ax_color_com, ax_color_cob) = plt.subplots(1, 4, sharey=False, gridspec_kw={'width_ratios': [1,1,0.1,0.1]})

    # Set up color maps for cob and com
    cmap_cob_name = 'Purples'
    cmap_com_name = 'Oranges'
    cmap_cob = cm.get_cmap(cmap_cob_name)
    cmap_com = cm.get_cmap(cmap_com_name)
    color_indicies = np.linspace(0.25, 0.75, len(offsets))
    colors_cob = [cmap_cob(color_index) for color_index in color_indicies]
    colors_com = [cmap_com(color_index) for color_index in color_indicies]

    # Plot the xs and ys of cob
    for offset, color_cob in zip(sorted_offsets, colors_cob):
        # Turn cob results into numpy array
        cob_arr = np.array(offset_to_cob[offset])
        # Plot cob x
        ax_xs.plot(good_ks_dict[offset], cob_arr[:,0], '.:', color=color_cob)
        # Plot cob y
        ax_ys.plot(good_ks_dict[offset], cob_arr[:,1], '.:', color=color_cob)
    # Plot the xs and ys of com
    for offset, color_com in zip(sorted_offsets, colors_com):
        # Turn com results into numpy array
        com_arr = np.array(offset_to_com[offset])
        # Plot com x
        ax_xs.plot(good_ks_dict[offset], com_arr[:,0], '.:', color=color_com)
        # Plot com y
        ax_ys.plot(good_ks_dict[offset], com_arr[:,1], '.:', color=color_com)
    # Label everything
    fig.suptitle("Center of Mass (COM) vs Center of Bounding Box (COBB)")
    ax_xs.set_title("X Comparison")
    ax_ys.set_title("Y Comparison")
    for ax in [ax_xs, ax_ys]:
        ax.set_ylabel("Position (m)")
        ax.set_xlabel("k")
    # Set up rightmost axes as colorbars
    # Remove ticks
    for ax, colors, title in zip([ax_color_com, ax_color_cob], [colors_com, colors_cob], ["COM", "COBB"]):
        # Remove ticks
        # tick_removal_axes='both'
        # enable_right_ticks = False
        # if ax is ax_color_cob:
        #     tick_removal_axes='x'
        #     enable_right_ticks = True
        # print(tick_removal_axes)
        # ax.tick_params(
        #     axis='both',          # changes apply to the x-axis
        #     which='both',      # both major and minor ticks are affected
        #     reset=True,
        #     bottom=False,      # ticks along the bottom edge are off
        #     top=False,         # ticks along the top edge are off
        #     left=False,
        #     right=enable_right_ticks,
        #     labelbottom=False, # labels along the bottom edge are off
        #     labelleft=False
        # )

        # Remove ticks
        ax.set_xticks([])
        ax.yaxis.tick_right()
        ax.set_yticks([0,19])
        if ax is ax_color_cob:
            ax.set_yticklabels(['Offset: '+str(offset) for offset in [sorted_offsets[-1], sorted_offsets[0]] ], minor=False)
        else:
            ax.set_yticklabels(['',''])

        # set up the color bars as images
        colors_np = np.flip(np.expand_dims(np.array(colors), axis=1), axis=0)
        ax.imshow(colors_np)
        ax.set_title(title)
    # import sys; sys.exit(0)
    # Set up labels that show colors corresponding to offsets
    # ax_color_cob.set_yticks([0,19])
    # ax_color_cob.set_yticklabels(['Offset: '+str(offset) for offset in [sorted_offsets[0], sorted_offsets[-1]] ], minor=True)
    # ax_color_cob.set_yticklabels([1.0,1.0])



    # ax_color_cob.axis('off')

    # Add a color bar to each subplot
    # fig, axs_colorbar = plt.subplots(1,2)
    # cmaps = ['Oranges', 'Purples']
    # for cmap, ax_colorbar in zip([cmap_cob_name, cmap_com_name], axs_colorbar):
    #     fake_mesh = ax_colorbar.pcolormesh(np.ones((2,2)),cmap = cmap)
    #     fig.colorbar(fake_mesh, ax=ax_ys)

    # Set up a color bar
    # cb = colorbar.ColorbarBase(ax_xs, orientation = 'horizontal', cmap=cmap_cob)
    # cob_legend = ["Offset (COBB): " + str(float(offset)/1.02) for offset in sorted_offsets]
    # com_legend = ["Offset (COM): " + str(float(offset)/1.02) for offset in sorted_offsets]
    # legend = cob_legend + com_legend
    # ax_xs.legend(legend)
    # ax_ys.legend(legend)
    if show:
        plt.show()
    return fig, (ax_xs, ax_ys)

def plot_metric(metric_name:str, metrics_dict: Dict, ks: List, offsets: List):
    offset_to_metric, good_ks_dict, sorted_ks, sorted_offsets = get_results_for_metric(metric_name, metrics_dict, ks, offsets)
    # Plot all of the ks and offsets with successful bridge formation
    cmap = cm.get_cmap('viridis')
    color_indicies = np.linspace(0.25, 0.75, len(offsets))
    colors = [cmap(color_index) for color_index in color_indicies]
    # print(sorted_offsets)
    for offset, color in zip(sorted_offsets, colors):
        plt.plot(good_ks_dict[offset], offset_to_metric[offset],'.:', color = color)
    plt.title("Results for "+metric_name)
    plt.legend(["Offset: "+ str(float(offset)/1.02) for offset in sorted_offsets])
    plt.xlabel("K")
    plt.ylabel(metric_name)
    plt.show()

def write_incomplete_dirs_to_yaml(incomplete_dirs: List[str], filepath: str)->None:
    import io
    import yaml
    yaml_dict = {"incomplete_dirs": incomplete_dirs}
    with io.open(filepath, 'w', encoding='utf8') as outfile:
        yaml.dump(yaml_dict, outfile, default_flow_style=False, allow_unicode=True)


# Parse the commandline for an optional top directory to store results of these tests in
parser = argparse.ArgumentParser(description="Process the sweep from the simulator.")
parser.add_argument("directory", help="Directory where results are saved.")
parser.add_argument("-m", "--metric", help="Metric to plot")
args = parser.parse_args()

ks = []
offsets = []
metrics_dict = {}
incomplete_dirs = []
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
    else:
        incomplete_dirs.append(root)

PP.pprint(incomplete_dirs)
print(len(incomplete_dirs))

# Find files missing a _result.txt file
no_result_dirs = []
for root, dirs, files in os.walk(args.directory):
    if "_result.txt" not in files:
        no_result_dirs.append(root)
        
PP.pprint(no_result_dirs)

if args.metric is None:
    for metric_name in METRICS:
        plot_metric(metric_name, metrics_dict, ks, offsets)
elif args.metric == "cob-com":
    plot_cob_com_comparison(metrics_dict, ks, offsets)
else:
    plot_metric(args.metric, metrics_dict, ks, offsets)
