import argparse
import os
from typing import Dict, List, Optional
from utils import get_metrics_from_folder, METRICS
from pathlib import Path
from functools import cmp_to_key
import numpy as np
import matplotlib.pyplot as plt
from matplotlib import cm, colorbar
from matplotlib.colors import ListedColormap
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

def get_results_for_metric(metric_name: str, metrics_dict: Dict, ks: List, offsets: List, filter_bad_dissolution: bool = False):
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
                if filter_bad_dissolution:
                    if metric_name not in ["dissolution_time", "travel_time"] or float(metrics_dict[(k,offset)][metric_name]) > 0.0:
                        good_ks.append(float(k))
                        good_metrics.append(metrics_dict[(k, offset)][metric_name])
                else:
                    good_ks.append(float(k))
                    good_metrics.append(metrics_dict[(k, offset)][metric_name])
        offset_to_metric[offset] = good_metrics
        good_ks_dict[offset] = good_ks
    return offset_to_metric, good_ks_dict, sorted_ks, sorted_offsets

def plot_cob_com_comparison(metrics_dict: Dict, ks: List, offsets: List, show: bool = True, save_dir: Optional[str] = None):
    offset_to_cob, good_ks_dict, _, sorted_offsets = get_results_for_metric("cob", metrics_dict, ks, offsets)
    offset_to_com, _, _, _ = get_results_for_metric("com", metrics_dict, ks, offsets)

    # Create two plots - one for xs and one for ys
    fig, (ax_xs, ax_ys, ax_color_com, ax_color_cob) = plt.subplots(1, 4, sharey=False, gridspec_kw={'width_ratios': [1,1,0.1,0.1]}, figsize=(24, 13))

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
    if save_dir is not None:
        fig.savefig(save_dir+'cob-com.png')
    if show:
        plt.show()
    return fig, (ax_xs, ax_ys)


