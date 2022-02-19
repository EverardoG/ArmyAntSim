/*
 * Demo.cpp
 *
 *  Created on: 6 Nov 2018
 *      Author: lucie
 *
 *  Update 01/22/2019:
 *  	- change in writeBridgeFile()
 *  		Robot position
 *			double x = r->getBody()->GetPosition().x/m_bodyLength; <-- r->getBody()->GetPosition().x;
 *			same for y
 *		- change in demoLoop()
 *			now when visualization is off, the simulation exit when the stability condition has been reached
 *			while (m_elapsedTime < m_config.simulation.duration) <-- while (m_elapsedTime < m_config.simulation.duration && !m_robotController.isBridgeStable() )
 *
 *	Update 01/24/2019:
 *		- Added the dissolution phase: changed cfg.simulation.duration <-- cfg.simulation.bridge_duration, add cfg.simulation.dissolution_duration
 *		- In Demo::demoLoop() added the while (m_elapsedTime < m_config.simulation.dissolution_duration + m_config.simulation.bridge_duration)
 *		- In Demo::writeResultFile() added the /Dissolution parameters /
 *		- Added in .h 	double m_elapsedTimeDissolution = 0;
 *						double m_elapsedTimeBridge = 0;
 *						int m_currentIt = 0;
 *						int m_nbRobotsInBridge
 *
 *	Update 02/8/2019:
 *		- Added the gaussian delay in Demo::demoLoop(). Can be activated by changing the gaussian_delay global variable to true
 *		//TODO: put it in global parameters so that it can be passed to the executable + standard deviation
 */

#include "Demo.h"
#include "helpers.h"

#include <thread>
#include <math.h>
#include <iostream>
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;

bool distance_from_bottom = false;

// bool gaussian_delay = true;
bool periodic_delay = false;

Demo::Demo(b2World* world, config::sConfig cfg){
	m_world = world;
	m_config = cfg;
//    m_tex.create(m_config.window.WINDOW_X_PX, m_config.window.WINDOW_Y_PX);

//	m_delay(delay), m_maxRobots(nb_robots)
	if (cfg.simulation.visualization) {
		window.create(sf::VideoMode(m_config.window.WINDOW_X_PX, m_config.window.WINDOW_Y_PX, 32), "Ant bridge simulation");
		window.setFramerateLimit(FPS);
	}

	world->SetGravity(b2Vec2(0,m_config.simulation.gravity));

	std::cout << "Terrain type: " << m_config.terrain.type << std::endl;

	// TODO: Add some sort of check that the user entered a valid terrain

	if (m_config.terrain.type == "v_terrain") {
		m_terrain = new Vterrain(m_world, m_config.window, m_config.terrain, m_config.robot.body_length );
	}
	else if (m_config.terrain.type == "v2bl_terrain") {
		m_terrain = new V2BLTerrain(m_world, m_config.window, m_config.terrain, m_config.robot.body_length );
	}
	else if (m_config.terrain.type == "ramp") {
		m_terrain = new Ramp(m_world, m_config.window, m_config.terrain, m_config.robot.body_length );
	}
	else if (m_config.terrain.type == "box") {
	 	m_terrain = new BoxTerrain(m_world, m_config.window, m_config.terrain, m_config.robot.body_length );
	}
	else if (m_config.terrain.type == "v_stepper") {
		m_terrain = new VStepper(m_world, m_config.window, m_config.terrain, m_config.robot.body_length );
	}
	else if (m_config.terrain.type == "cliff") {
		m_terrain = new CliffTerrain(m_world, m_config.window, m_config.terrain, m_config.robot.body_length );
	}
	else if (m_config.terrain.type == "island") {
		m_terrain = new IslandTerrain(m_world, m_config.window, m_config.terrain, m_config.robot.body_length );
	}
	else if (m_config.terrain.type == "pit") {
		m_terrain = new PitTerrain(m_world, m_config.window, m_config.terrain, m_config.robot.body_length);
	}
	else if (m_config.terrain.type == "terrace") {
		m_terrain = new TerraceTerrain(m_world, m_config.window, m_config.terrain, m_config.robot.body_length);
	}
	else if (m_config.terrain.type == "step_down") {
		m_terrain = new StepDownTerrain(m_world, m_config.window, m_config.terrain, m_config.robot.body_length);
	}
	else if (m_config.terrain.type == "flat") {
		m_terrain = new FlatTerrain(m_world, m_config.window, m_config.terrain, m_config.robot.body_length );
	}
	else if (m_config.terrain.type == "default") {
		m_terrain = new DefaultTerrain(m_world, m_config.window, m_config.terrain, m_config.robot.body_length );
	}
	else {
		std::cout << "No terrain specified or invalid terrain specified. Using default terrain." << std::endl;
		m_terrain = new DefaultTerrain(m_world, m_config.window, m_config.terrain, m_config.robot.body_length );
	}

	// std::cout << "m_config.window.WINDOW_X_PX: " << m_config.window.WINDOW_X_PX << std::endl;

	// m_terrain->create(m_world, m_config.window, m_config.terrain, m_config.robot.body_length );
	m_terrain->createBody(m_world);
	m_terrain->drawBody(window);
	m_to_px = m_terrain->getScale();

	/**Initial x position of the robot in the world*/
	if(distance_from_bottom){
		float V_slope = m_terrain->getVLength()/2;
		m_config.simulation.robot_initial_posX = m_terrain->getTopLeftCorner().x + V_slope - m_config.simulation.robot_initial_posX*m_config.robot.body_length;
		std::cout << "V_slope x position" << std::endl;
	}
	else if(!(m_terrain->getType()==DEFAULT)){
		m_config.simulation.robot_initial_posX = m_terrain->getTopLeftCorner().x - m_config.simulation.robot_initial_posX*m_config.robot.body_length;
	}
	else
	{
		// std::cout << "Else x position" << std::endl;
	}

	/**Initial y position of the robot*/
	m_config.robot.wheel_radius = (m_config.robot.body_length - 0.02)/4;
	m_config.simulation.robot_initial_posY = m_terrain->getTopLeftCorner().y - m_config.robot.wheel_radius - m_config.simulation.robot_initial_posY * m_config.robot.body_length;

	m_robotController.create(m_config.controller, m_config.robot, m_terrain->getBody(), m_terrain->getPosGoal());
	m_robotController.setScale(m_to_px);
	myContactListener = new MyContactListener_v2(m_robotController);

	if(m_config.simulation.gaussian_delay){
		m_gauss_delay = std::normal_distribution<double>(m_config.simulation.robot_delay,m_std_dev);
		m_seed = m_rd();// 2954034953.000000 ;//
		m_gen.seed(m_seed);
//		m_gen.seed(3.5);
	}

}

