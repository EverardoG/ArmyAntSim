/**@file main.cpp
*/

#include "Box2D/Box2D.h"
#include "SFML/Graphics.hpp"
#include "SFML/Window.hpp"
#include <iostream>

#include "Config.h"
#include "Demo.h"

void default_parameters(config::sConfig& cfg);
void parse_argument(char* argv[], int i, config::sConfig& cfg);
void load_config(std::string filename, config::sConfig& cfg);
void help();

int main(int argc, char* argv[])
{
	//TODO: create a demo class which take the nb of robot, the scene ... in arguments to do all the steps

	/** Prepare the world */
	b2Vec2 Gravity(0.f, 0.f);
	b2World* world = new b2World(Gravity);

	/** */
	bool config_file = false;
	std::string config_file_path;

	config::sConfig configParam;
	default_parameters(configParam);

    for (int i = 1; i < argc; i++) { /* We will iterate over argv[] to get the parameters stored inside.
                                      * Note that we're starting on 1 because we don't need to know the
                                      * path of the program, which is stored in argv[0] */
//        if (i + 1 != argc){ // Check that we haven't finished parsing already
            if (std::string(argv[i]) == "-h" || std::string(argv[i]) == "--help") {
            	printf("tetetete \n");
            	help();
            	return 0;
            }
            else if (i + 1 != argc){
						// The first thing to check is whether a configuration file has been specified
            	if (std::string(argv[i]) == "-cp" || std::string(argv[i]) == "--configuration_path") {
    				// We know the next argument *should* be the filename:
    				config_file_path = std::string(argv[i + 1]);
    				config_file = true;
    				load_config(config_file_path, configParam);
    				break;
            	}
                else{
                	parse_argument(argv, i, configParam);
                }
            }

    		if(configParam.terrain.v_angle > 0){
    			configParam.terrain.v_width = tan(configParam.terrain.v_angle)*2*configParam.terrain.v_height;
    		}

//        }
    }

	Demo myDemo(world, configParam);
	myDemo.init();
	myDemo.demoLoop();
	myDemo.writeResultFile();

   return 0;
}

/** Update the config::sConfig cfg with the default configuration parameter values
*/
void default_parameters(config::sConfig& cfg){

	cfg.terrain.runaway = 8; //7
	cfg.terrain.v_width = 10; //10.2
	cfg.terrain.v_height = 3.5; //8
	cfg.terrain.v_angle = 50/RAD_TO_DEG;

	cfg.simulation.gravity = 0.0;
	cfg.simulation.robot_distance = 3.5;
	cfg.simulation.robot_phase = 0;
	cfg.simulation.robot_delay = 3.25;
	cfg.simulation.robot_initial_posX = 5.20;
	cfg.simulation.robot_initial_posY = 1.0;
	cfg.simulation.nb_robots = 25; //250
	cfg.simulation.bridge_duration = 100;
	cfg.simulation.dissolution_duration = 200;
	cfg.simulation.visualization = true;

	cfg.controller.angle_limit = PI/2;// PI/2
	cfg.controller.bridge_delay = 5; //5
	cfg.controller.walk_delay = 0.5; //Should not be changed
	cfg.controller.time_before_pushing = 1;
	cfg.controller.max_robot_window = 50;
	cfg.controller.stability_condition = 60;

	cfg.robot.body_length = 1.02;
	cfg.robot.fixed_speed = true;
	cfg.robot.speed = 2*PI; //Should not be changed
	cfg.robot.proportional_control = 1.0;

	cfg.window.WINDOW_X_PX = 1920;
	cfg.window.WINDOW_Y_PX = 1080;

	cfg.logfile_name = "exp";
	cfg.logfile_path = "experiments/";

}