def plot_cob_com_comparison_with_delta(metrics_dict: Dict, ks: List, offsets: List, show: bool = True, save_dir: Optional[str] = None, y_correction: bool = True, normalize_to_bl = True, include_island: bool = True, include_com_in_deltas: bool = True, normalize_delta=True):
    offset_to_cob, good_ks_dict, _, sorted_offsets = get_results_for_metric("cob", metrics_dict, ks, offsets)
    offset_to_com, _, _, _ = get_results_for_metric("com", metrics_dict, ks, offsets)

    # Create two plots - one for xs and one for ys
    fig, ((ax_xs, ax_ys, ax_color_com),(ax_xs_delta, ax_ys_delta, ax_color_cob)) = plt.subplots(2, 3, sharey=False, gridspec_kw={'width_ratios': [1,1,0.1]}, figsize=(24, 13))
    # Set up color maps for cob and com
    cmap_cob_name = 'BuPu'
    cmap_com_name = 'Oranges'
    cmap_cob = cm.get_cmap(cmap_cob_name)
    cmap_com = cm.get_cmap(cmap_com_name)
    color_indicies = np.linspace(0.25, 0.75, len(offsets))
    colors_cob = [cmap_cob(color_index) for color_index in color_indicies]
    colors_com = [cmap_com(color_index) for color_index in color_indicies]

    def correct_y(arr: np.array, normalize_to_bl: bool)->None:
        """Corrects y in place for input array of Nx2 points"""
        arr[:,1] -= 10.2
        arr[:,1] *= -1
        if normalize_to_bl:
            arr[:,1] *= (1/1.02)

    # Plot the island in COM plots if specified
    if include_island:
        island_coords = np.array([[15.3, 6.12]])
        if y_correction: correct_y(island_coords, normalize_to_bl)
        ks_f = [float(k) for k in ks]
        island_linestyle = ":"
        island_linewidth = 1
        ax_xs.plot([min(ks_f), max(ks_f)], np.ones((2))*island_coords[:,0], linestyle=island_linestyle, linewidth=island_linewidth)
        ax_ys.plot([min(ks_f), max(ks_f)], np.ones(2)*island_coords[:,1], linestyle=island_linestyle, linewidth=island_linewidth)
        ax_xs.legend(["Island Left"])
        ax_ys.legend(["Island Bottom"])

    # Plot the COM in delta plots if specified
    if include_com_in_deltas:
        ks_f = [float(k) for k in ks]
        com_linestyle = ":"
        com_linewidth = 1
        ax_xs_delta.plot([min(ks_f), max(ks_f)], [0, 0], linestyle=com_linestyle, linewidth=com_linewidth)
        ax_ys_delta.plot([min(ks_f), max(ks_f)], [0,0], linestyle=com_linestyle, linewidth=com_linewidth)
        ax_xs_delta.legend(["Center of Mass"], loc='lower right')
        ax_ys_delta.legend(["Center of Mass"], loc='lower right')

    # metrics_dict[(k, offset)] = get_metrics_from_folder(Path(root))

    # Plot the xs and ys of delta from COBB
    linestyle = "."
    markersize = 3
    for offset, color_cob in zip(sorted_offsets, colors_cob):
        # Turn cob results into numpy array
        cob_arr = np.array(offset_to_cob[offset])
        if y_correction: correct_y(cob_arr, normalize_to_bl)
        # Turn com results into numpy array
        com_arr = np.array(offset_to_com[offset])
        if y_correction: correct_y(com_arr, normalize_to_bl)
        # Compute delta
        delta_arr = com_arr - cob_arr
        if normalize_delta:
            # Normalize delta according to the sizes of the bounding boxes
            box_widths = np.array([metrics_dict[(str(k), offset)]['box_width'] for k in good_ks_dict[offset]])
            box_heights = np.array([metrics_dict[(str(k), offset)]['box_height'] for k in good_ks_dict[offset]])
            delta_arr[:,0] *= 1/box_widths/2
            delta_arr[:,1] *= 1/box_heights/2
        # Plot cob x
        ax_xs_delta.plot(good_ks_dict[offset], delta_arr[:,0], linestyle, markersize=markersize, color=color_cob)
        # Plot cob y
        ax_ys_delta.plot(good_ks_dict[offset], delta_arr[:,1], linestyle, markersize=markersize, color=color_cob)
    # Plot the xs and ys of com
    for offset, color_com in zip(sorted_offsets, colors_com):
        # Turn com results into numpy array
        com_arr = np.array(offset_to_com[offset])
        if y_correction: correct_y(com_arr, normalize_to_bl)
        # Plot com x
        ax_xs.plot(good_ks_dict[offset], com_arr[:,0], linestyle, markersize=markersize, color=color_com)
        # Plot com y
        ax_ys.plot(good_ks_dict[offset], com_arr[:,1], linestyle, markersize=markersize, color=color_com)
    # Label everything
    fig.suptitle("Center of Mass (COM) vs Delta from Center of Bounding Box (COBB)")
    ax_xs.set_title("COM X Position")
    ax_ys.set_title("COM Y Position")
    if normalize_to_bl:
        y_units = " (body-lengths)"
    else:
        y_units = " (m)"
    for ax in [ax_xs, ax_ys]:
        ax.set_ylabel("Position"+y_units)
        ax.set_xlabel("k")
    ax_xs_delta.set_title("Delta X from COBB to COM")
    ax_ys_delta.set_title("Delta Y from COBB to COM")
    if normalize_delta:
        y_units = " (Proportion of Half Bounding Box)"
    for ax in [ax_xs_delta, ax_ys_delta]:
        ax.set_ylabel("Position"+y_units)
        ax.set_xlabel("k")
    # Set up rightmost axes as colorbars
    # Remove ticks
    for ax, colors in zip([ax_color_com, ax_color_cob], [colors_com, colors_cob]):
        # Remove ticks
        ax.set_xticks([])
        ax.yaxis.tick_right()
        ax.set_yticks([0,19])
        ax.set_yticklabels([str(offset) for offset in [sorted_offsets[-1], sorted_offsets[0]] ], minor=False)

        # set up the color bars as images
        colors_np = np.flip(np.expand_dims(np.array(colors), axis=1), axis=0)
        ax.imshow(colors_np)
        ax.set_title("Offset")
    if save_dir is not None:
        fig.savefig(save_dir+'cob-com.png')
    if show:
        plt.show()
    return fig, (ax_xs, ax_ys)