Demo::~Demo() {
	// TODO Auto-generated destructor stub
}

void Demo::init(){
	m_world->SetContactListener(myContactListener);
	// window.setFramerateLimit(60);
	createBridgeFile();

}

struct compare
{
    int key;
    compare(int const &i): key(i) {}

    bool operator()(int const &i) {
        return (i == key);
    }
};

void Demo::updateRobotPositionsForMovementCheck() {
	// Populate previous x positions with previous x positions
	if (!m_currPositionsX.empty()) {
		m_prevPositionsX = m_currPositionsX;
	}
	// Populate previous y positions with previous y positions
	if (!m_currPositionsY.empty()) {
		m_prevPositionsY = m_currPositionsY;
	}
	// Populate current x positions
	m_currPositionsX.clear();
	for (int num_robot = 0; num_robot < m_robotController.getNbActiveRobots(); num_robot++)
	{
		Robot* temp_robot_ptr = m_robotController.getRobot(num_robot);
		m_currPositionsX[temp_robot_ptr->getId()] = temp_robot_ptr->getPosition().x;
		// printf("Robot %d at %f\n", num_robot, m_currPositionsX[temp_robot_ptr->getId()]);
	}
	// Populate current y positions
	m_currPositionsY.clear();
	for (int num_robot = 0; num_robot < m_robotController.getNbActiveRobots(); num_robot++)
	{
		Robot* temp_robot_ptr = m_robotController.getRobot(num_robot);
		m_currPositionsY[temp_robot_ptr->getId()] = temp_robot_ptr->getPosition().y;
		// printf("Robot %d at %f\n", num_robot, m_currPositionsY[temp_robot_ptr->getId()]);
	}
}

bool Demo::robotDespawnedSinceLastCheck() {
	// Determine if any robots have despawned since the last check
	// If a robot despawned, that means it made it all the way off-screen
	// printf("Determining if robot despawned...\n");
	// printf("%ld previous robots found.\n", m_prevPositionsX.size());
	bool robotDespawned = false;
	for (std::pair<int, double> id_and_x : m_prevPositionsX)
	{
		// printf("  Previous robot:\n");
		int id = id_and_x.first;
		// printf("    id: %d\n", id);
		if (!m_currPositionsX.contains(id))
		{
			robotDespawned = true;
		}
	}
	return robotDespawned;
}

bool Demo::robotsMovingRight(){
	// printf("--robotsMovingRight-----\n");
	bool robotsmovingright = true;
	double deltaXThreshold = m_config.robot.body_length;
	// Compare x positions to previous x positions to
	// see if robots are moving right
	bool currentRobotsMovingRight = true;
	bool robotsMovingRightByThreshold = true;
	std::vector<double> deltaXs;
	if (!m_prevPositionsX.empty())
	{
		// printf("Previous positions was not empty\n");
		// Compare current x positions againts previous x positions
		for (std::pair<int, double> id_and_x : m_currPositionsX)
		{
			int id = id_and_x.first;
			double x = id_and_x.second;
			if (m_prevPositionsX.contains(id))
			{
				double currX = m_currPositionsX[id];
				double prevX = m_prevPositionsX[id];
				deltaXs.push_back(currX - prevX);
			}
		}
		// Determine if no robots are moving to the right at least one body length
		bool greaterThanThreshold = false;
		for (double deltaX  : deltaXs)
		{
			if (deltaX > deltaXThreshold)
			{
				greaterThanThreshold = true;
			}
		}
		if (!greaterThanThreshold)
		{
			robotsMovingRightByThreshold = false;
		}
	}
	// printf("robotsMovingRightByThreshold %d\n", robotsMovingRightByThreshold);

	// printf("robotDespawned %d\n", robotDespawned);
	// If not current robots have moved right AND no robots have despawned since
	// the last check, then the robots are NOT moving right
	if (!robotsMovingRightByThreshold)
	{
		robotsmovingright = false;
	}
	// printf("robotsmovingright %d\n", robotsmovingright);
	// printf("--end-----\n");

	return robotsmovingright;
}

bool Demo::robotsMovingLeft(){
	bool robotsmovingleft = true;
	double deltaXThreshold = - m_config.robot.body_length;
	// Compare x positions to previous x positions to
	// see if robots are moving left
	bool currentRobotsMovingLeft = true;
	bool robotsMovingLeftByThreshold = true;
	std::vector<double> deltaXs;
	if (!m_prevPositionsX.empty())
	{
		// printf("Previous positions was not empty\n");
		// Compare current x positions againts previous x positions
		for (std::pair<int, double> id_and_x : m_currPositionsX)
		{
			int id = id_and_x.first;
			double x = id_and_x.second;
			if (m_prevPositionsX.contains(id))
			{
				double currX = m_currPositionsX[id];
				double prevX = m_prevPositionsX[id];
				deltaXs.push_back(currX - prevX);
			}
		}
		// Determine if no robots are moving to the left at least one body length
		bool greaterThanThreshold = false;
		for (double deltaX  : deltaXs)
		{
			if (deltaX < deltaXThreshold)
			{
				greaterThanThreshold = true;
			}
		}
		if (!greaterThanThreshold)
		{
			robotsMovingLeftByThreshold = false;
		}
	}
	// printf("robotDespawned %d\n", robotDespawned);
	// If not current robots have moved left since
	// the last check, then the robots are NOT moving left
	if (!robotsMovingLeftByThreshold)
	{
		robotsmovingleft = false;
	}
	// printf("--end-----\n");

	return robotsmovingleft;
}

