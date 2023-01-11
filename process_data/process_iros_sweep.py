import os
from pathlib import Path
import argparse
import pprint
from typing import Optional, List
import numpy as np
import matplotlib.pyplot as plt
from matplotlib.lines import Line2D

from utils import get_metrics_from_folder

# Set global printing function
PP = pprint.PrettyPrinter(indent=4)

# Set global matplotlib parameters
TICK_FONT_SIZE = 13
AXES_FONT_SIZE = 18
FACE_COLOR = 0.91
GRID_COLOR = 0.98
MARKER_SIZE = 10
LEGEND_MARKER_SIZE = 8

plt.rc('xtick', labelsize=TICK_FONT_SIZE)
plt.rc('ytick', labelsize=TICK_FONT_SIZE)
plt.rc('legend', fontsize=TICK_FONT_SIZE)
plt.rc('axes', labelsize=AXES_FONT_SIZE)

plt.rc('mathtext', fontset='stix')
plt.rc('font', family='STIXGeneral')

TERRAIN_LABELS = {
    "pit" : "Canyon",
    "terrace": "Cliff",
    "stepdown": "Overhang",
    "island": "Island"
}

# Get colors for plotting and legend
COLOR_CYCLE = plt.rcParams['axes.prop_cycle'].by_key()['color']

# Set constants for labelling things
FORMATION = 0
TRAVEL = 1
DISSOLUTION = 2

STD_DEV = 0
FULL_RANGE = 1
NONE = 3

def getTerrainX(ts_path: str):
    terrain = ts_path.split("_")[0]
    x = ts_path.split("_")[1]
    return terrain, x

def getAllResults(folder_path: Path, populate_robots_at_times = True):
    terrain_x_paths = os.listdir(folder_path)
    terrains = list({ts_path.split("_")[0] for ts_path in terrain_x_paths})
    terrains.sort()
    xs = list({ts_path.split("_")[1] for ts_path in terrain_x_paths})
    xs.sort(key=lambda x:float(x))
    all_results = {}
    for ts_path in terrain_x_paths:
        terrain, x = getTerrainX(ts_path)
        all_results[(terrain, x)] = []
        for trial in os.listdir(folder_path + "/" + ts_path):
            full_trail_path = folder_path + "/" + ts_path + "/" + trial
            metrics = get_metrics_from_folder(Path(full_trail_path), populate_robots_at_times=populate_robots_at_times)
            all_results[(terrain, x)].append(metrics)
    return all_results, terrains, xs

def stylizePlot(ax, xlabels=Optional[List[float]]):
    # Setup xticks with labels
    if xlabels is not None:
        ax.set_xticks(xlabels)
        ax.set_xticklabels(xlabels)
        ax.set_xlim([min(xlabels), max(xlabels)])
    # Setup grid
    ax.set_facecolor((FACE_COLOR,FACE_COLOR,FACE_COLOR))
    ax.grid(color=(GRID_COLOR,GRID_COLOR,GRID_COLOR), linewidth=2)
    # Make borders and ticks white
    ax.tick_params(color="white")
    for spine in ax.spines.values():
        spine.set_edgecolor("white")
    return None


def calculateSuccessRates(all_results, terrains, xs, which):
    # key is terrain. Value is a list of percentage formation success in order of xs
    success_rates_dict = {}

    for terrain in terrains:
        success_rate_for_terrain = []
        for x in xs:
            trials_results = all_results[(terrain, x)]
            if which == FORMATION:
                times = [t["formation_time"] for t in trials_results]
            elif which == TRAVEL:
                times = [t["travel_time"] for t in trials_results]
            elif which == DISSOLUTION:
                times = [t["dissolution_time"] for t in trials_results]
            else:
                raise Exception(f"Input for \"which\" is not valid: {which}")
            num_successful = [ft is not None and ft != 0.0 for ft in times]
            success_rate = sum(num_successful)/len(num_successful)
            success_rate_for_terrain.append(success_rate)
        success_rates_dict[terrain] = success_rate_for_terrain

    return success_rates_dict