def plot_formation_dissolution_grid(metrics_dict: Dict, ks: List, offsets: List, show: bool = True, save_dir: Optional[str] = None, mid_k: Optional[str] = None):
    # Create an array the size of the ks and offsets
    # Populate it with numbers for bridge formation/ dissolution
    # 1. Goal Not Reached, No Successful Bridge Formation
    # 2. Bridge Formed but did not fully dissolve
    # 3. Bridge Formed and fully dissolved

    # If the bridge has a formation time of 0, I know it didn't form
    # If the bridge has a dissolution time >0, then I know it successfully dissolved
    # If the bridge has a dissolution time of 0, then I know that it did not successfully dissolve
    offset_to_dissolution_time, good_ks_dict, sorted_ks, sorted_offsets = get_results_for_metric("dissolution_time", metrics_dict, ks, offsets)

    # For debugging
    # offset_to_formation_time, _, _, _ = get_results_for_metric("formation_time", metrics_dict, ks, offsets)
    # offset_to_travel_time, _, _, _= get_results_for_metric("travel_time", metrics_dict, ks, offsets)

    # Generate the array with everything initiailized to 1 for no bridge formation
    success_grid = np.ones((len(offsets), len(ks)))

    # ks_f = [float(k) for k in sorted_ks]
    # offsets_f = [float(offset) for offset in sorted_offsets]
    good_vals = []
    for offset in sorted_offsets:
        for k in good_ks_dict[offset]:
            good_vals.append((offset, k))
    # PP.pprint(good_vals)

    print("Grid info")
    print(f"offsets: {sorted_offsets}")
    print(f"ks: {sorted_ks}")

    k_to_plot_ind = {}
    for plot_ind_k, k in enumerate(sorted_ks):
        k_to_plot_ind[k] = plot_ind_k
    # Look at good ks for a specific offset
    check_offset = "0.29"
    print(f"Good ks for offset of {check_offset}")
    print(good_ks_dict[check_offset])

    # PP.pprint(k_to_ind)

    for ind_offset, offset in enumerate(np.flip(sorted_offsets)):
        if offset == "0.29":
            print("Checkin in for loop")
            print(good_ks_dict["0.29"])
        for ind_k, k in enumerate(good_ks_dict[offset]): # This is NOT the TRUE index of k -> This is only the index of the k in this subsampled list, which does not include ALL ks. That means that the index is going to be wrong for higher k values a lot of the time.
            plot_ind_k = k_to_plot_ind[str(k)]
            # print(f"(offset, k) : {(offset,k)} | (ind_offset, ind_k, plot_ind_k) : {ind_offset, ind_k, plot_ind_k}")
            # We know that a bridge successfully formed for this data point
            # try:
            dissolution_time = offset_to_dissolution_time[offset][ind_k]
            # except IndexError:
            #     print(offset_to_dissolution_time[offset])
            #     print(len(offset_to_dissolution_time[offset]))
            #     import sys; sys.exit(0)
            # If there is a dissolution time, then we have case 3 where the
            # bridge formed AND fully dissolved
            if dissolution_time > 0:
                case_val = 3
            # Otherwise it means a bridge formed but did NOT fully dissolve
            else:
                case_val = 2
            success_grid[ind_offset,plot_ind_k] = case_val
            if k == 4.44 and offset == "0.29":
                print(f"case_val for this check is: {case_val}")
                print(success_grid)
            # if float(offset) == 5.6 and float(k) == 20.0:
            #     print(f"Highest K, Highest Offset | {case_val}")
            #     success_grid[ind_offset,plot_ind_k] = case_val
            # elif float(offset) == 0.0 and float(k) == 20.0:
            #     print(f"Highest K, Lowest Offset | {case_val}")
            #     success_grid[ind_offset,plot_ind_k] = case_val
            # elif float(offset) == 5.6 and float(k) == 0.1:
            #     print(f"Lowest K, Highest Offset | {case_val}")
            #     success_grid[ind_offset,plot_ind_k] = case_val
            # elif float(offset) == 0.0 and float(k) == 0.1:
            #     print(f"Lowest K, Lowest Offset | {case_val}")
            #     success_grid[ind_offset,plot_ind_k] = case_val
            # print(success_grid)

    print(success_grid)
    # Plot the results

    # Set up color map for the grid
    # cmap_name = 'BuPu'
    # cmap = cm.get_cmap(cmap_name)
    # color_indicies = np.linspace(0.25, 0.75, len(offsets))
    # colors = [cmap(color_index) for color_index in color_indicies]

    cmap_name = 'Blues'
    cmap_start = cm.get_cmap(cmap_name)
    color_indicies = np.linspace(0.5, 0.75, 3)
    colors = [cmap_start(color_index) for color_index in color_indicies]
    custom_cmap = ListedColormap(colors)

    # Have to handle cases where only 2 colors are actually represented
    # For now, only going to program in cases that I come accross
    cmap_23 = ListedColormap(colors[1:])

    if mid_k is None:
        # Left is the colormap itself and on the right is a colorbar/label
        fig, axs = plt.subplots(1, 2, sharey=False, gridspec_kw={'width_ratios': [1,0.1]}, figsize=(24, 13))
        ax_grid, ax_colorbar = axs
        ax_grid.imshow(success_grid, cmap = custom_cmap)
    else:
        fig, axs = plt.subplots(1,3, sharey=False, gridspec_kw={'width_ratios': [1,1,0.1]}, figsize=(24, 13))
        (ax_grid_low_ks, ax_grid_high_ks, ax_colorbar) = axs

        # Split grid so that hte mid k value is on the left side of the grid
        mid_k_ind = sorted_ks.index(mid_k)
        success_grid_low_ks = success_grid[:, :mid_k_ind+1]
        success_grid_high_ks = success_grid[:, mid_k_ind+1:]
        custom_cmap_highks = custom_cmap
        if 1 not in success_grid_high_ks:
            custom_cmap_highks = cmap_23
        ax_grid_low_ks.imshow(success_grid_low_ks, cmap = custom_cmap)
        ax_grid_high_ks.imshow(success_grid_high_ks, cmap = custom_cmap_highks)

        # Set up ticks for left and right
        offset_tick_positions = np.arange(len(sorted_offsets))
        k_tick_positions = np.arange(len(sorted_ks))
        low_k_tick_positions = k_tick_positions[:mid_k_ind+1]
        high_k_tick_positions = k_tick_positions[mid_k_ind+1:] - min(k_tick_positions[mid_k_ind+1:])

        # Place the ticks on the plot
        ax_grid_low_ks.set_xticks(low_k_tick_positions)
        ax_grid_low_ks.set_yticks(offset_tick_positions)
        ax_grid_high_ks.set_xticks(high_k_tick_positions)
        ax_grid_high_ks.set_yticks(offset_tick_positions)

        # PLace labels on the ticks
        # Turn offsets into bl
        sorted_offsets_bl = ['%.2f' % (float(o)/1.02) for o in sorted_offsets]
        ax_grid_low_ks.set_xticklabels(sorted_ks[:mid_k_ind+1])
        ax_grid_low_ks.set_yticklabels(np.flip(sorted_offsets_bl))
        ax_grid_high_ks.set_xticklabels(sorted_ks[mid_k_ind+1:])
        ax_grid_high_ks.set_yticklabels([])

        # Label axes
        ax_grid_low_ks.set_xlabel("k")
        ax_grid_low_ks.set_ylabel("Offset (body-lengths)")
        ax_grid_high_ks.set_xlabel("k")

        # Add subplot titles
        ax_grid_low_ks.set_title("For low k values")
        ax_grid_high_ks.set_title("For high k values")

    # Add a title
    fig.suptitle("Bridge Formation and Dissolution Results")

    # Setup colorbar
    colorbar_im = np.expand_dims(np.flip(np.arange(3)), axis = 1)
    ax_colorbar.imshow(colorbar_im, cmap = custom_cmap)

    # Setup ticks on colorbar
    ax_colorbar.set_title("Legend")
    ax_colorbar.set_xticks([])
    ax_colorbar.yaxis.tick_right()
    ax_colorbar.set_yticks([0,1,2])
    ax_colorbar.set_yticklabels(["Successful Formation\n  and Dissolution", "Successful Formation,\n  Failed Dissolution", "Failed Formation"])

    if save_dir is not None:
        fig.savefig(save_dir+'formation-dissolution-grid.png')
    if show:
        plt.show()
    return fig, axs

