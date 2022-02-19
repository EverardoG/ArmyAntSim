/**
 *  @file Demo.h
 *	@brief Implementation of the Demo class and the MyContactListener_v2 class.
 *	@details The Demo class gather all the elements of the simulation: the terrain, the robot controller and the robots and implement more abstract simulation methods.
 *	Those methods handle the robot interaction with the terrain and between themselves ie the group behavior.
 *	This is why the robots are only created via the robot controller.
 *	The demo class also synchronized the physics and simulation step.
 *
 *	The MyContactListener_v2 class is used to create a listener on the contact between the physics elements (robots and terrain).
 *  @date 26 sept. 2018
 *  @author lucie houel
 */

#ifndef DEMO_H_
#define DEMO_H_
#include "Box2D/Box2D.h"
#include "SFML/Graphics.hpp"
#include "SFML/Window.hpp"
#include <iostream>
#include "Robot.h"
#include "RobotController.h"
#include "CliffTerrain.h"
#include "FlatTerrain.h"
#include "BoxTerrain.h"
#include "Ramp.h"
#include "V2BLTerrain.h"
#include "VStepper.h"
#include "Vterrain.h"
#include "IslandTerrain.h"
#include "PitTerrain.h"
#include "TerraceTerrain.h"
#include "StepDownTerrain.h"
#include "DefaultTerrain.h"
#include <iostream>
#include <fstream>
#include "Config.h"

#include <random>

enum SimulationState { Formation, Travel, Dissolution, End };

/** @class MyContactListener_v2
 * This class is a listener on the box2D body contacts. In case of contact it calls the RobotController::findContactRobots(contact) method
 * */
class MyContactListener_v2 : public b2ContactListener
{
public:
	MyContactListener_v2(RobotController& robotController):m_robotController(&robotController){};

	void BeginContact(b2Contact* contact){
//		printf("contact \n");
		m_robotController->findContactRobots(contact);
	}
	void EndContact(b2Contact* contact) {
	}

private:
	RobotController* m_robotController;
};

/** @class Demo
 *	The Demo class gather all the elements of the simulation: the terrain, the robot controller and the robots and implement more abstract simulation methods.
 *	Those methods handle the robot interaction with the terrain and between themselves ie the group behavior.
 *	This is why the robots are only created via the robot controller.
 *	It also handle the simulation results by writing them in two distinct files:
 *	 - The overall result file with the summary of the results
 *	 - The bridge file that gather the timestep and position of every robots entering the bridge state. It is updated every time a new robot enter the bridge state.
 * */
class Demo {
public:
	Demo(b2World* m_world, config::sConfig cfg);
	virtual ~Demo();

	bool addRobot();

	/**
	 * Create a new robot after a given delay defined by m_config.simulation.robot_delay (which has been obtained as an argument of the Demo object creation)
	 * until the maximum number of robots have been reached.
	 * The traffic is thus controlled by the initial position and the delay between the robots.
	 * @return false if the robots are stacking (the previous robot too close to the one that will be created)
	 */
	bool addRobotWithDelay();
	/**
	 * Create a new robot when a given distance with the previous robot defined by m_config.simulation.robot_distance has been reached
	 * and the previous robot has an angle corresponding to the desired phase shift.
	 * It does so until the maximum number of robots have been reached.
	 * The traffic is thus controlled by the distance between the robots and the phase shift.
	 * @return true when a new robot is created.
	 */
	bool addRobotWithDistance();

	/**
	 * Create bridge file under m_bridgeFile.
	 * Every line is structured as follow:
	 * Timestamp; robot ID; x coordinate; y coordinate; angle; current joint x; current joint y; previous joint x; previous joint y; it entry; age
	 */
	void createBridgeFile();

	/**
	 *Simulation loop that handle the synchronization between the physics and the controller step.
	 *The visualization is activated or deactivated via m_config.simulation.visualization.
	 *The Simulation is composed of two steps: the bridge formation where new robots are created following the given delay or distance
	 *and the bridge dissolution step where no more robots are created.
	 *The duration of the two steps are determined by respectively m_config.simulation.bridge_duration and m_config.simulation.dissolution_duration.
	 *The results are written in the two distinct files.
	 */
	void demoLoop();

	/**
	 * Get the bridge height. This function is not up to date and should not be used in this state
	 */
	double getBridgeHeight();

	/**
	 * Get the robot controller object
	 * Rq: in future development, could be adapted to be a pointer
	 * @return the robot controller object
	 */
	RobotController getController();

	/**
	 * Get the new path length induced by the bridge from the left corner of the obstacle to its right corner.
	 * This function is not up to date and should not be used in this state.
	 */
	double getNewPathLength();

	/**
	 * Get a pointer on the SFML window
	 * @return a pointer on the SFML window
	 */
	sf::RenderWindow* getWindow();

	/**
	 * Initialize the demo
	 */
	void init();

