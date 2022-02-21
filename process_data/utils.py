"""Utility functions for analyzing data
"""

from pathlib import Path
from typing import Tuple, List, Dict
import numpy as np
import matplotlib.pyplot as plt

METRICS = ["num_robots", "cob", "com", "formation_time", "dissolution_time", "travel_time", "average_travel_time", "box_width", "box_height", "lever_ratio_width", "lever_ratio_height"]

class RobotInfo:
    """Container for robot info stored in _bridge.txt
    """
    def __init__(self, timestamp: int, _id: int, position: np.ndarray, angle: float, joint1: np.ndarray, joint2: np.ndarray):
        # angle is in radians
        # not sure what joint1 and joint2 actually represent
        self.id = _id
        self.position = position
        self.angle = angle
        self.joint1 = joint1
        self.joint2 = joint2
        self.wheel_radius = 0.25
        self.body_length = 1.02
        self.timestamp = timestamp
        self.jointpos1, self.jointpos2 = self.calculate_joint_positions()
        self.joint1_box = get_circle_bounding_box(self.jointpos1, self.wheel_radius)
        self.joint2_box = get_circle_bounding_box(self.jointpos2, self.wheel_radius)
        self.box = self.get_bounding_box()

    def get_bounding_box(self)->np.ndarray:
        """Get the bounding box for the robot
        """
        # print("New robot:")
        # print(f"  timestamp: {self.timestamp}")
        # print(f"  position: {self.position}")
        # print(f"  angle: {self.angle}")
        # print(f"  jointpos1: {self.jointpos1}")
        # print(f"  jointpos2: {self.jointpos2}")
        #
        # print(f"   boundboxjoint1:\n{self.joint1_box}")
        # print(f"   boundboxjoint2:\n{self.joint2_box}")
        joint_boxes = np.vstack((self.joint1_box, self.joint2_box))
        left = min(joint_boxes[:,0])
        right = max(joint_boxes[:,0])
        bottom = max(joint_boxes[:,1])
        top = min(joint_boxes[:,1])
        box =  np.array([
            [left, top],
            [left, bottom],
            [right, bottom],
            [right, top],
            [left, top]
        ])
        # print(f"   overallbox:\n{box}")
        return box

    def calculate_joint_positions(self)->Tuple[np.ndarray, np.ndarray]:
        """Calculate the joint positions for the flippy robot
        """
        center_to_wheel = 0.01 + self.wheel_radius
        diff_vec = [center_to_wheel * np.cos(self.angle), center_to_wheel * np.sin(self.angle) ]
        jointpos1 = self.position + diff_vec
        jointpos2 = self.position - diff_vec
        return jointpos1, jointpos2

    def plot(self, ax: plt.Axes, flip_colors: bool = False, plot_joint_boxes: bool = False, plot_full_box: bool = False):
        """Plot a robot
        y-axis is flipped to match simulator
        """
        # Create circles for joints
        joint1_color = 'r'
        joint2_color = 'b'
        if flip_colors:
            joint1_color = 'b'
            joint2_color = 'r'
        jointpos1_tuple = (self.jointpos1[0], -self.jointpos1[1])
        joint1_circle = plt.Circle(jointpos1_tuple, self.wheel_radius, color=joint1_color, linewidth=None)
        jointpos2_tuple = (self.jointpos2[0], -self.jointpos2[1])
        joint2_circle = plt.Circle(jointpos2_tuple, self.wheel_radius, color=joint2_color, linewidth=None)

        # Create line for body
        bodyline = np.vstack((jointpos1_tuple, jointpos2_tuple))
        # print(f"bodyline:\n{bodyline}")

        # Plot
        ax.plot(bodyline[:,0], bodyline[:,1], color='black')
        if plot_joint_boxes:
            ax.plot(self.joint1_box[:,0], -self.joint1_box[:,1], linewidth=0.5)
            ax.plot(self.joint2_box[:,0], -self.joint2_box[:,1], linewidth=0.5)
        if plot_full_box:
            ax.plot(self.box[:,0], -self.box[:,1], linewidth=0.5)
        ax.add_patch(joint1_circle)
        ax.add_patch(joint2_circle)

def get_circle_bounding_box(position: np.ndarray, radius: float)->np.ndarray:
    """Get the bounding box for a circle
    """
    left = position[0] - radius
    right = position[0] + radius
    top = position[1] - radius
    bottom = position[1] + radius
    box =  np.array([
        [left, top],
        [left, bottom],
        [right, bottom],
        [right, top],
        [left, top]
    ])
    return box