def plot_num_robots_bridge_and_dissolution(metrics_dict: Dict, ks: List, offsets: List, show: bool = True, save_dir: Optional[str] = None):
    offset_to_initial_num, good_ks_dict, sorted_ks, sorted_offsets = get_results_for_metric("num_robots_bridge_initial", metrics_dict, ks, offsets)
    offset_to_final_num, _, _, _ = get_results_for_metric("num_robots_bridge_final", metrics_dict, ks, offsets)
    # offset_to_percent_dissolution, _, _, _ = get_results_for_metric("percent_dissolution", metrics_dict, ks, offsets)

    # Plot all of the ks and offsets with successful bridge formation
    cmap = cm.get_cmap('viridis')
    color_indicies = np.linspace(0.25, 0.75, len(offsets))
    colors = [cmap(color_index) for color_index in color_indicies]
    # print(sorted_offsets)
    fig, ax_formation = plt.subplots(1,1, sharey=False, figsize=(24, 13))
    for offset, color in zip(sorted_offsets, colors):
        ax_formation.plot(good_ks_dict[offset], offset_to_initial_num[offset],'o', color = color, markersize=1.5)
        # ax_dissolution.plot(good_ks_dict[offset], offset_to_percent_dissolution[offset], ':o',color = color, markersize=1.5)
    # plt.title("Results for "+metric_name)
    ax_formation.legend(["Offset: "+ str(float(offset)/1.02) for offset in sorted_offsets])
    for offset, color in zip(sorted_offsets, colors):
        # Filter out any 0.0s
        offset_to_final_num_list = []
        corresponding_final_ks = []
        for result, k in zip(offset_to_final_num[offset], good_ks_dict[offset]):
            # print(result, k, type(result), type(k))
            if int(result) > 0:
                # print("")
                offset_to_final_num_list.append(result)
                corresponding_final_ks.append(k)
        ax_formation.plot(corresponding_final_ks, offset_to_final_num_list,'x', color = color, markersize=3)
    ax_formation.set_xlabel("K")
    ax_formation.set_ylabel("Number of Robots in Bridge State")
    if save_dir is not None:
        fig.savefig(save_dir+metric_name+".png")
    if show:
        plt.show()