	/**
	 * Take a screenshot of the simulation at a given time step.
	 * The image is saved under  m_config.logfile_path + m_config.logfile_name + "_dissolution_" + std::to_string(m_currentIt) + ".jpg"
	  */
	void takeScreenshot();

	/**
	 * Write the file containing the summary of the simulation results
	 * The file is saved under m_config.logfile_path + m_config.logfile_name + "_result.txt";
	 */
	void writeResultFile();

	/**
	 * Write the file containing the bridge formation details.
	 * This file is updated every time a new robot enter the bridge state
	 * The file is saved under m_config.logfile_path + m_config.logfile_name + "_bridge.txt";
	 */
	void writeBridgeFile();

	void updateRobotPositionsForMovementCheck();

	bool robotDespawnedSinceLastCheck();

	bool robotsMovingRight();

	bool robotsMovingLeft();

	bool robotsMovingDown();

	bool robotsMovingUp();

	bool robotsMoving();

private:
	sf::RenderWindow window; //SFML window used to display the simulation
//	b2Vec2 m_gravity = b2Vec2(0.f, 0.f);
	RobotController m_robotController; //Robot controller containing the vector of the robots
	double m_to_px; //Scale to do the conversion from real dimensions (m) to simulated ones (pixels). Usually obtained from the terrain
	Terrain* m_terrain;
	MyContactListener_v2* myContactListener; //Contact listener
	double m_it = 0; //Iteration counter used to create the robots with a given delay between them
	int m_nbRobots = 0; //Number of robots created
	double m_elapsedTime = 0; // in seconds (real-time =/= from simulation time)
	double m_elapsedTimeDissolution = 0; //Duration of the dissolution phase (s)
	double m_elapsedTimeBridgeInitial = 0; //Duration of the bridge formation phase (s)
	double m_timexPosCheck = 0;
	double m_length_initial = 0; // Bridge length (Body length)
	double m_height_initial = 0; // Bridge height (Body length)
	int m_currentIt = 0; // Current iteration
	int m_nbRobotsInBridgeInitial = 0; // Number of robots involved in the bridge structure, ie the ones in bridge state + the ones stuck under the structure
	int m_nbRobotsInBridgeStateInitial = 0; // Number of robots in the bridge state
	double m_elapsedTimeBridgeFinal = 0; //Duration of the bridge formation phase (s)
	double m_length_final = 0; // Bridge length (Body length)
	double m_height_final = 0; // Bridge height (Body length)
	int m_nbRobotsInBridgeFinal = 0; // Number of robots involved in the bridge structure, ie the ones in bridge state + the ones stuck under the structure
	int m_nbRobotsInBridgeStateFinal = 0; // Number of robots in the bridge state
	int m_nbRobotsInBridgeEnd = 0; // NUmber of robots involved in the bridge structure when the simulation ends
	int m_nbRobotsInBridgeStateEnd = 0; // Number of robots in the bridge state at the end of the simulation

	bool m_towering = false; //Flag to indicate that the robots are towering
	bool m_stacking = false; //Flag to indicate that the robots are stacking
	bool m_stableBridge = false; //Flag to indicate that a stable bridge has been reached
	bool m_tooLongDissolution = false; //Flag to indicate the dissolution step is taking unreasonably long
	float m_avg_x_pos = 0; // average x position of all active robots
	bool m_simulationStuck = false; // Flag to indicate robots have not moved significantly in a long time

	b2Vec2 m_startP; //Start point of the bridge (on the left side of the obstacle
	b2Vec2 m_endP; //End point of the bridge (on the left side of the obstacle

	config::sConfig m_config ; //Configuration parameters (cf Config.h file)

//    sf::Texture m_tex;

	std::ofstream m_logFile; //Overall result file
	std::ofstream m_bridgeFile; //Bridge file that describe the bridge formation

	/**
	 * Parameters used to implement a gaussian distribution of the delay between the robots
	 */
	// random device class instance, source of 'true' randomness for initializing random seed
	std::random_device m_rd;
	// Mersenne twister PRNG, initialized with seed from previous random device instance
	std::mt19937 m_gen;
	double m_seed;
	double m_std_dev=0.25; //0.25
	//	std::default_random_engine gen;
	std::normal_distribution<double> m_gauss_delay;

	// Set the initial state of the simulation
	SimulationState state = SimulationState::Formation;

	std::unordered_map<int, double> m_prevPositionsX;
	std::unordered_map<int, double> m_currPositionsX;

	std::unordered_map<int, double> m_prevPositionsY;
	std::unordered_map<int, double> m_currPositionsY;

	// std::vector<Robot> m_currentRobotSnapshot;
	// std::vector<Robot> m_prevRobotSnapshot;

protected:
	b2World* m_world = nullptr; // pointer on the Box2D world
};


#endif /* DEMO_H_ */
