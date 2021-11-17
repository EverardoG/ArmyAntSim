from utils import *
import pprint
PP = pprint.PrettyPrinter(indent=4)

test_dir = "/home/egonzalez/data/redo_island_2/sweep_k_0.1_to_20_robot_speed_0_to_5.60/k_16.1_offset_0.59/"
metrics = get_metrics_from_folder(Path(test_dir))
PP.pprint(metrics)