bool Demo::robotsMovingUp(){
	bool robotsmovingup = true;
	double deltaYThreshold = - m_config.robot.body_length;
	// Compare y positions to previous y positions to
	// see if robots are moving up
	bool currentRobotsMovingUp = true;
	bool robotsMovingUpByThreshold = true;
	std::vector<double> deltaYs;
	if (!m_prevPositionsY.empty())
	{
		// printf("Previous positions was not empty\n");
		// Compare current y positions againts previous y positions
		for (std::pair<int, double> id_and_y : m_currPositionsY)
		{
			int id = id_and_y.first;
			double y = id_and_y.second;
			if (m_prevPositionsY.contains(id))
			{
				double currY = m_currPositionsY[id];
				double prevY = m_prevPositionsY[id];
				deltaYs.push_back(currY - prevY);
			}
		}
		// Determine if no robots are moving to up at least one body length
		bool greaterThanThreshold = false;
		for (double deltaY  : deltaYs)
		{
			// A more negative y value indicates the robot is moving up
			if (deltaY < deltaYThreshold)
			{
				greaterThanThreshold = true;
			}
		}
		if (!greaterThanThreshold)
		{
			robotsMovingUpByThreshold = false;
		}
	}
	// printf("robotDespawned %d\n", robotDespawned);
	// If not current robots have moved up since
	// the last check, then the robots are NOT moving up
	if (!robotsMovingUpByThreshold)
	{
		robotsmovingup = false;
	}
	// printf("--end-----\n");

	return robotsmovingup;
}

bool Demo::robotsMovingDown(){
	bool robotsmovingdown = true;
	double deltaYThreshold = m_config.robot.body_length;
	// Compare y positions to previous y positions to
	// see if robots are moving down
	bool currentRobotsMovingDown = true;
	bool robotsMovingDownByThreshold = true;
	std::vector<double> deltaYs;
	if (!m_prevPositionsY.empty())
	{
		// printf("Previous positions was not empty\n");
		// Compare current y positions againts previous y positions
		for (std::pair<int, double> id_and_y : m_currPositionsY)
		{
			int id = id_and_y.first;
			double y = id_and_y.second;
			if (m_prevPositionsY.contains(id))
			{
				double currY = m_currPositionsY[id];
				double prevY = m_prevPositionsY[id];
				deltaYs.push_back(currY - prevY);
			}
		}
		// Determine if no robots are moving down at least one body length
		bool greaterThanThreshold = false;
		for (double deltaY  : deltaYs)
		{
			// A more positive y value indicates the robot is moving down
			if (deltaY > deltaYThreshold)
			{
				greaterThanThreshold = true;
			}
		}
		if (!greaterThanThreshold)
		{
			robotsMovingDownByThreshold = false;
		}
	}
	// printf("robotDespawned %d\n", robotDespawned);
	// If not current robots have moved down since
	// the last check, then the robots are NOT moving down
	if (!robotsMovingDownByThreshold)
	{
		robotsmovingdown = false;
	}
	// printf("--end-----\n");

	return robotsmovingdown;
}

bool Demo::robotsMoving() {
	if (robotDespawnedSinceLastCheck()
	|| robotsMovingRight()
	|| robotsMovingLeft()
	|| robotsMovingUp()
	|| robotsMovingDown()) {
		return true;
	}
	return false;
}


