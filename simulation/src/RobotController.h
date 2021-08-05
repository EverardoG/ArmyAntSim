/**
 *	@file RobotController.h
 *	@brief Implement the RobotController class. This class act as a supervisor for the Robots.
 *	@details It handles the collective behavior of the robots and the synchronization between them.
 *	The individual Robot object should be created via the RobotControlller.
 *	If it is not the case, the robot can be added afterward to the robotController via the addRobot(Robot& robot) method.
 *	@date 26 sept. 2018
 *	@author lucie houel
 */

#ifndef ROBOTCONTROLLER_H_
#define ROBOTCONTROLLER_H_

#include "Robot.h"
#include <unordered_map>

class RobotController {
public:
	RobotController();
	RobotController(config::sController controllerParam, config::sRobot robotParam, b2Body* terrain, b2Vec2 posGoal, double m_to_px = 0);
	virtual ~RobotController();

	/**
	 * Add a robot to the controller after the robot has been created.
	 * A better solution to add a robot to the simulation should be to create directly a new robot
	 * from the controller method using the createRobot method.
	 * @param robot is a pointer on a newly created robot object.
	 */
	void addRobot(Robot* robot);

	/**
	 * Check if the robot is grabbed by any other robot of the controller
	 * @param robot is the robot to be tested
	 * @return true if the robot is grabbed, false otherwise
	 */
	bool checkGrabbed(Robot& robot);

	void create(config::sController controllerParam, config::sRobot robotParam, b2Body* terrain, b2Vec2 posGoal, double m_to_px = 0);

	/**
	 * Takes the robot as parameter and create a new grip in the box2D world if the robot.isContact() is true
	 * @param 	robot generally from the m_robotVector of the controller
	 * @update 	Robot::m_previousGripJoint and Robot::m_currentGripJoint for contacted robots
	 * @update 	Robot::m_contact by setting it to false once the joint has been created
	 */
	void createGripRobots(Robot& robot);

	/**
	 * The function create a new robot object and add it to the m_robotVector
	 * @param 	world pointer on the box2D world object
	 * @param 	posX initial x position of the robot in meters
	 * @param 	posY initial y position of the robot in meters
	 * @param 	angle initial angle of the robot in rad
	 * @param 	delay number of iteration before the robot is able to move for the first time
	 * @return 	true if the robot has been created and added to m_robotVector, false otherwise
	 * */
	bool createRobot(b2World* world, int delay, double posX, double posY, double angle = 0);

	/**
	 * Remove from the world and destroy a given Box2D joint. This joint is one of a robot gripper
	 * @param 	s is either LEFT or RIGHT
	 * @update	Robot::m_previousGripJoint and Robot::m_currentGripJoint
	 */
	void destroyJoints(Robot& robot, side s);

	/**
	 * The function draws all the robots of the Robot controller on the SFML window
	 * @param 	window is the SFML window
	 * @param 	m_to_px is the conversion factor from m to pixels, it is generally obtained via the terrain
	 */
	void drawRobots(sf::RenderWindow& window, double m_to_px);

	void drawRobots(sf::RenderTexture& texture, double m_to_px);

	/**
	 * This is the function used in the contactListener.
	 * It determines if a contact must lead to a grip creation,
	 * call the Robot::gripSide(b2Contact* contact, b2Body* otherBody, double m_to_px) when a grip has to be created
	 * @param  contact is the Box2D contact object
	 * @update 	Robot::m_state for contacted robots
	 * @update 	Robot::m_leftGripJointDef or Robot::m_rightGripJointDef for future gripping robots
	 * @update 	Robot::m_contact for future gripping robots
	 */
	void findContactRobots(b2Contact* contact);

	/**
	 * Get the time required to obtain the dissolution of the bridge.
	 * The dissolution is reached when no more robots are in the bridge state
	 * @return the simulated time to reach the dissolution
	 */
	double getDissolutionTime();

	/**
	 * Get the number of active robots of the robotController.
	 * ie the number of robots in the world window
	 * @return the number of active robots
	 */
	int getNbActiveRobots();

