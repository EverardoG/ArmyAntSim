/**@file main.cpp
*/

#include "Box2D/Box2D.h"
#include "SFML/Graphics.hpp"
#include "SFML/Window.hpp"
#include <iostream>
#include "tclap/CmdLine.h"

#include "Config.h"
#include "Demo.h"
#include <list>
#include <any>

void load_config(std::string filename, config::sConfig& cfg);
void help();
int print_help_and_exit(TCLAP::CmdLineInterface& cmd);

using namespace TCLAP;

// class CustomHelpOutput : public TCLAP::StdOutput {
// 	public:
// 		virtual void usage(TCLAP::CmdLineInterface& _cmd) override {
// 			std::cout << "my program is called " << _cmd.getProgramName() << std::endl;
// 		}
// };

// inline void CmdLine::setOutput(CmdLineOutput* co)
// {
// 	if (!_userSetOutput)
// 		delete _output;
// 	_userSetOutput = true;
// 	_output = co;
// }

int main(int argc, char* argv[])
{
	//TODO: create a demo class which take the nb of robot, the scene ... in arguments to do all the steps

	/** Prepare the world */
	b2Vec2 Gravity(0.f, 0.f);
	b2World* world = new b2World(Gravity);

	/** */
	bool config_file = false;
	std::string config_file_path;

	config::sConfig cfg;
	// default_parameters(configParam);

	// list of args
	std::list<std::any> arg_list;

	// try {
	// Create the command line object
	CmdLine cmd("Command description message",' ',"0.99", false);
	// cmd.setOutput(new CustomHelpOutput());

	// Check if help swtich is triggered
	SwitchArg   help_arg("h","help", "Print help message", false);
	cmd.add(help_arg);

	// Configuration file. If one is specified, it overrides all other commandline inputs
	ValueArg<std::string> config_path_arg("","configuration_path", "Path of the configuration file. If specified, it overrides all other parameters.", false, "", "PATH string \"\"");
	cmd.add(config_path_arg);

	// Create Terrain parameters
	ValueArg<std::string> terrain_type_arg("y", "terrain_type", "Type of terrain used for simulation.", false, "v_terrain", "TERRAIN string \"v_terrain\"");
	ValueArg<float>       terrain_runway_arg("r", "terrain_runway", "Length of the terrain runway in robot body lengths", false, 8.0, "DIST float 8.0");
	ValueArg<float>       v_width_arg("", "v_width","Width of the v shape in v_terrain in robot body lengths", false, 10.0, "WIDTH float 10.0");
	ValueArg<float>       v_height_arg("", "v_height", "Height of the v shape in v_terrain in robot body lengths", false, 3.5, "HEIGHT float 3.5");
	ValueArg<float>       angle_arg("", "v_half_angle", "Half angle of the v shape in v_terrain in robot body lengths. When angle > 0, width is not taken into account", false, 50/RAD_TO_DEG, "DEG float 50/RAD_TO_DEG");

	// Create Simulation Parameters
	ValueArg<float>       gravity_arg("g", "gravity", "Magnitude of gravity in simulation in m/s^2", false, 0.0, "ACCEL float 0.0");
	ValueArg<float>       robot_distance_arg("", "robot_distance", "Distance between the creation of two successive robots in s", false, 3.5, "BL float 3.5");
	ValueArg<float>       robot_phase_arg("","robot_phase", "Phase shift between two successive robots in rad", false, 0.0, "RAD float 0.0");
	ValueArg<float>       robot_delay_arg("","robot_delay", "Delay between the creation of two successive robots in s",false,3.25,"SEC float 3.25");
	ValueArg<float>       robot_init_x_arg("", "robot_init_x", "Initial x distance of the robot from the V start",false,5.2,"BL float 5.2");
	ValueArg<float>       robot_init_y_arg("", "robot_init_y", "Initial y position of the robot",false,0.0,"BL float 0.0");
	ValueArg<int>         num_robots_arg("","number_robots", "Number of robots for the whole simulation", false, 25, "NUM int 25");
	ValueArg<float>       sim_duration_arg("t", "simulation_duration", "Duration of the bridge part of the simulation", false, 100, "SEC float 100");
	ValueArg<float>       dis_duration_arg("", "dissolution_duration", "Duration of the dissolution part of the simulation", false, 200, "SEC float 200");
	ValueArg<bool>        visual_arg("", "enable_visualization", "Play the visualizer for the simulation", false, true, "BOOL bool true");

	// Create Controller Parameters
	ValueArg<float> angle_limit_arg("a", "limit_angle", "Minimum angle before robot is allowed to grab", false, PI/2, "RAD float PI/2");
	ValueArg<float> bridge_delay_arg("", "bridge_delay", "Pause delay in the bridge state in s", false, 5.0, "SEC float 5.0");
	ValueArg<float> walk_delay_arg("","walk_delay", "Pause delay in the walking state in s", false, 0.5,"SEC float 0.5");
	ValueArg<float> push_delay_arg("", "push_delay", "Maximum duration of the movement before a robot is considered as pushing in s. The robot creates a grip after this time.", false, 1.0,"SEC float 1.0");
	ValueArg<int> max_robots_arg("","max_robots", "Maximum number of robots in the window", false, 50, "NUM int 50");
	ValueArg<float> stable_time_arg("", "stability_condition", "Time after which a bridge is considered stable in s", false, 60, "SEC float 60");

	// Create Robots Parameters
	ValueArg<float> body_length_arg("","body_length", "Body length of the robot in m", false, 1.02, "METERS float 1.02");
	ValueArg<float> speed_arg("v", "robot_speed", "Rotational speed of the robot in rad/s", false, 2*PI, "RAD/S float 2*PI");
	ValueArg<bool> fixed_speed_arg("", "fixed_speed", "True: Robots have a fixed speed. False: Each robot has a dynamic speed.", false, true, "BOOL bool true");
	ValueArg<float> kp_arg("", "proportional_control", "Proportional control gain for dynamic speed", false, 1.0, "VALUE float 1.0");
	ValueArg<float> torque_arg("", "torque", "Torque of the robot joints", false, 30.0f, "NEWTONS float 30.0f");

	// Create Window Parameters
	ValueArg<int> window_x_arg("", "window_x", "Width of the window in pixels", false, 1920, "PIX int 1920");
	ValueArg<int> window_y_arg("", "window_y", "Height of the window in pixels", false, 1080, "PIX int 1080");

	// Create File Parameters
	ValueArg<std::string> logfile_path_arg("", "file_path", "Path to write the result files", false, "experiments/log/", "PATH string \"experiments/log/\"");
	ValueArg<std::string> logfile_name_arg("", "file_name", "Prefix for the names of the result files", false, "exp_", "NAME string \"exp_\"");

	// Add Terrain parameters
	cmd.add(terrain_type_arg);
	cmd.add(terrain_runway_arg);
	cmd.add(v_width_arg);
	cmd.add(v_height_arg);
	cmd.add(angle_arg);

	// Add Simulation parameters
	cmd.add(gravity_arg);
	cmd.add(robot_distance_arg);
	cmd.add(robot_phase_arg);
	cmd.add(robot_delay_arg);
	cmd.add(robot_init_x_arg);
	cmd.add(robot_init_y_arg);
	cmd.add(num_robots_arg);
	cmd.add(sim_duration_arg);
	cmd.add(dis_duration_arg);
	cmd.add(visual_arg);

	// Add Controlller parameters
	cmd.add(angle_limit_arg);
	cmd.add(bridge_delay_arg);
	cmd.add(walk_delay_arg);
	cmd.add(push_delay_arg);
	cmd.add(max_robots_arg);
	cmd.add(stable_time_arg);

	// Add Robots parameters
	cmd.add(body_length_arg);
	cmd.add(speed_arg);
	cmd.add(fixed_speed_arg);
	cmd.add(kp_arg);
	cmd.add(torque_arg);

	// Add Window parameters
	cmd.add(window_x_arg);
	cmd.add(window_y_arg);

	// Add file parameters
	cmd.add(logfile_path_arg);
	cmd.add(logfile_name_arg);

	// Parse the command line
	cmd.parse(argc, argv);

	// std::cout << "Terrain type: A " << terrain_type_arg.getValue() << std::endl;

	if (help_arg.getValue()){
		return print_help_and_exit(cmd);
	}

	// Set all the config parameters according to the command line parsing
	cfg.terrain.type = terrain_type_arg.getValue();
	cfg.terrain.runaway = terrain_runway_arg.getValue();
	cfg.terrain.v_width = v_width_arg.getValue();
	cfg.terrain.v_height = v_height_arg.getValue();
	cfg.terrain.v_angle = angle_arg.getValue();

	cfg.simulation.gravity = gravity_arg.getValue();
	cfg.simulation.robot_distance = robot_distance_arg.getValue();
	cfg.simulation.robot_phase = robot_phase_arg.getValue();
	cfg.simulation.robot_delay = robot_delay_arg.getValue();
	cfg.simulation.robot_initial_posX = robot_init_x_arg.getValue();
	cfg.simulation.robot_initial_posY = robot_init_y_arg.getValue();
	cfg.simulation.nb_robots = num_robots_arg.getValue();
	cfg.simulation.bridge_duration = sim_duration_arg.getValue();
	cfg.simulation.dissolution_duration = dis_duration_arg.getValue();
	cfg.simulation.visualization = visual_arg.getValue();

	cfg.controller.angle_limit = angle_limit_arg.getValue();
	cfg.controller.bridge_delay = bridge_delay_arg.getValue();
	cfg.controller.walk_delay = walk_delay_arg.getValue();
	cfg.controller.time_before_pushing = push_delay_arg.getValue();
	cfg.controller.max_robot_window = max_robots_arg.getValue();
	cfg.controller.stability_condition = stable_time_arg.getValue();

	cfg.robot.body_length = body_length_arg.getValue();
	cfg.robot.fixed_speed = fixed_speed_arg.getValue();
	cfg.robot.speed = speed_arg.getValue();
	cfg.robot.proportional_control = kp_arg.getValue();
	cfg.robot.torque = torque_arg.getValue();

	cfg.window.WINDOW_X_PX = window_x_arg.getValue();
	cfg.window.WINDOW_Y_PX = window_y_arg.getValue();

	cfg.logfile_name = logfile_name_arg.getValue();
	cfg.logfile_path = logfile_path_arg.getValue();

	// Follow rule for v half angle
	if(cfg.terrain.v_angle > 0){
    	cfg.terrain.v_width = tan(cfg.terrain.v_angle)*2*cfg.terrain.v_height;
    }

	// Set parameters to config file if one is specified
	if (config_path_arg.getValue() != "") {
		load_config(config_path_arg.getValue(), cfg);
	}

	Demo myDemo(world, cfg);
	myDemo.init();
	myDemo.demoLoop();
	myDemo.writeResultFile();

   return 0;
}