def plotSuccesRates(ax, all_results, terrains, xs, which, x_name):
    # Grab all the success rates
    success_rates_dict = calculateSuccessRates(all_results, terrains, xs, which)
    # Convert xs to floats
    xs_f = [float(x) for x in xs]
    # Plot the success rate for each terrain
    for terrain, c in zip(terrains, COLOR_CYCLE[:len(terrains)]):
        ax.plot(xs_f, success_rates_dict[terrain],'.-', color=c, markersize=MARKER_SIZE)
    # Create a legend for the terrains
    custom_legend_handles = [Line2D([0],[0], color=c, marker='o', linestyle='', markersize=LEGEND_MARKER_SIZE) for c in COLOR_CYCLE[:len(terrains)]]
    terrain_legend = [TERRAIN_LABELS[terrain] for terrain in terrains]
    ax.legend(custom_legend_handles, terrain_legend)
    # Create labels for x and y
    if which == FORMATION:
        ax.set_ylabel("Formation Sucess Rate")
    elif which == TRAVEL:
        ax.set_ylabel("Utility Sucess Rate")
    elif which == DISSOLUTION:
        ax.set_ylabel("Dissolution Success Rate")
    else:
        raise Exception(f"Input for \"which\" is not valid: {which}")
    # ax.set_xlabel(r'$\sigma$'+" Noise (BL)")
    ax.set_xlabel(x_name)
    ax.set_ylim([0,1.05])
    # Stylize plot so it looks nice
    stylizePlot(ax, xs_f)
    return None

def plotSigmaFormationSuccessRates(ax, all_results, terrains, sigmas):
    return plotSuccesRates(ax, all_results, terrains, sigmas, FORMATION)

def plotSigmaTravelSucessRates(ax, all_results, terrains, sigmas):
    return plotSuccesRates(ax, all_results, terrains, sigmas, TRAVEL)

def filterOutInvalidResults(results, filter_None, filter_0):
    valid_results = []
    for r in results:
        if r is None and filter_None:
            pass
        elif r == 0.0 and filter_0:
            pass
        else:
            valid_results.append(r)
    return valid_results

def metricShouldBeFiltered(metric):
    filter_None = False
    filter_0 = False
    if len(metric.split("_")) >= 2:
        if metric.split("_")[1] == "time":
            filter_None = True
            filter_0 = True
        elif metric.split("_")[0] == "num" and metric.split("_")[1] == "robots":
            filter_None = True
            filter_0 = True
        elif metric == "percent_dissolution":
            filter_None = True
        elif metric == "box_width" or metric == "box_height":
            filter_None = True
            filter_0 = True
    return filter_None, filter_0

def calculateMetricStatistics(all_results, terrains, xs, metric):
    # Key is terrain. Value is a list of times in order of xs
    averages = {}
    uppers = {}
    lowers = {}
    std_deviations = {}

    for terrain in terrains:
        averages_for_terrain = []
        uppers_for_terrain = []
        lowers_for_terrain = []
        std_deviations_for_terrain = []
        for x in xs:
            trials_results = all_results[(terrain, x)]
            metric_results = [t[metric] for t in trials_results]
            filter_None, filter_0 = metricShouldBeFiltered(metric)
            valid_metric_results = filterOutInvalidResults(metric_results, filter_None, filter_0)
            if len(valid_metric_results) > 0:
                averages_for_terrain.append(np.average(valid_metric_results))
                uppers_for_terrain.append(np.max(valid_metric_results))
                lowers_for_terrain.append(np.min(valid_metric_results))
                std_deviations_for_terrain.append(np.std(valid_metric_results))
            else:
                averages_for_terrain.append(None)
                uppers_for_terrain.append(None)
                lowers_for_terrain.append(None)
                std_deviations_for_terrain.append(None)
        averages[terrain] = averages_for_terrain
        uppers[terrain] = uppers_for_terrain
        lowers[terrain] = lowers_for_terrain
        std_deviations[terrain] = std_deviations_for_terrain

    return averages, uppers, lowers, std_deviations

def calculateTimeStatisitics(all_results, terrains, sigmas, which):
    # Key is terrain. Value is a list of times in order of sigmas
    if which == FORMATION:
        return calculateMetricStatistics(all_results, terrains, sigmas, "formation_time")
    elif which == TRAVEL:
        return calculateMetricStatistics(all_results, terrains, sigmas, "travel_time")
    else:
        raise Exception(f"Input for \"which\" is not valid: {which}")