def plot_metric(metric_name:str, metrics_dict: Dict, ks: List, offsets: List, show: bool = True, save_dir: Optional[str] = None):
    offset_to_metric, good_ks_dict, sorted_ks, sorted_offsets = get_results_for_metric(metric_name, metrics_dict, ks, offsets)
    # Plot all of the ks and offsets with successful bridge formation
    cmap = cm.get_cmap('viridis')
    color_indicies = np.linspace(0.25, 0.75, len(offsets))
    colors = [cmap(color_index) for color_index in color_indicies]
    # print(sorted_offsets)
    fig = plt.figure(figsize=(24, 13))
    for offset, color in zip(sorted_offsets, colors):
        plt.plot(good_ks_dict[offset], offset_to_metric[offset],'.:', color = color)
    plt.title("Results for "+metric_name)
    plt.legend(["Offset: "+ str(float(offset)/1.02) for offset in sorted_offsets])
    plt.xlabel("K")
    plt.ylabel(metric_name)
    if save_dir is not None:
        fig.savefig(save_dir+metric_name+".png")
    if show:
        plt.show()

def write_incomplete_dirs_to_yaml(incomplete_dirs: List[str], filepath: str)->None:
    import io
    import yaml
    yaml_dict = {"incomplete_dirs": incomplete_dirs}
    with io.open(filepath, 'w', encoding='utf8') as outfile:
        yaml.dump(yaml_dict, outfile, default_flow_style=False, allow_unicode=True)

def get_early_termination_sims(ks, offsets, metrics_dict):
    stacking_list = []
    towering_list = []
    stuck_list = []
    toolong_list = []
    for k in ks:
        for offset in offsets:
            if (k, offset) in metrics_dict:
                results = metrics_dict[(k, offset)]
                if results["m_stacking"] == "1":
                    stacking_list.append((k,offset))
                if results["m_towering"] == "1":
                    towering_list.append((k, offset))
                if results["m_simulationStuck"] == "1":
                    stuck_list.append((k, offset))
                if results["m_tooLongDissolution"] == "1":
                    toolong_list.append((k, offset))
    return stacking_list, towering_list, stuck_list, toolong_list


# Parse the commandline for an optional top directory to store results of these tests in
parser = argparse.ArgumentParser(description="Process the sweep from the simulator.")
parser.add_argument("directory", help="Directory where results are saved.")
parser.add_argument("-m", "--metric", help="Metric to plot")
parser.add_argument("-s", "--save", help="Directory to save images of plots in")
parser.add_argument("-q", "--quiet", help="Don't show any plots", default=False)

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

# Get early termination sims
stacking_list, towering_list, stuck_list, toolong_list = get_early_termination_sims(ks, offsets, metrics_dict)
print(f"Stuck simulations: {len(stuck_list)}")
PP.pprint(stuck_list)
print("Simulations ran too long:")
PP.pprint(toolong_list)


if args.metric is None:
    for metric_name in METRICS:
        plot_metric(metric_name, metrics_dict, ks, offsets, show = not args.quiet, save_dir = args.save)
elif args.metric is None or args.metric == "cob-com" or args.metric == "com-cob":
    plot_cob_com_comparison(metrics_dict, ks, offsets, show = not args.quiet, save_dir = args.save)
elif args.metric is None or args.metric == "cob-com-delta" or args.metric == "com-cob-delta":
    plot_cob_com_comparison_with_delta(metrics_dict, ks, offsets, show = not args.quiet, save_dir = args.save)
elif args.metric is None or args.metric == "formation-grid":
    plot_formation_dissolution_grid(metrics_dict, ks, offsets, show = not args.quiet, save_dir = args.save, mid_k = "2.5")
elif args.metric is None or args.metric == "num-robots-formation":
    plot_num_robots_bridge_and_dissolution(metrics_dict, ks, offsets, show = not args.quiet, save_dir = args.save)
else:
    plot_metric(args.metric, metrics_dict, ks, offsets, show = not args.quiet, save_dir = args.save)