def get_robots_in_initial_bridge(bridge_file: Path)->List[RobotInfo]:
    """Gets all the robots in the initial bridge from a _bridge.txt file

    Args:
        bridge_file (Path): directory for the _bridge.txt file

    Returns:
        List[RobotInfo]: list of robots in the initial bridge
    """
    # Open up the file
    with open(bridge_file) as fp:
        # Grab the initial timestamp for the bridge and initialize
        # the container for robot info
        line = fp.readline()
        initial_bridge_timestamp = line.split(";")[0]
        robots = []
        while line.split(";")[0] == initial_bridge_timestamp:
            info = line.split(";")
            robot_info = RobotInfo(
                    timestamp = int(info[0]),
                    _id = int(info[1]),
                    position = np.array([float(info[2]), float(info[3])]),
                    angle = float(info[4]),
                    joint1 = np.array([float(info[4]), float(info[5])]),
                    joint2 = np.array([float(info[6]), float(info[7])])
                    )
            robots.append(robot_info)
            line = fp.readline()
        return robots

def get_robots_from_important_times(bridge_file: Path, finished_travel_timestamp: int, full_sim_timestamp: int):
    """ I know that the first row of the file is when the bridge initially forms
    I need to know at what point the robots are finished travelling so that I can mark that
    as the point where the bridge begins to dissolve. That timestamp helps me know what robots
    represent the final bridge
    I just need to look at the final row to know which robots were left in bridge state at the very end of the simulation
    All robots in the bridge file are in a BRIDGE state
    """
    # Grab the final time ahead-of-time
    with open(bridge_file) as fp:
        lines = fp.readlines()
        final_line = lines[-1]
        final_time = int(final_line.split(";")[0])
        # for line in lines:
        #     print(line.split(";")[0])

    # Open up the file
    with open(bridge_file) as fp:
        # Grab the robots that form the initial bridge
        line = fp.readline()
        initial_bridge_timestamp = line.split(";")[0]
        initial_bridge_robots = []
        while line.split(";")[0] == initial_bridge_timestamp:
            info = line.split(";")
            robot_info = RobotInfo(
                    timestamp = int(info[0]),
                    _id = int(info[1]),
                    position = np.array([float(info[2]), float(info[3])]),
                    angle = float(info[4]),
                    joint1 = np.array([float(info[4]), float(info[5])]),
                    joint2 = np.array([float(info[6]), float(info[7])])
                    )
            initial_bridge_robots.append(robot_info)
            line = fp.readline()
        # print(finished_travel_timestamp)
        # Get the row that has the correct finished_travel_timestamp or a timestamp
        # that comes right before it.
        # closest_finished_travel_timestamp = 0
        while int(line.split(";")[0]) < finished_travel_timestamp:
             line = fp.readline()
        closest_finished_travel_timestamp = int(line.split(";")[0])
        final_bridge_robots = []
        while int(line.split(";")[0]) == closest_finished_travel_timestamp:
            info = line.split(";")
            robot_info = RobotInfo(
                    timestamp = int(info[0]),
                    _id = int(info[1]),
                    position = np.array([float(info[2]), float(info[3])]),
                    angle = float(info[4]),
                    joint1 = np.array([float(info[4]), float(info[5])]),
                    joint2 = np.array([float(info[6]), float(info[7])])
                    )
            final_bridge_robots.append(robot_info)
            line = fp.readline()
        # Grab the final time
        while int(line.split(";")[0]) < final_time:
            line = fp.readline()
        # If that final time is numerically very close to the final time of the simulator
        # Then it means that the rows with this timestamp accurately represent the final
        # bridge
        # If it isn't accurate, then it doesn't accurately represent the final bridge
        final_dissolved_robots = []
        while  int(line.split(";")[0]) == full_sim_timestamp:
            info = line.split(";")
            robot_info = RobotInfo(
                    timestamp = int(info[0]),
                    _id = int(info[1]),
                    position = np.array([float(info[2]), float(info[3])]),
                    angle = float(info[4]),
                    joint1 = np.array([float(info[4]), float(info[5])]),
                    joint2 = np.array([float(info[6]), float(info[7])])
                    )
            final_dissolved_robots.append(robot_info)
            line = fp.readline()

    return {
        "initial_bridge_robots": initial_bridge_robots,
        "final_bridge_robots" : final_bridge_robots,
        "final_dissolved_robots" : final_dissolved_robots
    }

def calculate_COM_from_robots(robots: List[RobotInfo])->np.ndarray:
    """Calculates center of mass from robots

    Args:
        robots (List[RobotInfo]): list of robots you want the center of mass of

    Returns:
        np.ndarray: 1x2 array representing COM as [x,y]
    """
    robot_positions_list = [np.expand_dims(robot.position, axis=0) for robot in robots]
    robot_positions_arr = np.vstack(robot_positions_list)
    position_avg = np.average(robot_positions_arr, axis=0)
    return position_avg