def combineValidFloats(list0: List[float], list1: List[float], addition = True):
    new_list = []
    for e0, e1 in zip(list0, list1):
        if e0 is not None and e1 is not None:
            if addition:
                new_list.append(e0+e1)
            else:
                new_list.append(e0-e1)
        else:
            new_list.append(None)
    return new_list

def addValidFloats(list0: List[float], list1: List[float]):
    return combineValidFloats(list0, list1, addition=True)

def subtractValidFloats(list0: List[float], list1: List[float]):
    return combineValidFloats(list0, list1, addition=False)

def filterOutNone(target_list: List[float or None])->List[float]:
    return [e for e in target_list if e is not None]

def getValidXs(xs: List[float], ys: List[float or None]):
    valid_xs = []
    for x,y in zip(xs, ys):
        if y is not None:
            valid_xs.append(x)
    return valid_xs

def plotMetricStatistics(ax, all_results, terrains, xs, metric, x_name, plot_range):
    # Grab all the metric results
    averages, uppers, lowers, std_deviations = calculateMetricStatistics(all_results, terrains, xs, metric)
    # Convert xs to floats
    xs_f = [float(s) for s in xs]
    # Plot the metric results for each terrain (Mean and standard deviation)
    for terrain, c in zip(terrains, COLOR_CYCLE[:len(terrains)]):
        valid_xs = getValidXs(xs_f, averages[terrain])
        avg = filterOutNone(averages[terrain])
        ax.plot(valid_xs, avg, '.-', color=c, markersize=MARKER_SIZE)
        if plot_range == STD_DEV:
            std_dev = filterOutNone(std_deviations[terrain])
            upper_std_dev = np.array(avg) + np.array(std_dev)
            lower_std_dev = np.array(avg) - np.array(std_dev)
            ax.fill_between(valid_xs,lower_std_dev,upper_std_dev,alpha=0.25)
        elif plot_range == FULL_RANGE: # Plot upper and lower ranges if specified
            up = filterOutNone(uppers[terrain])
            low = filterOutNone(lowers[terrain])
            ax.fill_between(valid_xs, up, low, alpha=0.2)
        elif plot_range == NONE:
            pass
    # Create a legend for the terrains
    custom_legend_handles = [Line2D([0],[0], color=c, marker='o', linestyle='', markersize=LEGEND_MARKER_SIZE) for c in COLOR_CYCLE[:len(terrains)]]
    terrain_legend = [TERRAIN_LABELS[terrain] for terrain in terrains]
    ax.legend(custom_legend_handles, terrain_legend)
    # Create labels for x and y
    if metric == "formation_time":
        ax.set_ylabel("Formation Time (s)")
    elif metric == "travel_time":
        ax.set_ylabel("Utility Time (s)")
    elif metric == "num_robots_bridge_initial":
        ax.set_ylabel("Robots in Structure")
    elif metric == "num_robots_bridge_final":
        ax.set_ylabel("Robots in Final Structure")
    elif metric == "box_width":
        ax.set_ylabel("Structure Width (BL)")
    elif metric == "box_height":
        ax.set_ylabel("Structure Height (BL)")
    elif metric == "percent_dissolution":
        ax.set_ylabel("Percent Dissolution (%)")
    ax.set_xlabel(x_name)
    # Set y limits to make plot more readable
    if metric == "formation_time" or metric == "travel_time":
        ax.set_ylim([0,500])
    elif metric == "percent_dissolution":
        ax.set_ylim([0,105])
    elif metric == "box_width" or metric == "box_height":
        ax.set_ylim([0,10])
    elif metric == "num_robots_bridge_initial":
        ax.set_ylim([0,40])

    # Stylize plot so it looks nice
    stylizePlot(ax, xs_f)
    return None

def plotTimeStatistics(ax, all_results, terrains, xs, x_name, which, plot_range):
    if which == FORMATION:
        return plotMetricStatistics(ax, all_results, terrains, xs, "formation_time", x_name, plot_range)
    elif which == TRAVEL:
        return plotMetricStatistics(ax, all_results, terrains, xs, "travel_time", x_name, plot_range)