int print_help_and_exit(CmdLineInterface& cmd)
{
	std::cout << "Launch a simulation for the ant bridge formation" << std::endl;
	std::cout << "Usage: ./ArmyAntSim [parameters]" << std::endl;
	std::cout << "Parameters:" << std::endl;
	std::list<Arg*> arg_list = cmd.getArgList(); arg_list.reverse();
	std::vector<std::string> help_vec;
	for (Arg* arg : arg_list) {
		// This part gets the argument flags Ex: -h and --help
		std::string line;
		std::vector<std::string> vec;
		std::stringstream ss(arg->toString());
		while(std::getline(ss,line,' ')) {
			vec.push_back(line);
		}

		// better name
		std::vector<std::string> flags = vec;

		// both a short flag and long flag -> take out the parenthesis from the long flag
		if (flags.size() == 1) {
			flags[0] = flags[0].substr(1,flags[0].size()-2);
		}
		else if (flags.size() == 2) {
			flags[1] = flags[1].substr(1,flags[1].size()-2);
		}

		// This part gets the human-readable INPUT, type, default
		// std::string line2;
		std::string short_ID = arg->shortID();
		if (short_ID.find('<') != std::string::npos) {
			// So we know this is a value argument
			std::string info_string = short_ID.substr(short_ID.find("<")+1, short_ID.find(">")-short_ID.find("<")-1);
			// std::cout << "info_string: " << info_string << std::endl;

			// Split the info string into its components
			std::vector<std::string> vec2;
			std::stringstream ss(info_string);
			while(std::getline(ss,line,' ')) {
				vec2.push_back(line);
			}

			std::string human_readable = vec2[0];
			std::string arg_type = vec2[1];
			std::string arg_default = vec2[2];

			// Figure out what to do for flag in the parentheses at the end
			std::string second_arg = "";
			if (flags.size() == 2) { second_arg = flags[1] + ", "; }

			// Build the help message for this argument
			std::string help_output = flags[0] + " <" + human_readable + ">" + arg->getDescription() + " (" + second_arg + arg_type + ", default = " + arg_default + ")";

			// Shove it in with the others
			help_vec.emplace_back(help_output);
		}
		else {
			// We'll do something else here
			std::string help_output = flags[0] + "<>" + arg->getDescription() + " (" + flags[1] + ")";
			help_vec.emplace_back(help_output);
		}

		// std::cout << "getFlag: " << arg->getFlag() << std::endl;
		// std::cout << "getName: " << arg->getName() << std::endl;
		// std::cout << "getDescription: " << arg->getDescription() << std::endl;
		// std::cout << "isRequired: " << arg->isRequired() << std::endl;
		// std::cout << "toString: " << arg->toString() << std::endl;
		// std::cout << "Short ID: " << arg->shortID() << std::endl;
		// std::cout << "Long ID: " << arg->longID() << std::endl;
		// std::cout << "setBy: " << arg->setBy() << std::endl;
		// // std::cout << arg->getValue() << std::endl;
		// std::cout << "flagStartString: " << arg->flagStartString() << std::endl;
		// std::cout << "nameStartString: " << arg->nameStartString() << std::endl;
	} // end for (Arg* arg : arg_list)

	// Find the length of the longest intro for all the arguments. Intro includes --flag <HUMAN-READABLE-INPUT>
	int longest_intro = 0;
	// std::cout << "longest_intro: " << longest_intro << std::endl;
	for (std::string help_output : help_vec) {
		if (help_output.find(">") > longest_intro) {
			longest_intro = help_output.find(">");
			// std::cout << "longest_intro: " << longest_intro << std::endl;
		}
		// std::cout << help_output << std::endl;
	}

	// Left align all of the descriptions according to the longest intro
	for (std::string help_output : help_vec) {
		// split the string into intro and description
		std::vector<std::string> help_components;
		std::stringstream ss(help_output);
		std::string line;
		while(std::getline(ss,line,'>')) {
			help_components.push_back(line);
		}

		// Check if we need to add back in the ">"
		std::string intro = help_components[0];
		if (intro[intro.length()-1] == '<') {
			intro = intro.substr(0, intro.length() -1);
		}
		else {
			intro = intro + ">";
		}

		// std::string intro = help_components[0] + ">";
		std::string description = help_components[1];

		// Check if this is a key argument and print extra message if relevant
		if (intro == "--") {std::cout << "\n General:" << std::endl;}
		else if (intro == "--configuration_path <PATH>") {std::cout << "\n  Config: " << std::endl;}
		else if (intro == "-y <TERRAIN>") {std::cout << "\n  Terrain:" << std::endl;}
		else if (intro == "--robot_distance <BL>") {std::cout << "\n  Simulation:" << std::endl;}
		else if (intro == "--body_length <METERS>") {std::cout << "\n  Robot:" << std::endl;}
		else if (intro == "--window_x <PIX>") {std::cout << "\n  Window:" << std::endl;}
		else if (intro == "--file_path <PATH>") {std::cout << "\n  File:" << std::endl;}
		// Make the intro longer according to the longest_intro. Add an offset for nicer spacing
		intro =  intro + std::string( longest_intro - intro.size() + 5, ' ');

		// Print the argument help
		std::cout << "    " << intro << description << std::endl;
	}
	std::cout << std::endl;


	return 0;

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