def calculate_bounding_box_from_robots(robots: List[RobotInfo])->np.ndarray:
    """Calculate the bounding box around all the robots

    Args:
        robots (List[RobotInfo]): list of robots you want the bounding box of

    Returns:
        np.ndarray: 4x2 array representing bounding box with xy coordinates
            Row 1: Top-Left
            Row 2: Bottom-Left
            Row 3: Top-Right
            Row 4: Bottom-Right
    """
    bounding_boxes = [robot.get_bounding_box() for robot in robots]
    bounding_boxes_np = np.vstack(bounding_boxes)
    # print("robot bounding boxes:")
    # print(bounding_boxes_np)
    left = np.min(bounding_boxes_np[:,0])
    right = np.max(bounding_boxes_np[:,0])
    top = np.min(bounding_boxes_np[:,1]) # remember y is flipped
    bottom = np.max(bounding_boxes_np[:,1])
    box = np.array([
        [left, top],
        [left, bottom],
        [right, bottom],
        [right, top],
        [left, top]
        ])
    # print(box)
    return box

def calculate_center_of_bounding_box(bounding_box: np.ndarray)->np.ndarray:
    """Calculate the center of a bounding box for robots

    Args:
        bounding_box (np.ndarray): 4x2 array representing the bounding box with xy coordinates
            Row 1: Top-Left
            Row 2: Bottom-Left
            Row 3: Top-Right
            Row 4: Bottom-Right

    Returns:
        np.ndarray: 1x2 array [x,y] center of the bounding box
    """
    return np.average(bounding_box[0:4], axis=0)


def plot_island(ax: plt.Axes):
    ax.plot([0,24.5098], [-10.2, -10.2], color="black", linewidth=0.5)
    ax.plot([15.3, 15.3, 26.52, 26.52, 15.3],[-5.1, -6.12, -6.12, -5.1, -5.1], color="black", linewidth=0.5)

def get_metrics_from_results_file(results_file: Path)->Dict:
    formation_time = None
    travel_time = None
    dissolution_time = None
    num_robots_bridge_final = None
    num_robots_bridge_initial = None
    num_robots_bridge_end = None
    with open(results_file) as fp:
        for line in fp:
            splitline = line.split(" ")
            # print(splitline)
            if len(splitline) > 2:
                # Bridge formation. This is number of seconds for the bridge to form
                # from the start of the simulation
                if splitline[0] == "\tBridge" and splitline[1] == "formation":
                    # print(splitline)
                    formation_time = float(splitline[4])
                # Bridge travel. This is numebr of seconds from start of simulation
                # to the end of the travel time
                elif splitline[2] == "Bridge":
                    # print(splitline)
                    travel_time_temp = float(splitline[6]) - formation_time
                    if travel_time_temp > 0.0:
                        travel_time = travel_time_temp
                # Bridge dissolution. This is seoncds from end of travel to last
                # robot leaving the sim
                elif splitline[0] == "\tBridge" and splitline[1] == "dissolution":
                    # print(f"splitline:{splitline}")
                    try:
                        dissolution_time = float(splitline[4]) - travel_time - formation_time
                        # if float(splitline[4]) == 0.0:
                        #     print("ZERO DISSOLUTION", dissolution_time)
                    except:
                        pass
                # Grab the early termination flags
                elif splitline[-2] == "m_stacking:":
                    m_stacking = splitline[-1][0]
                elif splitline[-2] == "m_towering:":
                    m_towering = splitline[-1][0]
                elif splitline[-2] == "m_simulationStuck:":
                    m_simulationStuck = splitline[-1][0]
                elif splitline[-2] == "m_tooLongDissolution:":
                    m_tooLongDissolution = splitline[-1][0]
                # Grab the number of robots in bridge state at different times
                elif len(splitline) >= 4 and splitline[-4] == "bridge" and splitline[-3] == "state":
                    # print("FOUND BRIDGE STATE")
                    if splitline[-2] == "initial:":
                        num_robots_bridge_initial = int(splitline[-1])
                    elif splitline[-2] == "final:":
                        num_robots_bridge_final = int(splitline[-1])
                elif splitline[-1][:4] == "end:":
                    num_robots_bridge_end = int(splitline[-1][4:-1])
    if num_robots_bridge_final is not None and num_robots_bridge_final > 0:
        percent_dissolution = 100*(num_robots_bridge_final-num_robots_bridge_end)/num_robots_bridge_final
    else:
        percent_dissolution = None
    num_robots_travelled = 9 # 10 robots total, and travel state is triggered when 1st robot passes goal
    if travel_time is not None:
        average_travel_time = travel_time/num_robots_travelled
    else:
        average_travel_time = None
    # if dissolution_time < 0.0:
    #     print("FOUND ZERO")
    metrics = {
        "formation_time": formation_time,
        "travel_time": travel_time,
        "dissolution_time": dissolution_time,
        "average_travel_time": average_travel_time,
        "m_stacking": m_stacking,
        "m_towering": m_towering,
        "m_simulationStuck": m_simulationStuck,
        "m_tooLongDissolution": m_tooLongDissolution,
        "num_robots_bridge_initial": num_robots_bridge_initial,
        "num_robots_bridge_final": num_robots_bridge_final,
        "num_robots_bridge_end": num_robots_bridge_end,
        "percent_dissolution": percent_dissolution
        }
    return metrics

