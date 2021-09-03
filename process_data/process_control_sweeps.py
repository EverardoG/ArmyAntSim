"""CLI for crawling through results from sweeps and processing that data
"""

import argparse
import os
from pathlib import Path
from utils import METRICS, get_metrics_from_folder

# Parse commandline for input directory for getting results from
parser = argparse.ArgumentParser(description="Process a sweep of the control parameters")
parser.add_argument("top_directory", help="Top level directory containing sweep results")
args = parser.parse_args()

# Get the paths for all the folders containing results
result_dirs = [args.top_directory + folder for folder in os.listdir(args.top_directory)]

# Create overall dictionary containing results
# format: results_dict[(k, offset)] = RobotInfo
# Save directories for any results that couldn't be processed
bad_results = []
results_dict = {}
for result_dir in result_dirs:
    result_folder = result_dir.split('/')[-1]
    k = result_folder.split('_')[1]
    offset = result_folder.split('_')[3]
    try:
        metrics = get_metrics_from_folder(Path(result_dir))
    except FileNotFoundError:
        bad_results.append(result_folder)
    print(f"k: {k} | offset: {offset} | metrics:\n{metrics}")
    results_dict[(k, offset)] = metrics