//Main demoLoop called in the main file: The demoLoop is structured in two cases: if the visualization is activated or not.
// Both cases are then almost identical apart from the simulation part.
void Demo::demoLoop(){
  if (m_elapsedTime > 790) printf("Starting main loop\n");
	if (m_config.simulation.visualization) {
		printf("Visualization is on\n");
	}
	else {
		printf("Visualization is off\n");
	}

	// While the simulation is running
	while (state != SimulationState::End) {
		if (m_elapsedTime > 790) printf("Simulation has not ended\n");
		// Run non-case specific code
		// printf("does anything here get run at allll???");



		// Run case specific code
		switch (state)
		{
			case SimulationState::Travel:
				writeBridgeFile();

			case SimulationState::Formation:

			//	if (m_robotController.getNbActiveRobots() > 0){
			//		std::cout << "angle: " << m_robotController.getRobot(0)->getAngle() << std::endl;
			//	}
				// Update the robot speeds dynamically if relevant
				if (m_config.robot.dynamic_speed) {
					m_robotController.calculateSpeedsToGoal(m_terrain->getPosGoal(), m_elapsedTime, "!dissolution");
				}
				// std::cout << m_stacking << std::endl;

				// Check if any robot has gotten up beyond the window
				if(!addRobot()){
					// Robots are stacking in the x direction
					m_stacking = true;
					printf("Robot stacking.\n");
				}
				else if (m_robotController.checkTowering()){
					// Robots are stacking in the y direction
					m_towering = true;
					printf("Robots towering.\n");
				}

				m_robotController.step(m_config.window.WINDOW_X_PX);
				m_world->Step(1.f/60.f, 100, 100);
				m_robotController.removeRobot();

				// Render if visualization is on
				if ( m_config.simulation.visualization ) {
					window.clear(sf::Color::White);
					m_terrain->drawBody(window);
					m_robotController.drawRobots(window, m_to_px);
					window.display();
				}


				//writeBridgeFile();

				// Save a screenshot every 600 iteration, ie every 10 s of real-time at 60 FPS
				if(m_currentIt % 600 == 0){
					takeScreenshot();
				}

				m_elapsedTime += 1.f/FPS;
				m_currentIt ++;
				if(!m_stableBridge){
					m_stableBridge = m_robotController.isBridgeStable();
				}
				if(periodic_delay){
					m_config.simulation.robot_delay = 2.5/(cos(PI/(18*60)*m_currentIt)*cos(PI/(18*60)*m_currentIt));
				}

				// End the simulation if the average x position has not moved to the right in 10 seconds
				// if (m_elapsedTime >= m_timexPosCheck + 60.0){
				// 	// std::cout << "m_avg_x_pos: " << m_avg_x_pos << std::endl;
				// 	if (m_robotController.getAvgPos().x <= m_avg_x_pos){
				// 		std::cout << "Simulation is stuck" << std::endl;
				// 		m_simulationStuck = true;
				// 	}
				// 	m_timexPosCheck = m_elapsedTime;
				// 	m_avg_x_pos = m_robotController.getAvgPos().x;
				// }

				// Flag the simulation if it's taking too long to dissolve (10800 s = 3 hrs)
				if (m_elapsedTime - m_elapsedTimeBridgeInitial > 10800) {
					m_tooLongDissolution;
				}

				break;

			case SimulationState::Dissolution:
				// Update the robot speeds dynamically if relevant
				if (m_config.robot.dynamic_speed) {
					m_robotController.calculateSpeedsToGoal(m_terrain->getPosGoal(), m_elapsedTime, "dissolution");
				}


				// std::cout << "Dissolution State" << std::endl;
				m_robotController.step(m_config.window.WINDOW_X_PX);
				m_world->Step(1.f/60.f, 100, 100);
				m_robotController.removeRobot();
				// printf("Finished removing robot\n");

				if ( m_config.simulation.visualization ) {
					window.clear(sf::Color::White);
					m_terrain->drawBody(window);
					m_robotController.drawRobots(window, m_to_px);
					window.display();
				}

				// if (m_elapsedTime > 790) printf("About to write bridge file\n");
				// writeBridgeFile();
				// if (m_elapsedTime > 790) printf("successfully wrote bridge file\n");

				// m_robotController.isBridgeDissolved();
				// if (m_elapsedTime > 790) printf("Checked if bridge is dissolved\n");

				// Save a screenshot every 600 iteration, ie every 10 s of real-time at 60 FPS
				if(m_currentIt % 600 == 0){
					takeScreenshot();
				}

				m_elapsedTime += 1.f/FPS;
				m_currentIt ++;

				// if (m_elapsedTime > 790) printf("Updated timers\n");
				// Flag the simulation if it's taking too long to dissolve (10800 s = 3 hrs)
				if (m_elapsedTime - m_elapsedTimeBridgeInitial > 10800) {
					m_tooLongDissolution;
				}

				break;
		}
		// if (m_elapsedTime > 790) printf("Got out of cases\n");
		// Run this code regardless of case
		// End the simulation if no robot has moved right by a body length in 60 seconds
		//     and no robot has despawnedAbout to write bridge file
		// printf("m_elapsedTime: %f | m_timexPosCheck: %f\n", m_elapsedTime, m_timexPosCheck);
		if (m_elapsedTime >= m_timexPosCheck + 10.0){
			// Update the value for whether robots are stuck
			if (!robotsMoving())
			{
				std::cout << "Simulation is stuck" << std::endl;
				m_simulationStuck = true;
			}
			updateRobotPositionsForMovementCheck();
			m_timexPosCheck = m_elapsedTime;
		}

		// Based on robots reaching the goal
		if ( m_config.simulation.smart_dissolution ) {
			// std::cout << "Smart dissolution is on" << std::endl;
			// Switch the state to Travel if one robot has reached the goal
			if ( state == SimulationState::Formation && m_robotController.getNbRobotsReachedGoal() >= 1 ) {
				// Get the number of robots in the initial bridge
				m_nbRobotsInBridgeStateInitial = m_robotController.getNbRobots(BRIDGE);
				m_nbRobotsInBridgeInitial = m_nbRobotsInBridgeStateInitial + m_robotController.getNbRobotsBlocked();
				m_elapsedTimeBridgeInitial = m_elapsedTime;
				std::cout << "---------------------------------------" << std::endl;
				std::cout << "Initial Bridge Time: " << m_elapsedTimeBridgeInitial << std::endl;
				std::cout << "---------------------------------------" << std::endl;

				// Store bridge characteristics
				m_length_initial = getNewPathLength();
				m_height_initial = getBridgeHeight();

				state = SimulationState::Travel;
				std::cout << "Switched to travel state." << std::endl;

				// Save a screenshot of initial bridge
				takeScreenshot();
			}
			// Switch to Dissolution once 10 robots have reached the goal
			else if ( state == SimulationState::Travel && m_robotController.getNbRobotsReachedGoal() >= 10 ) {
				// Get the number of robots in the final bridge
				m_nbRobotsInBridgeStateFinal = m_robotController.getNbRobots(BRIDGE);
				m_nbRobotsInBridgeFinal = m_nbRobotsInBridgeStateFinal + m_robotController.getNbRobotsBlocked();
				m_elapsedTimeBridgeFinal = m_elapsedTime;

				std::cout << "---------------------------------------" << std::endl;
				std::cout << "Final Bridge Time: " << m_elapsedTimeBridgeFinal << std::endl;
				std::cout << "---------------------------------------" << std::endl;

				// Store bridge characteristics
				m_length_final = getNewPathLength();
	      m_height_final = getBridgeHeight();

				// Set robots to use fixed speeds if they were previously using dynamic speeds
				// TODO: dont update speeds if a robot is in its regrip state
				if (m_config.robot.dynamic_speed) {
					m_robotController.SetGlobalSpeed(m_config.robot.speed);
				}

				// Switch to dissolution state
				state = SimulationState::Dissolution;
				std::cout << "Switched to dissolution state." << std::endl;

				// Save a screenshot of the final bridge
				takeScreenshot();
			}
			// End the simulation if all robots have despawned after dissolving their bridge
			else if ( state == SimulationState::Dissolution && m_robotController.getNbActiveRobots() == 0 ) {
				m_elapsedTimeDissolution = m_elapsedTime;
				state = SimulationState::End;
				std::cout << "---------------------------------------" << std::endl;
				std::cout << "Finished Disolution Time: " << m_elapsedTimeDissolution << std::endl;
				std::cout << "---------------------------------------" << std::endl;
			}
			// End the simulation early if robots are stacking or robots are stuck
			if (m_stacking || m_simulationStuck || m_tooLongDissolution || m_towering){
				takeScreenshot();
				state = SimulationState::End;
			}
		}

		// Based on timing
		else {
			if ( m_elapsedTime < m_config.simulation.bridge_duration ) {
				state = SimulationState::Formation;
			}
			else if ( m_elapsedTime < m_config.simulation.dissolution_duration + m_config.simulation.bridge_duration ) {
				// Get the number of robots in the final bridge if the simulation just switched
				if (state == SimulationState::Formation) {
					// Get the number of robots in the final bridge
					m_nbRobotsInBridgeStateInitial = m_robotController.getNbRobots(BRIDGE);
					m_nbRobotsInBridgeInitial = m_nbRobotsInBridgeStateInitial + m_robotController.getNbRobotsBlocked();
					m_elapsedTimeBridgeInitial = m_elapsedTime;
					m_length_initial = getNewPathLength();
					m_height_initial = getBridgeHeight();

					// Set robots to use fixed speeds if they were previously using dynamic speeds
					if (m_config.robot.dynamic_speed) {
						m_robotController.SetGlobalSpeed(m_config.robot.speed);
					}
				}
				state = SimulationState::Dissolution;
			}
			else {
				state = SimulationState::End;
			}
		}

		// If the visualization is active, then take in keyboard inputs and act on them
		if ( m_config.simulation.visualization ) {
			sf::Event event;
			while (window.pollEvent(event)) {
				// "close requested" evm_timexPosCheckent: close the window
				if (event.type == sf::Event::Closed) {
					window.close();
				}
				// "s" event: take a screenshot
				// change this to use takeScreenshot
				if (event.key.code == sf::Keyboard::S) {
					takeScreenshot();
				}
			}

			// Stop the simulation if visualization is active and window is closed
			if ( m_config.simulation.visualization && !window.isOpen() ) {
				state = SimulationState::End;
			}
		}
	}

	// This is where the simulation ends
	printf("End simulation \n");
	// Record a few last results of the run
	takeScreenshot();
	// Get the number of robots in the initial bridge
	m_nbRobotsInBridgeStateEnd = m_robotController.getNbRobots(BRIDGE);
	m_nbRobotsInBridgeEnd = m_nbRobotsInBridgeStateEnd + m_robotController.getNbRobotsBlocked();
	if ( m_config.simulation.visualization && window.isOpen() ) {
		window.close();
	}
	m_bridgeFile.close();

// This comment seems important but I'm not sure where it belongs
/**  Data processing
* Precise the simulation parameters: distance between robots, speed
* Get time of the first bridge contact
* get time when the last robot enter the stable bridge state
* get points of contact for every robot + position and orientation of center
* */

}