def get_metrics_from_folder(results_folder: Path, populate_robots_at_times: bool = True)->Dict:
    metrics = {
        "num_robots": None,
        "bounding_box" : None,
        "box_width" : None,
        "box_height" : None,
        "cob" : None,
        "com" : None,
        "lever_ratio_width" : None,
        "lever_ratio_height" : None,
        "robots" : None,
        "formation_time": None,
        "travel_time": None,
        "dissolution_time": None,
        "average_travel_time": None,
        "m_stacking": None,
        "m_towering": None,
        "m_simulationStuck": None,
        "m_tooLongDissolution": None,
        "num_robots_bridge_initial": None,
        "num_robots_bridge_final": None,
        "num_robots_bridge_end": None,
        "percent_dissolution": None
    }
    if (results_folder/"_bridge.txt").exists():
        robots = get_robots_in_initial_bridge(results_folder/"_bridge.txt")
        com = calculate_COM_from_robots(robots)
        box = calculate_bounding_box_from_robots(robots)
        box_width = abs(np.max(box[:,0]) - np.min(box[:,0]))
        box_height = abs(np.max(box[:,1])- np.min(box[:,1]))
        cob = calculate_center_of_bounding_box(box)
        com_to_cob = cob - com
        lever_ratio_width = com_to_cob[0]/(box_width/2)
        lever_ratio_height = com_to_cob[1]/(box_height/2)
        metrics["num_robots"] = len(robots)
        metrics["bounding_box"] = box[0:4]
        metrics["box_width"] = box_width
        metrics["box_height"] = box_height
        metrics["cob"] = cob
        metrics["com"] = com
        metrics["lever_ratio_width"] = lever_ratio_width
        metrics["lever_ratio_heigth"] = lever_ratio_height
        metrics["robots"] = robots

    if (results_folder/"_result.txt").exists():
        results_file_metrics = get_metrics_from_results_file(results_folder/"_result.txt")
        metrics.update(results_file_metrics)
        if results_file_metrics["formation_time"] is not None and results_file_metrics["travel_time"] is not None:
            travel_time_ends_timestamp = int((results_file_metrics["formation_time"] + results_file_metrics["travel_time"]) * 60)
            full_sim_timestamp = travel_time_ends_timestamp + int(results_file_metrics["dissolution_time"]*60)
            if populate_robots_at_times and (results_folder/"_bridge.txt").exists():
                metrics["robots_at_times"] = get_robots_from_important_times(results_folder/"_bridge.txt", travel_time_ends_timestamp, full_sim_timestamp)
            else:
                metrics["robots_at_times"] = None
        else:
            metrics["robots_at_times"] = None

    return metrics

def visualize_initial_bridge(results_folder: Path)->None:
    results = get_metrics_from_folder(results_folder)
    cob = results["cob"]
    com = results["com"]
    robots = results["robots"]
    fig, ax = plt.subplots()
    ax.plot(cob[0], -cob[1], 'c.', markersize=5)
    ax.plot(com[0], -com[1], 'm.', markersize=5)
    plot_island(ax)
    overall_box = calculate_bounding_box_from_robots(robots)
    ax.plot(overall_box[:,0], -overall_box[:,1], linewidth=0.5)
    for robot in robots:
        robot.plot(ax, plot_joint_boxes=True, plot_full_box=True)
    ax.plot(cob[0], -cob[1], 'c.', markersize=5)
    ax.plot(com[0], -com[1], 'm.', markersize=5)
    ax.set_xlim(0, 24.5098)
    ax.set_ylim(-12,0)
    ax.set_aspect('equal')
    ax.legend(["Center of box","Center of mass"])
    plt.show()

if __name__ == "__main__":
    # folder_path = Path("/home/egonzalez/data/redo_island_2/sweep_k_0.1_to_20_robot_speed_0_to_5.60/k_1.98_offset_3.83")
    folder_path = Path("/media/egonzalez/Extreme SSD/FlippybotsData/sweep_iros_terrains_sigma_0_to_10/pit_0/0")
    visualize_initial_bridge(folder_path)
    metrics_dict = get_metrics_from_folder(folder_path)
    desired_metrics = ["num_robots", "cob", "com", "formation_time", "dissolution_time", "travel_time", "average_travel_time", "box_width", "box_height", "lever_ratio_width", "lever_ratio_height"]
    for metric_name in desired_metrics:
        print(f"{metric_name}: {metrics_dict[metric_name]}")
