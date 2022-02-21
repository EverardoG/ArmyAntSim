import os
from pathlib import Path
import pprint
from typing import Optional, List
import numpy as np
import matplotlib.pyplot as plt
from matplotlib.lines import Line2D

from utils import get_metrics_from_folder

# Set global printing function
PP = pprint.PrettyPrinter(indent=4)

# Set global matplotlib parameters
TICK_FONT_SIZE = 18
AXES_FONT_SIZE = 23
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
    "pit" : "Bridge",
    "terrace": "Cantilever",
    "stepdown": "Ramp",
    "island": "Tower"
}

# Get colors for plotting and legend
COLOR_CYCLE = plt.rcParams['axes.prop_cycle'].by_key()['color']

def getTerrainSigma(ts_path: str):
    terrain = ts_path.split("_")[0]
    sigma = ts_path.split("_")[1]
    return terrain, sigma

def getAllResults(folder_path: Path, populate_robots_at_times = True):
    terrain_sigma_paths = os.listdir(folder_path)
    terrains = list({ts_path.split("_")[0] for ts_path in terrain_sigma_paths})
    terrains.sort()
    sigmas = list({ts_path.split("_")[1] for ts_path in terrain_sigma_paths})
    sigmas.sort(key=lambda x:float(x))
    all_results = {}
    for ts_path in terrain_sigma_paths:
        terrain, sigma = getTerrainSigma(ts_path)
        all_results[(terrain, sigma)] = []
        for trial in os.listdir(folder_path + "/" + ts_path):
            full_trail_path = folder_path + "/" + ts_path + "/" + trial
            metrics = get_metrics_from_folder(Path(full_trail_path), populate_robots_at_times=populate_robots_at_times)
            all_results[(terrain, sigma)].append(metrics)
    return all_results, terrains, sigmas

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


def calculateSuccessRates(all_results, terrains, sigmas):
    # key is terrain. Value is a list of percentage formation success in order of sigmas
    success_rates_dict = {}

    for terrain in terrains:
        success_rate_for_terrain = []
        for sigma in sigmas:
            trials_results = all_results[(terrain, sigma)]
            formation_times = [t["formation_time"] for t in trials_results]
            num_successful_formations = [ft is not None and ft != 0.0 for ft in formation_times]
            success_rate = sum(num_successful_formations)/len(num_successful_formations)
            success_rate_for_terrain.append(success_rate)
        success_rates_dict[terrain] = success_rate_for_terrain

    return success_rates_dict

def plotSuccesRates(all_results, terrains, sigmas):
    # Grab all the success rates
    success_rates_dict = calculateSuccessRates(all_results, terrains, sigmas)
    # Convert sigmas to floats
    sigmas_f = [float(s) for s in sigmas]
    # Create an Axes object for plotting on
    fig, ax = plt.subplots(1,1,dpi=100,figsize=(12, 8))
    # Plot the success rate for each terrain
    for terrain, c in zip(terrains, COLOR_CYCLE[:len(terrains)]):
        ax.plot(sigmas_f, success_rates_dict[terrain],'.-', color=c, markersize=MARKER_SIZE)
    # Create a legend for the terrains
    custom_legend_handles = [Line2D([0],[0], color=c, marker='o', linestyle='', markersize=LEGEND_MARKER_SIZE) for c in COLOR_CYCLE[:len(terrains)]]
    terrain_legend = [TERRAIN_LABELS[terrain] for terrain in terrains]
    ax.legend(custom_legend_handles, terrain_legend)
    # Create labels for x and y
    ax.set_ylabel("Formation Sucess Rate")
    ax.set_xlabel(r'$\sigma$'+" Noise (BL)")
    # Stylize plot so it looks nice
    stylizePlot(ax, sigmas_f)
    # Remove excess whitespace
    fig.tight_layout()
    plt.show()

def calculateFormationTimeStatisitics(all_results, terrains, sigmas):
    # Key is terrain. Value is a list of times in order of sigmas
    averages = {}
    uppers = {}
    lowers = {}
    std_deviations = {}

    for terrain in terrains:
        averages_for_terrain = []
        uppers_for_terrain = []
        lowers_for_terrain = []
        std_deviations_for_terrain = []
        for sigma in sigmas:
            trials_results = all_results[(terrain, sigma)]
            formation_times = [t["formation_time"] for t in trials_results]
            valid_formation_times = [ft for ft in formation_times if ft is not None and ft != 0.0]
            if len(valid_formation_times) > 0:
                averages_for_terrain.append(np.average(valid_formation_times))
                uppers_for_terrain.append(np.max(valid_formation_times))
                lowers_for_terrain.append(np.min(valid_formation_times))
                std_deviations_for_terrain.append(np.std(valid_formation_times))
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

def plotFormationTimes(all_results, terrains, sigmas, plot_std_dev=True):
    # Grab all the formation times
    averages, uppers, lowers, std_deviations = calculateFormationTimeStatisitics(all_results, terrains, sigmas)
    # Convert sigmas to floats
    sigmas_f = [float(s) for s in sigmas]
    # Create axes object
    fig, ax = plt.subplots(1,1,dpi=100,figsize=(12,8))
    # Plot the formation times for each terrain (Mean and standard deviation)
    for terrain, c in zip(terrains, COLOR_CYCLE[:len(terrains)]):
        valid_sigmas = getValidXs(sigmas_f, averages[terrain])
        avg = filterOutNone(averages[terrain])
        if plot_std_dev:
            std_dev = filterOutNone(std_deviations[terrain])
            upper_std_dev = np.array(avg) + np.array(std_dev)
            lower_std_dev = np.array(avg) - np.array(std_dev)
            ax.plot(valid_sigmas, avg, '.-', color=c, markersize=MARKER_SIZE)
            ax.fill_between(valid_sigmas,lower_std_dev,upper_std_dev,alpha=0.25)
        else: # Plot upper and lower ranges if specified
            up = filterOutNone(uppers[terrain])
            low = filterOutNone(lowers[terrain])
            ax.fill_between(valid_sigmas, up, low, alpha=0.2)
    # Create a legend for the terrains
    custom_legend_handles = [Line2D([0],[0], color=c, marker='o', linestyle='', markersize=LEGEND_MARKER_SIZE) for c in COLOR_CYCLE[:len(terrains)]]
    terrain_legend = [TERRAIN_LABELS[terrain] for terrain in terrains]
    ax.legend(custom_legend_handles, terrain_legend)
    # Stylize plot so it looks nice
    stylizePlot(ax, sigmas_f)
    # Remove excess whitespace
    fig.tight_layout()
    plt.show()

folder_path = "/media/egonzalez/Extreme SSD/FlippybotsData/sweep_iros_terrains_sigma_0_to_10_preliminary"
all_results, terrains, sigmas = getAllResults(folder_path, populate_robots_at_times=False)
plotSuccesRates(all_results, terrains, sigmas)
# plotFormationTimes(all_results, terrains, sigmas)