def plotFormationTimes(ax, all_results, terrains, xs, x_name, plot_range=STD_DEV):
    return plotTimeStatistics(ax, all_results, terrains, xs, x_name, FORMATION, plot_range)

def plotTravelTimes(ax, all_results, terrains, xs, x_name, plot_range=STD_DEV):
    return plotTimeStatistics(ax, all_results, terrains, xs, x_name, TRAVEL, plot_range)

def plotTrafficNumRobotsFormation(ax, all_results, terrains, traffics):
    return plotMetricStatistics(ax, all_results, terrains, traffics, "num_robots_bridge_initial", "Traffic level (robots/min)", STD_DEV)

def plotTrafficNumRobotsTravel(ax, all_results, terrains, traffics):
    return plotMetricStatistics(ax, all_results, terrains, traffics, "num_robots_bridge_final", "Traffic level (robots/min)", STD_DEV)

def plotPercentDissolution(ax, all_results, terrains, traffics):
    return plotMetricStatistics(ax, all_results, terrains, traffics, "percent_dissolution", "Traffic level (robots/min)", STD_DEV)

def generateQuadFigure(all_results, terrains, xs, x_name):
    fig, axs = plt.subplots(1,4,dpi=100,figsize=(24,4))
    plotSuccesRates(axs[0], all_results, terrains, xs, FORMATION, x_name)
    plotFormationTimes(axs[1], all_results, terrains, xs, x_name)
    plotMetricStatistics(axs[2], all_results, terrains, xs, "num_robots_bridge_initial", x_name, STD_DEV)
    plotMetricStatistics(axs[3], all_results, terrains, xs, "percent_dissolution", x_name, STD_DEV)
    fig.tight_layout()
    fig.subplots_adjust(left=0.04)
    return fig, axs

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Plot IROS results.")
    parser.add_argument("-x","--sweep_variable", help="Which variable to plot the sweep for")
    args = parser.parse_args()

    if args.sweep_variable == "sigma":
        folder_path = "/media/egonzalez/Extreme SSD/FlippybotsData/sweep_iros_terrains_sigma_0_to_10"
        all_results, terrains, sigmas = getAllResults(folder_path, populate_robots_at_times=False)
        fig, axs = generateQuadFigure(all_results, terrains, sigmas, r'$\sigma$'+" Noise (BL)")
        plt.show()
    elif args.sweep_variable == "robot_delay":
        # folder_path = "/media/egonzalez/Extreme SSD/FlippybotsData/sweep_iros_terrains_traffic_6_to_12_preliminary(sigma=0.5)"
        # folder_path = "/media/egonzalez/Extreme SSD/FlippybotsData/preliminary_robot_delay"
        folder_path = "/media/egonzalez/Extreme SSD/FlippybotsData/1_robotdelays"
        all_results, terrains, traffics = getAllResults(folder_path, populate_robots_at_times=False)
        fig, axs = generateQuadFigure(all_results, terrains, traffics, "Spawn Delay (BL)")
        plt.show()
    elif args.sweep_variable == "traffic":
        folder_path = "/media/egonzalez/Extreme SSD/FlippybotsData/sweep_iros_terrains_traffic_6_to_12_preliminary(sigma=0.5)"
        all_results, terrains, traffics = getAllResults(folder_path, populate_robots_at_times=False)
        fig, axs = generateQuadFigure(all_results, terrains, traffics, "Traffic (Robots/min)")
        plt.show()
    elif args.sweep_variable == "ks":
        # folder_path = "/media/egonzalez/Extreme SSD/FlippybotsData/preliminary_ks"
        folder_path = "/media/egonzalez/Extreme SSD/FlippybotsData/0_ks"
        all_results, terrains, ks = getAllResults(folder_path, populate_robots_at_times=False)
        fig, axs = generateQuadFigure(all_results, terrains, ks, r'$k$'+" (Gain)")
        plt.show()
    else:
        raise Exception(f"{args.sweep_variable} is not a valid variable for plotting")