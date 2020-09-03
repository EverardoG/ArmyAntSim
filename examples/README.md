# Examples

### Requirement:
To run the examples, there is no need to install Box2D lib but the SFML one should be installed on your computer (cf [Installation guide](../../wiki/Installation-guide))

### Description:
The example folder contains some executable to directly run variants of the simulator.

### Usage:
Usage: Simulation_v2 [parameters]
Parameters:
	
	-h 		Help (--help)
	-cp PATH	Give the path of the configuration file (--configuration_path) 

	Terrain parameters:
	-r DIST		Sets the length of the v-terrain runaway relatively to the robot body length (--terrain_runaway, default = 7) 
	-vw WIDTH	Sets the width of the v shape relatively to the robot body length (--v_width, default = 10) 
	-vh HEIGHT	Sets the height of the v shape relatively to the robot body length (--v_height, default = 8) 

	-va DEG		Sets the half-angle of the v shape in deg, when angle is set > 0 the width is not taken into account (--v_height, default = 0) 

	Simulation parameters:
	-g VALUE	Sets the gravity (--gravity, default = 0.0) 
	-rd BL		Sets the distance between the creation of two successive robots in body length (--robot_distance, default = 1.1) 
	-rp RAD		Sets the phase shift between two successive robots in rad (--robot_phase, default = 0) 
	-rt SEC		Sets the delay between the creation of two successive robots in s (--robot_delay, default = 3.0) 
	-rx BL		Sets the initial x distance of the robot from the V start (--robot_init_x, default = 10) 
	-ry POS		Sets the initial y position of the robot (--robot_init_y, default = 10) 
	-nb_r VALUE	Sets the number of robots for the whole simulation (--number_robots, default = 10) 
	-t  SEC		Sets the duration of the bridge part of the simulation in s (--simulation_duration, default = 300) 
	-td SEC		Sets the duration of the dissolution part of the simulation in s (--dissolution_duration, default = 300) 
	-vz BOOL	Has to be false to disable the visualization  (--enable_visualization, default = true) 

	Controller parameters:
	-a  RAD		Sets the minimum angle before the robot is allowed to grab (--limit_angle, default = PI/2) 
	-bt SEC		Sets the pause delay in the bridge state in s (--bridge_delay, default = 5.0) 
	-wt SEC		Sets the pause delay in the walking state in s (--walk_delay, default = 0.5) 
	-pt SEC		Sets the maximum duration of the movement before a robot is considered as pushing in s: create a grip (--pushing_delay, default = 5.0) 
	-mr VALUE	Sets the maximum number of robots in the window (--max_robots, default = 10) 
	-st SEC		Sets the time after which a bridge is considered stable in s (--stability_condition, default = 10) 

	Robot parameters:
	-bl MET		Sets the body length of the robot in m (--body_length, default = 0.82) 
	-v  SPEED	Sets the rotational speed of the robot in rad/s (--robot_speed, default = 2*PI) 
	Window parameters:
	-wx PX		Sets the width of the window in pixels (--window_x, default = 1920) 
	-wy PX		Sets the height of the window in pixels (--window_y, default = 1080) 
	Result parameters:
	-fp PATH		Sets the path to write the result files (--file_path, default = log/) 
	-fn NAME		Sets the name of the result file (--file_name, default = exp_) 