	/**
	 * Get the number of robots of the robotController under a certain state
	 * @param s is the considered state, can be either BRIDGE or WALK
	 * @return the number of robot in this given state
	 */
	int getNbRobots(e_state s);

	/**
	 * Get the number of robot still active at the end of the simulation
	 * @return the number of robots still active at the time of the function call.
	 */
	int getNbRobotsBlocked();

	/**
	 * @param 	pos_id the robot position in the m_robotVector Robot vector containing all the robots
	 * belonging to the RobotController. The pos_id starts at 0
	 * Be careful pos_id does not necessarily match the robot id
	 * @return 	a pointer on the robot object */
	Robot* getRobot(int pos_id);

	/**
	 * @param 	id is the robot unique id
	 * Be careful, it does not necessarily match the robot position within the robots' vector.
	 * @	return a pointer on the robot object */
	Robot* getRobotWithId(int id);

	/**
	 * Get the time required to obtain a stable bridge.
	 * A stable bridge is reached when the stability condition is reached.
	 * This condition is expressed as a duration defined by m_controllerParam.stability_condition during which no more robots enter the bridge state.
	 * This duration can be changed in the main file or as a parameter in the simulation launch.
	 * @return the simulated time to reach stability
	 */
	double getStabilizationTime();

	/**
	 * Invert the moving side of the robot
	 * @param 	robot is the Robot object
	 * @update	Robot::m_movingSide: become LEFT if it was RIGHT and vice versa
	 */
	void invertMovingWheel(Robot& robot);

	/**
	 * Check if the bridge has been dissolved. We consider that the dissolution is reached if only one robot remains in the simulation.
	 * @return true if the dissolution of the bridge is obtained, false otherwise
	 */
	bool isBridgeDissolved();

	/**
	 * Check if a stable bridge has been obtained. To do so, we compare the time the last robot entered the bridge with the stability condition.
	 * The time the last robot has been obtained is stocked inside m_stableTime.
	 * m_stableTime is updated everytime a robot enter the bridge state via a call of the function setRobotState(Robot& robot,  e_state robotState);
	 * @return true if a stable bridge is obtained, false otherwise
	 */
	bool isBridgeStable();

	/**
	 * Remove robots from the controller and call its destructor.
	 * The robots that are destroyed using this method are the ones whose id has been copied
	 * inside the m_robotToDestroy vector.
	 * @update at the end all the robots whose id is in the m_robotToDestroy vector are destroyed and this vector is emptied
	 */
	void removeRobot();

	/**
	 * Check if a robot with a given unique id is outside the world.
	 * If it is the robot id is copied into the m_robotToDestroy vector to be destroyed at the next call
	 * of the removeRobot() method
	 * @param 	end_x corresponds to the end of the world
	 * @param	id is the unique id of the robot, can be obtained with robot.getID()
	 */
	void robotOut(double end_x, int id);

	/**
	 * Determine if a robot is pushing after the pushing delay has been reached
	 * by determining if the robot is touching another object.
	 * This contact has to be on the moving side.
	 * This is checked by calling the function Robot::contactOnGripSide(b2Contact* contact).
	 * In addition, the robot must have run a higher angle than m_angle_min to avoid gripping the exact previous position
	 * @param r is the Robot to be checked when its delay corresponds to the pushing_delay, if no direct joint has been created
	 * @update Robot::m_delay, Robot::m_contact, Robot::m_leftGripJointDef, Robot::m_rightGripJointDef
	 * @return true if the robot is pushing, false otherwise
	 */
	bool robotPushing(Robot& r);

	/**
	 * This function is used to help determine if the robots are stacking.
	 * It is called when a new robot will be created to check if the previous robot is too close to the new robot.
	 * The previous robot is too close when it is located before the given position posX + one bodyLength unit.
	 * @param r is the Robot to be checked
	 * @param posX is the superior limit of the position one wants to check
	 * @return true if the robot is located before posX, false otherwise
	 */
	bool robotStacking(Robot* r, float posX);