void parse_argument(char* argv[], int i, config::sConfig& cfg){

        	// Terrain parameters
			if (std::string(argv[i]) == "-y" || std::string(argv[i]) == "--terrain_type") {
            		cfg.terrain.type = std::string(argv[i + 1]);
					std::cout << "Parsed terrain type" << std::endl;
					std::cout << "Terrain type is: " << cfg.terrain.type << std::endl;
            }
            if (std::string(argv[i]) == "-r" || std::string(argv[i]) == "--terrain_runaway") {
            		cfg.terrain.runaway = atof(argv[i + 1]);
            }
            else if (std::string(argv[i]) == "-vw" || std::string(argv[i]) == "--v_width") {
            		cfg.terrain.v_width = atof(argv[i + 1]);
            }
            else if (std::string(argv[i]) == "-vh" || std::string(argv[i]) == "--v_height") {
            		cfg.terrain.v_height = atof(argv[i + 1]);
            }
            else if (std::string(argv[i]) == "-va" || std::string(argv[i]) == "--v_half_angle") {
            		cfg.terrain.v_angle = atof(argv[i + 1])/RAD_TO_DEG;
            }

        	// Simulation parameters
            else if (std::string(argv[i]) == "-g" || std::string(argv[i]) == "--gravity") {
            		cfg.simulation.gravity = atof(argv[i + 1]);
            }
            else if (std::string(argv[i]) == "-rd" || std::string(argv[i]) == "--robot_distance") {
            		cfg.simulation.robot_distance = atof(argv[i + 1]);
            }
            else if (std::string(argv[i]) == "-rp" || std::string(argv[i]) == "--robot_phase") {
            		cfg.simulation.robot_phase = atof(argv[i + 1])/RAD_TO_DEG;
            }
            else if (std::string(argv[i]) == "-rt" || std::string(argv[i]) == "--robot_delay") {
            		cfg.simulation.robot_delay = atof(argv[i + 1]);
            }
            else if (std::string(argv[i]) == "-rx" || std::string(argv[i]) == "--robot_init_x") {
            		cfg.simulation.robot_initial_posX = atof(argv[i + 1]);
            }
            else if (std::string(argv[i]) == "-ry" || std::string(argv[i]) == "--robot_init_y") {
            		cfg.simulation.robot_initial_posY = atof(argv[i + 1]);
            }
            else if (std::string(argv[i]) == "-nb_r" || std::string(argv[i]) == "--number_robots") {
            		cfg.simulation.nb_robots = atoi(argv[i + 1]);
            }
            else if (std::string(argv[i]) == "-t" || std::string(argv[i]) == "--bridge_duration") {
            		cfg.simulation.bridge_duration = atof(argv[i + 1]);
            }
            else if (std::string(argv[i]) == "-td" || std::string(argv[i]) == "--dissolution_duration") {
            		cfg.simulation.dissolution_duration = atof(argv[i + 1]);
            }
            else if (std::string(argv[i]) == "-vz" || std::string(argv[i]) == "--enable_visualization") {
            		if (std::string(argv[i+1]) == "true"){
            			cfg.simulation.visualization = true;
            		}
            		else if (std::string(argv[i+1]) == "false"){
            			cfg.simulation.visualization = false;
            		}
            		else{
            			std::cout<< "the argument should be either 'true' or 'false'"<<std::endl;
            		}
            }

			// Controller parameters
			else if (std::string(argv[i]) == "-a" || std::string(argv[i]) == "--limit_angle") {
				cfg.controller.angle_limit = atof(argv[i + 1]);
			}
			else if (std::string(argv[i]) == "-bt" || std::string(argv[i]) == "--bridge_delay") {
				cfg.controller.bridge_delay = atof(argv[i + 1]);
			}
			else if (std::string(argv[i]) == "-wt" || std::string(argv[i]) == "--walk_delay") {
				cfg.controller.walk_delay = atof(argv[i + 1]);
			}
			else if (std::string(argv[i]) == "-pt" || std::string(argv[i]) == "--pushing_delay") {
				cfg.controller.time_before_pushing = atof(argv[i + 1]);
			}
			else if (std::string(argv[i]) == "-mr" || std::string(argv[i]) == "--max_robots") {
				cfg.controller.max_robot_window = atoi(argv[i + 1]);
			}
			else if (std::string(argv[i]) == "-st" || std::string(argv[i]) == "--stability_condition") {
				cfg.controller.stability_condition = atof(argv[i + 1]);
			}

			// Robots parameters
			else if (std::string(argv[i]) == "-bl" || std::string(argv[i]) == "--body_length") {
				cfg.robot.body_length = atof(argv[i + 1]);
				std::cout << cfg.robot.body_length << std::endl;
			}
			else if (std::string(argv[i]) == "-fs" || std::string(argv[i]) == "--fixed_speed") {
				cfg.robot.fixed_speed = atof(argv[i+1]);
				std::cout << "Robot Fixed Speed: " << cfg.robot.fixed_speed << std::endl;
			}
			else if (std::string(argv[i]) == "-v" || std::string(argv[i]) == "--robot_speed") {
				cfg.robot.speed = atof(argv[i + 1]);
			}
			else if (std::string(argv[i]) == "-kp" || std::string(argv[i]) == "--proportional_control") {
				cfg.robot.speed = atof(argv[i + 1]);
			}

			// Window parameters
			else if (std::string(argv[i]) == "-wx" || std::string(argv[i]) == "--window_x") {
				cfg.window.WINDOW_X_PX = atoi(argv[i + 1]);
				std::cout << cfg.window.WINDOW_X_PX << std::endl;
			}
			else if (std::string(argv[i]) == "-wy" || std::string(argv[i]) == "--window_y") {
				cfg.window.WINDOW_Y_PX = atoi(argv[i + 1]);
			}

			// File parameters
			else if (std::string(argv[i]) == "-fp" || std::string(argv[i]) == "--file_path") {
				cfg.logfile_path = std::string(argv[i + 1]);
			}
			else if (std::string(argv[i]) == "-fn" || std::string(argv[i]) == "--file_name") {
				cfg.logfile_name = std::string(argv[i + 1]);
			}

        std::cout << argv[i] << " ";
}