bool Demo::addRobot() {
	if ( m_config.simulation.use_delay ) {
		return addRobotWithDelay();
	}
	else {
		return addRobotWithDistance();
	}
}

bool Demo::addRobotWithDelay(){

	if(m_config.controller.infinite_robots || m_nbRobots < m_config.simulation.nb_robots){
		int final_it = int(60 * m_config.simulation.robot_delay) ; // at 60 fps
		if(m_it > final_it){
			if(m_nbRobots && m_robotController.robotStacking(m_robotController.getRobotWithId(m_nbRobots), m_config.simulation.robot_initial_posX)){
				m_robotController.setBridgeStability(false);
				return false;
			}
			m_robotController.createRobot(m_world, 0, m_config.simulation.robot_initial_posX, m_config.simulation.robot_initial_posY);
			m_it = 0;
			if(m_config.simulation.gaussian_delay){
				m_config.simulation.robot_delay=std::max(m_gauss_delay(m_gen), 1.5);
				std::cout<<"delay rand= "<< m_config.simulation.robot_delay<<std::endl;
			}
			m_nbRobots++;

			if(m_nbRobots == 2){
				m_config.simulation.robot_distance = (m_robotController.getRobot(0)->getPosition().x- m_robotController.getRobot(1)->getPosition().x)/m_config.robot.body_length;
				int delay = m_robotController.getRobot(0)->getDelay();
				/* If the first robot is flat, the phase shift is proportional to the delay remaining*/
				int walk_delay=int(m_config.controller.walk_delay*FPS);
				if(delay > 0 && delay < walk_delay){
					m_config.simulation.robot_phase = PI*(1 + float(walk_delay-delay)/float(walk_delay));
				}
				else{
					m_config.simulation.robot_phase = moduloAngle(m_robotController.getRobot(0)->getAngle(), PI);
				}
				takeScreenshot();
			}
		}
		else{
			m_it++;
		}
	}
	return true;
}