	/**
	 * Change the bridge stability flag value.
	 * @param stable is either true if the stability has been reached or false
	 * @update m_isStable
	 */
	void setBridgeStability(bool stable);

	/**
	 * Set the maximum number of robots that can be active at the same time in the simulation window.
	 * It is used to limit the size of the m_robotVector.
	 * @param 	nb_robots is the maximum number of robot that can be existing at the same time.
	 * */
	void setNbRobots(int nb_robots);

	/**
	 * Function used to set the state of a specific robot
	 * @param 	robot is the Robot object
	 * @param	robotState can be either WALK or BRIDGE
	 * @update	Robot::m_moving, Robot::m_state, Robot::m_contact and Robot::m_delay
	 */
	void setRobotState(Robot& robot,  e_state robotState);

	/**
	 * Set the scale to do the conversion from real dimensions (m) to simulated ones (pixels).
	 * Ususlly the scale is obtained from the terrain with the Terrain::getScale() method.
	 * @param 	m_to_px is the conversion factor from meters to pixels
	 * */
	void setScale(double m_to_px);

	/**
	 * Run a step of the simulation.
	 * @param 	end_x is the position of the end of the simulation world
	 * 			it generally correspond to the end of the window.
	 * 			above this position the robot is destroyed.
	 */
	void step(double end_x);

	/**
	 * Handle the waiting delay of the robots by decreasing it at each world step.
	 * When the delay is null, the robot.m_ready state is set to true
	 * @param 	robot generally from the m_robotVector of the controller
	 * @update	Robot::m_ready is set to true when Robot::m_delay is null
	 * @update	Robot::m_delay is decreased otherwise
	 */
	void wait_delay(Robot& robot);

	void SetGlobalSpeed(double desired_speed);

	/**
	 *
	*/
	void calculateSpeedsToGoal(b2Vec2 m_goal_pos);

	b2Vec2 getAvgPos();

	/**
	 * Flag used to notify when a new robot enter the bridge state.
	 * It is used to write a new entry in the bridge file.
	 * It is set to false once the bridge file have been updated (in the demo.cpp file).
	 */
	bool m_bridgeEntry = false;

	int getNbRobotsReachedGoal();

private:

	std::vector<Robot*> m_robotVector; // Vector containing a pointer on every robot object attached to this robot controller
	std::vector<int> m_robotToDestroy; // Vector containing the id of all the robot that have to be destroyed during the next simulation step
	int m_nbRobots = 0; // Number of robot attached to this robot controller
	int m_currentIt = 0; // Current iteration
	int m_idLastRobot = 0; // id of the last robot that entered the bridge state
	int m_nbRobInBridge = 0; // Number of robot in the bridge state
	double m_stableTime = 0; // Time to reach stable bridge (real time)
	bool m_isStable = false; // Flag to indicate bridge stability
	bool m_isDissolved = false; // Flag to indicate if the bridge dissolved
	double m_dissolutionTime = 0; // Time to reach bridge dissolution (real time)

	/** Angle minimum (rad) the robot has to run to be able to grab. Below this angle the robot cannot create a grip even after the pushing delay.
	 * This angle prevent the robot to grip again its previous position.
	 * Rq: This angle could be adapted to fit 1 or 2 times the angular resolution of the robot rotation
	 * which is determined by the ratio between the time step and the robot angular speed: angular_resolution = angular_speed (rad/s) / time_step (s)
	 */
	double m_angle_min= PI/8;

	b2Body* m_terrainBody=nullptr; //Pointer on the terrain Box2D body
	b2Vec2 m_posGoal; // Position of the goal set in the terrain

	config::sController m_controllerParam; //Structure containing the controller parameters. The structure is defined in the Config.h file
	config::sRobot m_robotParam; //Structure containing the robot parameters. The structure is defined in the Config.h file

	int m_M_TO_PX; //Copy of the scale to do the conversion from real dimensions (m) to simulated ones (pixels)
	std::unordered_map<int, bool> m_robots_reached_goal; // ids of all the robots that have passed the goal (if one was set)
};

#endif /* ROBOTCONTROLLER_H_ */