/** Function called when launch argument requires help. Print all the possible commands
*/
void help(){
	std::cout << "Launch a simulation for the ant bridge formation" << std::endl;
	std::cout << "Usage: Simulation_v2 [parameters]" << std::endl;
	std::cout << "Parameters:" << std::endl;
	std::cout << "	-cp PATH	Give the path of the configuration file (--configuration_path) \n" << std::endl;

	std::cout << "	Terrain parameters:" << std::endl;
	std::cout << "	-r DIST		Sets the length of the v-terrain runaway relatively to the robot body length (--terrain_runaway, default = 7) " << std::endl;
	std::cout << "	-vw WIDTH	Sets the width of the v shape relatively to the robot body length (--v_width, default = 10) " << std::endl;
	std::cout << "	-vh HEIGHT	Sets the height of the v shape relatively to the robot body length (--v_height, default = 8) \n" << std::endl;
	std::cout << "	-va DEG		Sets the half-angle of the v shape in deg, when angle is set > 0 the width is not taken into account (--v_height, default = 0) \n" << std::endl;

	std::cout << "	Simulation parameters:" << std::endl;
	std::cout << "	-g VALUE	Sets the gravity (--gravity, default = 0.0) " << std::endl;
	std::cout << "	-rd BL		Sets the distance between the creation of two successive robots in body length (--robot_distance, default = 1.1) " << std::endl;
	std::cout << "	-rp RAD		Sets the phase shift between two successive robots in rad (--robot_phase, default = 0) " << std::endl;
	std::cout << "	-rt SEC		Sets the delay between the creation of two successive robots in s (--robot_delay, default = 3.0) " << std::endl;
	std::cout << "	-rx BL		Sets the initial x distance of the robot from the V start (--robot_init_x, default = 10) " << std::endl;
	std::cout << "	-ry POS		Sets the initial y position of the robot (--robot_init_y, default = 10) " << std::endl;
	std::cout << "	-nb_r VALUE	Sets the number of robots for the whole simulation (--number_robots, default = 10) " << std::endl;
	std::cout << "	-t  SEC		Sets the duration of the bridge part of the simulation in s (--simulation_duration, default = 300) " << std::endl;
	std::cout << "	-td SEC		Sets the duration of the dissolution part of the simulation in s (--dissolution_duration, default = 300) " << std::endl;
	std::cout << "	-vz BOOL	Has to be false to disable the visualization  (--enable_visualization, default = true) \n" << std::endl;

	std::cout << "	Controller parameters:" << std::endl;
	std::cout << "	-a  RAD		Sets the minimum angle before the robot is allowed to grab (--limit_angle, default = PI/2) " << std::endl;
	std::cout << "	-bt SEC		Sets the pause delay in the bridge state in s (--bridge_delay, default = 5.0) " << std::endl;
	std::cout << "	-wt SEC		Sets the pause delay in the walking state in s (--walk_delay, default = 0.5) " << std::endl;
	std::cout << "	-pt SEC		Sets the maximum duration of the movement before a robot is considered as pushing in s: create a grip (--pushing_delay, default = 5.0) " << std::endl;
	std::cout << "	-mr VALUE	Sets the maximum number of robots in the window (--max_robots, default = 10) " << std::endl;
	std::cout << "	-st SEC		Sets the time after which a bridge is considered stable in s (--stability_condition, default = 10) \n" << std::endl;

	std::cout << "	Robot parameters:" << std::endl;
	std::cout << "	-bl MET		Sets the body length of the robot in m (--body_length, default = 0.82) " << std::endl;
	std::cout << "	-v  SPEED	Sets the rotational speed of the robot in rad/s (--robot_speed, default = 2*PI) " << std::endl;

	std::cout << "	Window parameters:" << std::endl;
	std::cout << "	-wx PX		Sets the width of the window in pixels (--window_x, default = 1920) " << std::endl;
	std::cout << "	-wy PX		Sets the height of the window in pixels (--window_y, default = 1080) " << std::endl;

	std::cout << "	Result parameters:" << std::endl;
	std::cout << "	-fp PATH		Sets the path to write the result files (--file_path, default = log/) " << std::endl;
	std::cout << "	-fn NAME		Sets the name of the result file (--file_name, default = exp_) " << std::endl;
}