bool Demo::addRobotWithDistance(){

	if(m_nbRobots == 0){
		int delay = int(m_config.controller.walk_delay*FPS);
		m_robotController.createRobot(m_world, delay, m_config.simulation.robot_initial_posX, m_config.simulation.robot_initial_posY);
		// std::cout << "first robot created"<<std::endl;
		m_nbRobots++;
		return true;
	}
	else if(m_config.controller.infinite_robots || m_nbRobots < m_config.simulation.nb_robots){

		float distance = m_robotController.getRobotWithId(m_nbRobots)->getBody()->GetPosition().x;
		distance -= m_config.simulation.robot_initial_posX*m_config.robot.body_length;
		float target = m_config.simulation.robot_distance*m_config.robot.body_length;
		// std::cout << "distance: " << distance << " | " << "target: " << target << std::endl;

		if(m_config.simulation.robot_phase > PI){
			// std::cout << "m_config.simulation.robot_phase > PI" << std::endl;
			int delay = round(int(m_config.controller.walk_delay*FPS)*(2*PI-m_config.simulation.robot_phase)/PI); // what is the better option: int or round ?
			if(distance >= target && m_robotController.getRobotWithId(m_nbRobots)->getDelay()==delay){
				float x = m_robotController.getRobotWithId(m_nbRobots)->getBody()->GetPosition().x - target;
				if(m_robotController.createRobot(m_world, 0, x, m_config.simulation.robot_initial_posY)){
					m_nbRobots++;
					return true;
				}
			}

		}
		else{
			// std::cout << "m_config.simulation.robot_phase < PI" << std::endl;
			float angle = m_robotController.getRobotWithId(m_nbRobots)->getBody()->GetAngle();
			float flipping_distance = m_config.robot.body_length-2*m_config.robot.wheel_radius;
			distance = distance - flipping_distance*sin(angle)/2;
			int delay = -round(int(m_config.controller.walk_delay*FPS)*m_config.simulation.robot_phase/PI); // what is the better option: int or round ?
			if(distance >= target && m_robotController.getRobotWithId(m_nbRobots)->getDelay()==delay){
				float x = m_robotController.getRobotWithId(m_nbRobots)->getBody()->GetPosition().x - target;
				if(m_robotController.createRobot(m_world, 0, x, m_config.simulation.robot_initial_posY)){
					m_nbRobots++;
					return true;
				}
			}
		}
	}

	if(m_nbRobots && m_robotController.robotStacking(m_robotController.getRobotWithId(m_nbRobots), m_config.simulation.robot_initial_posX)){
		m_robotController.setBridgeStability(false);
		return false;
	}

	return true;
}

RobotController Demo::getController(){
	return m_robotController;
}

sf::RenderWindow* Demo::getWindow(){
	sf::RenderWindow* ptrWind = &window;
	return ptrWind;
}

double Demo::getBridgeHeight(){
	int n = m_robotController.getNbActiveRobots();
	double x_start = FLT_MAX;
	double y_start = 0;

	Robot* r;
	for(int i=0 ; i<n ; i++){
		r =  m_robotController.getRobot(i);
		if (r->getState()== BRIDGE){

			// test for first grip joint
			if(r->m_currentGripJoint){
				double x_r = r->m_currentGripJoint->GetAnchorA().x;
				if(x_r < x_start){
					x_start = x_r;
					y_start = r->m_currentGripJoint->GetAnchorA().y;
				}
			}

			// test for second grip joint
			if(r->m_previousGripJoint){
				double x_r = r->m_previousGripJoint->GetAnchorA().x;
				if(x_r < x_start){
					x_start = x_r;
					y_start = r->m_previousGripJoint->GetAnchorA().y;
				}
			}
		}
	}

	double y_bottom_bridge = m_terrain->getBottom().y;

	y_start = abs(y_bottom_bridge - y_start)/m_config.robot.body_length;

	return y_start;
}

double Demo::getNewPathLength(){
	int n = m_robotController.getNbActiveRobots();
	double x_start = FLT_MAX;
	double y_start = 0;
	double x_end = 0;
	double y_end = 0;

	double l=0;

	Robot* r;
	for(int i=0 ; i<n ; i++){
		r =  m_robotController.getRobot(i);
		if (r->getState() == BRIDGE){

			// test for first grip joint
			if(r->m_currentGripJoint){
				double x_r = r->m_currentGripJoint->GetAnchorA().x;
				if(x_r < x_start){
					x_start = x_r;
					y_start = r->m_currentGripJoint->GetAnchorA().y;
				}

				if(x_r > x_end){
					x_end = x_r;
					y_end = r->m_currentGripJoint->GetAnchorA().y;
				}
			}

			// test for second grip joint
			if(r->m_previousGripJoint){
				double x_r = r->m_previousGripJoint->GetAnchorA().x;
				if(x_r < x_start){
					x_start = x_r;
					y_start = r->m_previousGripJoint->GetAnchorA().y;
				}

				if(x_r > x_end){
					x_end = x_r;
					y_end = r->m_previousGripJoint->GetAnchorA().y;
				}
			}
		}
	}

	m_startP.Set(x_start, y_start);
	m_endP.Set(x_end, y_end);

	l = distance(m_terrain->getTopLeftCorner().x, m_terrain->getTopLeftCorner().y, x_start, y_start);
	l+= distance(x_end, y_end, x_start, y_start);
	l+= distance(m_terrain->getTopRightCorner().x, m_terrain->getTopRightCorner().y, x_end, y_end);

	l = l/m_config.robot.body_length;
	return l;

}

