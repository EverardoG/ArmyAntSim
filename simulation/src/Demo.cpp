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
#include <filesystem>
namespace fs = std::filesystem;

bool distance_from_bottom = false;

bool gaussian_delay = true;
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

	// TODO: This is where terrain type will be set
	if (m_config.terrain.type == "v_terrain") {
		m_terrain = new Vterrain;
	}
	else if (m_config.terrain.type == "v2bl_terrain") {
		m_terrain = new V2BLTerrain;
	}
	else if (m_config.terrain.type == "ramp") {
		m_terrain = new Ramp;
	}
	else if (m_config.terrain.type == "box") {
		m_terrain = new BoxTerrain;
	}
	else if (m_config.terrain.type == "v_stepper") {
		m_terrain = new VStepper;
	}
	else if (m_config.terrain.type == "cliff") {
		m_terrain = new CliffTerrain;
	}

	std::cout << "m_config.window.WINDOW_X_PX: " << m_config.window.WINDOW_X_PX << std::endl;

	m_terrain->create(m_world, window, m_config.terrain, m_config.window.WINDOW_X_PX, m_config.robot.body_length );
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

	m_robotController.create(m_config.controller, m_config.robot, m_terrain->getBody());
	m_robotController.setScale(m_to_px);
	myContactListener = new MyContactListener_v2(m_robotController);

	if(gaussian_delay){
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

//Main demoLoop called in the main file: The demoLoop is structured in two cases: if the visualization is activated or not.
// Both cases are then almost identical apart from the simulation part.
void Demo::demoLoop(){
	if (m_config.simulation.visualization) {
		printf("Visualization is on\n");
	}
	else {
		printf("Visualization is off\n");
	}

	// While the simulation is running
	while (state != SimulationState::End) {
		switch (state)
		{
			case SimulationState::Formation:
				printf("Case: Formation\n");

				// Update the robot speeds dynamically if relevant
				if (!m_config.robot.fixed_speed) {
					m_robotController.calculateSpeedsToGoal(m_terrain->getPosGoal(), 1.0);
				}

				// This is where the traffic control is defined: either using addRobotWithDelay() or addRobotWithDistance()
				if(!addRobotWithDelay()){
					m_stacking = true;
					printf("robot stacking \n");
					break;
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


				writeBridgeFile();

				// Save a screenshot every 600 iteration, ie every 10 s of real-time at 60 FPS
				if(m_config.simulation.visualization && m_currentIt % 600 == 0){
					takeScreenshot(true);
				}

				m_elapsedTime += 1.f/FPS;
				m_currentIt ++;
				if(!m_stableBridge){
					m_stableBridge = m_robotController.isBridgeStable();
				}
				if(periodic_delay){
					m_config.simulation.robot_delay = 2.5/(cos(PI/(18*60)*m_currentIt)*cos(PI/(18*60)*m_currentIt));
				}
				break;

			case SimulationState::Dissolution:
				printf("Case: Dissolution State\n");
				m_robotController.step(m_config.window.WINDOW_X_PX);
				m_world->Step(1.f/60.f, 100, 100);
				m_robotController.removeRobot();

				if ( m_config.simulation.visualization ) {
					window.clear(sf::Color::White);
					m_terrain->drawBody(window);
					m_robotController.drawRobots(window, m_to_px);
					window.display();
				}

				writeBridgeFile();

				m_robotController.isBridgeDissolved();

				// Save a screenshot every 600 iteration, ie every 10 s of real-time at 60 FPS
				if(m_config.simulation.visualization && m_currentIt % 600 == 0){
					takeScreenshot(true);
				}

				m_elapsedTime += 1.f/FPS;
				m_currentIt ++;
				break;
		}
		// Update the simulation state
		if ( m_elapsedTime < m_config.simulation.bridge_duration ) {
			state = SimulationState::Formation;
		}
		else if ( m_elapsedTime < m_config.simulation.dissolution_duration + m_config.simulation.bridge_duration ) {
			// Get the number of robots in the final bridge if the simulation just switched
			if (state == SimulationState::Formation) {
				// Get the number of robots in the final bridge
				m_nbRobotsInBridgeState = m_robotController.getNbRobots(BRIDGE);
				m_nbRobotsInBridge = m_nbRobotsInBridgeState + m_robotController.getNbRobotsBlocked();
				m_elapsedTimeBridge = m_elapsedTime;
				m_length = getNewPathLength();
				m_height = getBridgeHeight();

				// Set robots to use fixed speeds if they were previously using dynamic speeds
				if (!m_config.robot.fixed_speed) {
					m_robotController.SetGlobalSpeed(m_config.robot.speed);
				}
			}
			state = SimulationState::Dissolution;
		}
		else {
			state = SimulationState::End;
		}

		// If the visualization is active, then take in keyboard inputs and act on them
		if ( m_config.simulation.visualization ) {
			sf::Event event;
			while (window.pollEvent(event)) {
				// "close requested" event: close the window
				if (event.type == sf::Event::Closed) {
					window.close();
				}
				// "s" event: take a screenshot
				// change this to use takeScreenshot
				if (event.key.code == sf::Keyboard::S) {
					takeScreenshot(true);
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

bool Demo::addRobotWithDelay(){

	if(m_nbRobots < m_config.simulation.nb_robots){
		int final_it = int(60 * m_config.simulation.robot_delay) ; // at 60 fps
		if(m_it > final_it){
			if(m_nbRobots && m_robotController.robotStacking(m_robotController.getRobotWithId(m_nbRobots), m_config.simulation.robot_initial_posX)){
				m_robotController.setBridgeStability(false);
				return false;
			}
			m_robotController.createRobot(m_world, 0, m_config.simulation.robot_initial_posX, m_config.simulation.robot_initial_posY);
			m_it = 0;
			if(gaussian_delay){
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
				takeScreenshot(true);
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
		std::cout << "first robot created"<<std::endl;
		m_nbRobots++;
		return true;
	}
	else if(m_nbRobots < m_config.simulation.nb_robots){

		float distance = m_robotController.getRobotWithId(m_nbRobots)->getBody()->GetPosition().x;
		distance -= m_config.simulation.robot_initial_posX*m_config.robot.body_length;
		float target = m_config.simulation.robot_distance*m_config.robot.body_length;

		if(m_config.simulation.robot_phase > PI){
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
	return false;
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
	if(gaussian_delay){
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
	m_logFile << "	Bridge formation step duration: "<< std::to_string(m_elapsedTimeBridge) << " s\n\n";
	m_logFile << "	Bridge dissolution step duration: "<< std::to_string(m_elapsedTimeDissolution) << " s\n\n";
	m_logFile << "	Simulation duration: "<< std::to_string(m_elapsedTime) << " s\n\n";

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
	if(m_nbRobotsInBridge >0){
		m_logFile << "Bridge parameters: \n";
		if(m_terrain->getType()==V_TERRAIN){
			m_logFile << "	New path length: "<< std::to_string(m_length) << "\n";
			m_logFile << "	Bridge height: "<< std::to_string(m_height) << "\n";
			m_logFile << "	Bridge start: "<< std::to_string(m_startP.x) << ", "<< std::to_string(m_startP.y)<< "\n";
			m_logFile << "	Bridge end: "<< std::to_string(m_endP.x) << ", "<< std::to_string(m_endP.y)<< "\n";
		}
		m_logFile << "	Number of robots in the bridge: "<< std::to_string(m_nbRobotsInBridge) << "\n\n";
		m_logFile << "	Number of robots in bridge state: "<< std::to_string(m_nbRobotsInBridgeState) << "\n\n";

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
	std::string filename = m_config.logfile_path + m_config.logfile_name + "_bridge.txt";
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

void Demo::takeScreenshot(bool draw){

	if(draw){
		window.clear(sf::Color::White);
		m_terrain->drawBody(window);
		m_robotController.drawRobots(window, m_to_px);
	}

	if(state == SimulationState::Formation){
	    sf::Image Screen = window.capture();
	    std::string filename = m_config.logfile_path + m_config.logfile_name + "_formation_" + std::to_string(m_currentIt) + ".jpg";
	    std::cout << "[Step 1] About to save image " << filename << std::endl;
		// Screen.saveToFile("whatever.jpg");
		Screen.saveToFile(filename);
	}

	else if(state == SimulationState::Dissolution){
	    sf::Image Screen = window.capture();
	    std::string filename = m_config.logfile_path + m_config.logfile_name + "_dissolution_" + std::to_string(m_currentIt) + ".jpg";
	    std::cout << "[Step 2] About to save image " << filename << std::endl;
		Screen.saveToFile(filename);
	}

}