void load_config(std::string filename, config::sConfig& cfg){

	config::Configuration configFile;
	configFile.Load(filename);

//	if (m_config.Get("robot_delay", m_delay)    &&
//		m_config.Get("nb_robots",  m_maxRobots)   )
//	{
//		std::cout << "Parameter in configuration file have been loaded." << std::endl;
//	}

	//-------- simulation parameters
	if (configFile.Get("gravity", cfg.simulation.gravity)    &&
		configFile.Get("robot_delay", cfg.simulation.robot_delay)    &&
		configFile.Get("robot_initial_posX", cfg.simulation.robot_initial_posX)    &&
		configFile.Get("robot_initial_posY", cfg.simulation.robot_initial_posY)    &&
		configFile.Get("nb_robots", cfg.simulation.nb_robots)    &&
		configFile.Get("bridge_duration", cfg.simulation.bridge_duration)    &&
		configFile.Get("dissolution_duration", cfg.simulation.dissolution_duration)    &&
		configFile.Get("visualization", cfg.simulation.visualization))
	{
		std::cout << "Simulation parameters in configuration file have been loaded." << std::endl;
	}
	else
	{
		std::cout << "Missing simulation parameter in configuration file." << std::endl;
	}

	//-------- terrain parameters
	if (configFile.Get("runaway", cfg.terrain.runaway)    &&
		configFile.Get("v_width", cfg.terrain.v_width)    &&
		configFile.Get("v_half_angle", cfg.terrain.v_angle)    &&
		configFile.Get("v_height", cfg.terrain.v_height) )
	{
		cfg.terrain.v_angle = cfg.terrain.v_angle/RAD_TO_DEG;
		std::cout << "Terrain's parameters in configuration file have been loaded." << std::endl;
	}
	else
	{
		std::cout << "Missing terrain parameter in configuration file." << std::endl;
	}

	//-------- controller parameters
	if (configFile.Get("angle_limit", cfg.controller.angle_limit)    &&
		configFile.Get("bridge_delay", cfg.controller.bridge_delay)    &&
		configFile.Get("time_before_pushing", cfg.controller.time_before_pushing)    &&
		configFile.Get("max_robot_window", cfg.controller.max_robot_window)    &&
		configFile.Get("stability_condition", cfg.controller.stability_condition)    &&
		configFile.Get("walk_delay", cfg.controller.walk_delay) )
	{
		std::cout << "Terrain's parameters in configuration file have been loaded." << std::endl;
	}
	else
	{
		std::cout << "Missing terrain parameter in configuration file." << std::endl;
	}

	//-------- robot parameters
	if (configFile.Get("body_length", cfg.robot.body_length)    &&
		configFile.Get("speed", cfg.robot.speed))
	{
		std::cout << cfg.robot.body_length << std::endl;
		std::cout << cfg.robot.speed << std::endl;
		std::cout << "Robot's parameters in configuration file have been loaded." << std::endl;
	}
	else
	{
		std::cout << "Missing robot parameter in configuration file." << std::endl;
	}

	//-------- window parameters
	if (configFile.Get("WINDOW_X_PX", cfg.window.WINDOW_X_PX)    &&
		configFile.Get("WINDOW_Y_PX", cfg.window.WINDOW_Y_PX))
	{
		std::cout << "Window's parameters in configuration file have been loaded." << std::endl;
	}
	else
	{
		std::cout << "Missing window parameter in configuration file." << std::endl;
	}

	//-------- result parameters
	if (configFile.Get("logfile_name", cfg.logfile_name)	&&
		configFile.Get("logfile_path", cfg.logfile_path)	)
	{
		std::cout << "Result's parameters in configuration file have been loaded." << std::endl;
	}
	else
	{
		std::cout << "Missing result parameter in configuration file." << std::endl;
	}
}