void Demo::writeResultFile(){

	fs::create_directories(m_config.logfile_path);
	std::string filename = m_config.logfile_path + m_config.logfile_name + "_result.txt";
	std::cout<<filename<<std::endl;
	m_logFile.open(filename);
	double t;

	/** Terrain parameters */
	if(m_terrain->getType()==V_TERRAIN){
		m_logFile << "V-terrain parameters: \n";
		m_logFile << "	Width: "<< m_config.terrain.v_width << "\n";
		m_logFile << "	Height: "<< m_config.terrain.v_height << "\n";
		m_logFile << "	Angle: "<< m_config.terrain.v_angle*RAD_TO_DEG << " deg\n";
		double l = m_terrain->getVLength()/m_config.robot.body_length;
		m_logFile << "	total V path length: "<< std::to_string(l) << "\n \n";
		m_logFile << "	top right corner: "<< std::to_string(m_terrain->getTopRightCorner().x) << ", "<< std::to_string( m_terrain->getTopRightCorner().y)<< "\n";
		m_logFile << "	top left corner: "<< std::to_string(m_terrain->getTopLeftCorner().x) << ", "<< std::to_string( m_terrain->getTopLeftCorner().y)<< "\n";
		m_logFile << "	bottom: "<< std::to_string(m_terrain->getBottom().x) << ", "<< std::to_string(m_terrain->getBottom().y)<< "\n \n";
	}


	/** Simulation parameters */
	m_logFile << "Simulation parameters: \n";
//	m_logFile << "	Robots entry point: "<< std::to_string(m_delay) << "s\n";
	if(m_config.simulation.gaussian_delay){
		m_logFile << "	Gaussian delay "<< " \n";
		m_logFile << "		standard deviation "<< m_std_dev << " \n";
		m_logFile << std::fixed << "		seed "<< m_seed << " \n";
	}
	m_logFile << "	Delay between robots: "<< std::to_string(m_config.simulation.robot_delay) << " s\n";
	m_logFile << "	World gravity: "<< std::to_string(m_config.simulation.gravity) << "\n";
	m_logFile << "	Max number of robots: "<< std::to_string(m_config.simulation.nb_robots) << "\n";
	m_logFile << "	Distance between robots: "<< std::to_string(m_config.simulation.robot_distance) << " body length\n";
	m_logFile << "	Phase shift between robots: "<< std::to_string(m_config.simulation.robot_phase) << " rad\n";
	m_logFile << "	Initial x position of the first robot: "<< std::to_string(m_config.simulation.robot_initial_posX) << " m\n";
	m_logFile << "	Initial distance of the robot from the edge of the V: "<< std::to_string((m_terrain->getTopLeftCorner().x-m_config.simulation.robot_initial_posX)/m_config.robot.body_length) << " m\n";
	m_logFile << "	Bridge formation step duration: "<< std::to_string(m_elapsedTimeBridgeInitial) << " s\n\n";
	m_logFile << "  Bridge travel step duration: "<< std::to_string(m_elapsedTimeBridgeFinal) << " s\n\n";
	m_logFile << "	Bridge dissolution step duration: "<< std::to_string(m_elapsedTimeDissolution) << " s\n\n";
	m_logFile << "	Simulation duration: "<< std::to_string(m_elapsedTime) << " s\n\n";
	m_logFile << "  Early termination flags:\n";
	m_logFile << "    m_stacking: " << std::to_string(m_stacking) << "\n";
	m_logFile << "    m_towering: " << std::to_string(m_towering) << "\n";
	m_logFile << "    m_simulationStuck: " << std::to_string(m_simulationStuck) << "\n";
	m_logFile << "    m_tooLongDissolution: " << std::to_string(m_tooLongDissolution) << "\n\n";

	/** Controller parameters */
	m_logFile << "Controller parameters: \n";
	m_logFile << "	Angle limit before the robot is able to grab: "<< std::to_string(m_config.controller.angle_limit*RAD_TO_DEG ) << " deg\n";
	m_logFile << "	Delay in bridge state: "<< std::to_string(m_config.controller.bridge_delay) << " s\n";
	m_logFile << "	Delay in walking state: "<< std::to_string(m_config.controller.walk_delay) << " s\n";
	m_logFile << "	Movement bridge_duration before the robot is considered to push: "<< std::to_string(m_config.controller.time_before_pushing) << " s\n";
	m_logFile << "	Max robot in the window: "<< std::to_string(m_config.controller.max_robot_window) << " s\n";
	m_logFile << "	Stability condition: "<< std::to_string(m_config.controller.stability_condition) << " s\n\n";

	/** Controller parameters */
	m_logFile << "Robot parameters: \n";
	m_logFile << "	Robot body length "<< std::to_string(m_config.robot.body_length) << " m\n";
	m_logFile << "	Robot velocity "<< std::to_string(m_config.robot.speed) << " rad/s\n\n";

	/** Bridge parameters */
	if(m_nbRobotsInBridgeInitial >0){
		m_logFile << "Bridge parameters: \n";
		if(m_terrain->getType()==V_TERRAIN){
			m_logFile << "	Bridge start: "<< std::to_string(m_startP.x) << ", "<< std::to_string(m_startP.y)<< "\n";
			m_logFile << "	Bridge end: "<< std::to_string(m_endP.x) << ", "<< std::to_string(m_endP.y)<< "\n";
		}
		m_logFile << "	New path length initial: "<< std::to_string(m_length_initial) << "\n";
		m_logFile << "	Bridge height initial: "<< std::to_string(m_height_initial) << "\n";
		m_logFile << "	Number of robots in the bridge initial: "<< std::to_string(m_nbRobotsInBridgeInitial) << "\n\n";
		m_logFile << "	Number of robots in bridge state initial: "<< std::to_string(m_nbRobotsInBridgeStateInitial) << "\n\n";

		m_logFile << "	New path length final: "<< std::to_string(m_length_final) << "\n";
		m_logFile << "	Bridge height final: "<< std::to_string(m_height_final) << "\n";
		m_logFile << "	Number of robots in the bridge final: "<< std::to_string(m_nbRobotsInBridgeFinal) << "\n\n";
		m_logFile << "	Number of robots in bridge state final: "<< std::to_string(m_nbRobotsInBridgeStateFinal) << "\n\n";

		m_logFile << "  Number of robots in bridge end: " << std::to_string(m_nbRobotsInBridgeEnd) << "\n";
		m_logFile << "  Number of robots in bridge state end:" << std::to_string(m_nbRobotsInBridgeStateEnd) << "\n";

		if(m_stableBridge){
		  m_logFile << "The bridge is STABLE \n";
		  t = m_robotController.getStabilizationTime();
		  m_logFile << "Time to reach stable bridge: "<< std::to_string(t) << " s \n\n";
		}

		else{
		  m_logFile << "Bridge stability has not been reached \n \n";
		  if(m_stacking){
			  m_logFile << "Robots are stacking \n \n";
		  }
		}
	}

	else{
		m_logFile << "No bridge has formed \n\n";
	}

	/** Dissolution parameters */
	m_logFile << "Dissolution parameters: \n";
	if(m_robotController.getNbRobots(BRIDGE) > 0){
		m_logFile << "	Bridge dissolution has not been reached \n ";
		m_logFile << "	Number of robots in the bridge: "<< std::to_string(m_robotController.getNbActiveRobots()) << "\n\n";

	}
	else{
		m_logFile << "	The bridge has DISSOLVED \n";
		t=m_robotController.getDissolutionTime();
		m_logFile << "	Time to reach bridge dissolution: "<< std::to_string(t) << " s \n\n";
		m_logFile << "	Number of robots blocked: "<< std::to_string(m_robotController.getNbActiveRobots()) << "\n\n";
	}

	m_logFile << "Rq: All distances are expressed in body length unit but coordinates are not. \n";
	m_logFile.close();

	printf("file closed \n");
}

