/**
 * @file config.h
 *	@brief Implementation of the Configuration class used to parse the configuration file and the sConfig structure gathering the configuration parameters.
 *	@details
 *  @date 26 sept. 2018
 *  @author lucie houel
 *
 */

#ifndef CONFIG_H_
#define CONFIG_H_

#include "constants.h"


namespace config
{

	const double WINDOW_X_PX = 1920; // [pixel]
	const double WINDOW_Y_PX = 1080; // [pixel]

	const double WALL_WIDTH_M = 6.0; // [m]
	const double WALL_HEIGHT_M = 3.0; // [m]



/** @struct configuration parameters describing the terrain
*
*/
struct sTerrain{
	std::string type;
	/**It is either the distance before the eentrance of the obstcle or 1/3 of the length of the ground in the default terrain*/
	double runaway;
	/** */
	double v_width;
	/** */
	double v_height;
	/** */
	double v_angle;
};

struct sSimulation{
	double gravity;
	bool use_delay;
	bool gaussian_delay;
	bool smart_dissolution;
	double robot_delay;
	double robot_distance;
	double robot_phase;
	double robot_initial_posX;
	double robot_initial_posY;
	int nb_robots;
	double bridge_duration;
	double dissolution_duration;
	bool visualization;
};

struct sController{
	double angle_limit;
	double bridge_delay;
	double walk_delay;
	double time_before_pushing;
	int max_robot_window;
	bool infinite_robots;
	double stability_condition;
	float gain;
};

struct sRobot{
	double body_length;
	bool dynamic_speed;
	double speed;
	double proportional_control;
	double wheel_distance;
	double attach_height;
	double wheel_radius;
	float torque;
};

struct sWindow{
	int WINDOW_X_PX ;
	int WINDOW_Y_PX;
};

struct sConfig{
	struct sTerrain terrain;
	struct sSimulation simulation;
	struct sController controller;
	struct sRobot robot;
	struct sWindow window;

	std::string logfile_name;
	std::string logfile_path;
};


class Configuration
{
public:
    // clear all values
    void Clear();

    // load a configuration file
    bool Load(const std::string& File);

    // check if value associated with given key exists
    bool Contains(const std::string& key) const;

    // get value associated with given key
    bool Get(const std::string& key, std::string& value) const;
    bool Get(const std::string& key, int&    value) const;
    bool Get(const std::string& key, long&   value) const;
    bool Get(const std::string& key, double& value) const;
    bool Get(const std::string& key, bool&   value) const;

private:
    // the container
    std::map<std::string,std::string> data;

    // remove leading and trailing tabs and spaces
    static std::string Trim(const std::string& str);
};

//extern sConfig sConfig;
};
#endif /* CONFIG_H_ */
