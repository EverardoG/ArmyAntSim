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

STD_DEV = 0
FULL_RANGE = 1
NONE = 3

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


def calculateSuccessRates(all_results, terrains, sigmas, which):
    # key is terrain. Value is a list of percentage formation success in order of sigmas
    success_rates_dict = {}

    for terrain in terrains:
        success_rate_for_terrain = []
        for sigma in sigmas:
            trials_results = all_results[(terrain, sigma)]
            if which == FORMATION:
                times = [t["formation_time"] for t in trials_results]
            elif which == TRAVEL:
                times = [t["travel_time"] for t in trials_results]
            num_successful = [ft is not None and ft != 0.0 for ft in times]
            success_rate = sum(num_successful)/len(num_successful)
            success_rate_for_terrain.append(success_rate)
        success_rates_dict[terrain] = success_rate_for_terrain

    return success_rates_dict

def plotSuccesRates(ax, all_results, terrains, sigmas, which):
    # Grab all the success rates
    success_rates_dict = calculateSuccessRates(all_results, terrains, sigmas, which)
    # Convert sigmas to floats
    sigmas_f = [float(s) for s in sigmas]
    # Plot the success rate for each terrain
    for terrain, c in zip(terrains, COLOR_CYCLE[:len(terrains)]):
        ax.plot(sigmas_f, success_rates_dict[terrain],'.-', color=c, markersize=MARKER_SIZE)
    # Create a legend for the terrains
    custom_legend_handles = [Line2D([0],[0], color=c, marker='o', linestyle='', markersize=LEGEND_MARKER_SIZE) for c in COLOR_CYCLE[:len(terrains)]]
    terrain_legend = [TERRAIN_LABELS[terrain] for terrain in terrains]
    ax.legend(custom_legend_handles, terrain_legend)
    # Create labels for x and y
    if which == FORMATION:
        ax.set_ylabel("Formation Sucess Rate")
    elif which == TRAVEL:
        ax.set_ylabel("Utility Sucess Rate")
    ax.set_xlabel(r'$\sigma$'+" Noise (BL)")
    # Stylize plot so it looks nice
    stylizePlot(ax, sigmas_f)
    return None

def plotFormationSuccessRates(ax, all_results, terrains, sigmas):
    return plotSuccesRates(ax, all_results, terrains, sigmas, FORMATION)

def plotTravelSucessRates(ax, all_results, terrains, sigmas):
    return plotSuccesRates(ax, all_results, terrains, sigmas, TRAVEL)

def calculatePhaseTimeStatisitics(all_results, terrains, sigmas, which):
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
            if which == FORMATION:
                times = [t["formation_time"] for t in trials_results]
            elif which == TRAVEL:
                times = [t["travel_time"] for t in trials_results]
            valid_times = [t for t in times if t is not None and t != 0.0]
            if len(valid_times) > 0:
                averages_for_terrain.append(np.average(valid_times))
                uppers_for_terrain.append(np.max(valid_times))
                lowers_for_terrain.append(np.min(valid_times))
                std_deviations_for_terrain.append(np.std(valid_times))
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

def plotTimeStatistics(ax, all_results, terrains, sigmas, which, plot_range):
    # Grab all the formation times
    averages, uppers, lowers, std_deviations = calculatePhaseTimeStatisitics(all_results, terrains, sigmas, which)
    # Convert sigmas to floats
    sigmas_f = [float(s) for s in sigmas]
    # Plot the formation times for each terrain (Mean and standard deviation)
    for terrain, c in zip(terrains, COLOR_CYCLE[:len(terrains)]):
        valid_sigmas = getValidXs(sigmas_f, averages[terrain])
        avg = filterOutNone(averages[terrain])
        ax.plot(valid_sigmas, avg, '.-', color=c, markersize=MARKER_SIZE)
        if plot_range == STD_DEV:
            std_dev = filterOutNone(std_deviations[terrain])
            upper_std_dev = np.array(avg) + np.array(std_dev)
            lower_std_dev = np.array(avg) - np.array(std_dev)
            ax.fill_between(valid_sigmas,lower_std_dev,upper_std_dev,alpha=0.25)
        elif plot_range == FULL_RANGE: # Plot upper and lower ranges if specified
            up = filterOutNone(uppers[terrain])
            low = filterOutNone(lowers[terrain])
            ax.fill_between(valid_sigmas, up, low, alpha=0.2)
        elif plot_range == NONE:
            pass
    # Create a legend for the terrains
    custom_legend_handles = [Line2D([0],[0], color=c, marker='o', linestyle='', markersize=LEGEND_MARKER_SIZE) for c in COLOR_CYCLE[:len(terrains)]]
    terrain_legend = [TERRAIN_LABELS[terrain] for terrain in terrains]
    ax.legend(custom_legend_handles, terrain_legend)
    # Create labels for x and y
    if which == FORMATION:
        ax.set_ylabel("Formation Time (s)")
    elif which == TRAVEL:
        ax.set_ylabel("Utility Time (s)")
    ax.set_xlabel(r'$\sigma$'+" Noise (BL)")
    # Set y limits to make plot more readable
    ax.set_ylim([0,500])
    # Stylize plot so it looks nice
    stylizePlot(ax, sigmas_f)
    return None

def plotFormationTimes(ax, all_results, terrains, sigmas, plot_range=STD_DEV):
    return plotTimeStatistics(ax, all_results, terrains, sigmas, FORMATION, plot_range)

def plotTravelTimes(ax, all_results, terrains, sigmas, plot_range=STD_DEV):
    return plotTimeStatistics(ax, all_results, terrains, sigmas, TRAVEL, plot_range)

def generateSigmaFigure():
    fig, axs = plt.subplots(1,4,dpi=100,figsize=(24,4))
    plotFormationSuccessRates(axs[0], all_results, terrains, sigmas)
    plotTravelSucessRates(axs[1],all_results, terrains, sigmas)
    plotFormationTimes(axs[2], all_results, terrains, sigmas)
    plotTravelTimes(axs[3], all_results, terrains, sigmas)
    fig.tight_layout()
    fig.subplots_adjust(left=0.04)
    return fig, axs

if __name__ == "__main__":
    folder_path = "/media/egonzalez/Extreme SSD/FlippybotsData/sweep_iros_terrains_sigma_0_to_10"
    all_results, terrains, sigmas = getAllResults(folder_path, populate_robots_at_times=False)
    fig, axs = generateSigmaFigure()
    plt.show()