void Demo::createBridgeFile(){
	std::string filename = m_config.logfile_path + m_config.logfile_name + "bridge.txt";
	m_bridgeFile.open(filename);
	m_bridgeFile << "Timestamp; robot ID; x coordinate; y coordinate; angle; current joint x; current joint y; previous joint x; previous joint y; it entry; age \n\n";
}

void Demo::writeBridgeFile(){
	if(m_robotController.m_bridgeEntry){
		m_robotController.m_bridgeEntry = false;
//		std::cout<<"write in bridge file "<<std::endl;
		if(!m_bridgeFile.is_open()){
			std::string filename = m_config.logfile_path + m_config.logfile_name + "_bridge.txt";
			m_bridgeFile.open(filename);
		}
		int n = m_robotController.getNbActiveRobots();
		for (int i=0; i<n; i++){
		Robot* r = m_robotController.getRobot(i);
		if(r->getState()==BRIDGE){
			/** Robot id and age*/
			int id = r->getId();
			int age = m_currentIt - r->m_bridgeAge; //TODO: check that current it of demo match the one of robot controller

			/** Robot position*/
			double x = r->getBody()->GetPosition().x;
			double y = r->getBody()->GetPosition().y;
			double a = r->getBody()->GetAngle();

			/** Joint position*/
			float x_cj = 0;
			float y_cj = 0;
			if(r->m_currentGripJoint){
				x_cj = r->m_currentGripJoint->GetAnchorA().x;
				y_cj = r->m_currentGripJoint->GetAnchorA().y;
			}
			float x_pj = 0;
			float y_pj = 0;
			if(r->m_previousGripJoint){
				x_pj = r->m_previousGripJoint->GetAnchorA().x;
				y_pj = r->m_previousGripJoint->GetAnchorA().y;
			}

			m_bridgeFile << m_currentIt << "; "<< id << "; "<< x<< "; "<< y << "; "<< a<< "; "<<x_cj<< "; "<<y_cj<< "; "<<x_pj<< "; "<<y_pj<<"; "<<r->m_bridgeAge<<"; "<<age<<"\n";

		}
		}
	}
}

void Demo::takeScreenshot(){
	std::cout << "TAKING SCREENSHOT" << std::endl;
	// Change this so that it renders an image using the dimensions for the window
	// and saves that image
	// m_config.window.WINDOW_X_PX
	// m_config.window.WINDOW_Y_PX

	// Create a white canvas
	// sf::Image screenshot;
	// screenshot.create(m_config.window.WINDOW_X_PX, m_config.window.WINDOW_Y_PX, sf::Color::Black);

	// Create a render texture
	// sf::RenderTexture texture;
	// texture.create(m_config.window.WINDOW_X_PX, m_config.window.WINDOW_Y_PX);

	// Draw the terrain onto the texture
	// m_terrain->drawBody(texture);
	// Draw robots onto the texture

	// copy the render texture to an image

	// save the image

	// if(draw){
	// 	window.clear(sf::Color::White);
	// 	m_terrain->drawBody(window);
	// 	m_robotController.drawRobots(window, m_to_px);
	// }

	// Create a white render texture
	sf::RenderTexture texture;
	texture.create(m_config.window.WINDOW_X_PX, m_config.window.WINDOW_Y_PX);
	texture.clear(sf::Color::White);

	// Draw the terrain onto the texture
	m_terrain->drawBody(texture);

	// Draw the robots onto the texture
	m_robotController.drawRobots(texture, m_to_px);

	// Finalize the texture
	texture.display();

	// Convert the texture into an image
	sf::Image image = texture.getTexture().copyToImage();

	// Name the image according to the simulation state
	std::string filename = m_config.logfile_path + m_config.logfile_name;

	// 6 digits in the beginning
	std::string time_str = std::to_string( (int) m_elapsedTime);
	if (time_str.size() < 6) {
		std::string zero_str(6 - time_str.size(), '0');
		time_str = zero_str + time_str;
	}

	// Add the timestamp to the filename
	filename = filename + time_str;
	// Add an indicator for whether this was taken during bridge formation or dissolution
        if ( state == SimulationState::Formation ) { filename = filename + "_formation"; }
	else if ( state == SimulationState::Travel ) { filename = filename + "_travel"; }
	else if (state == SimulationState::Dissolution ) { filename = filename + "_dissolution"; }
	// Add file extension
	filename = filename + ".jpg";
	// Create the target directory if none exists
	fs::create_directories(m_config.logfile_path);
	// Save the image
	image.saveToFile(filename);
	std::cout << "filename: " << filename << std::endl;
}
