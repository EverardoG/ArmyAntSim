/*
 * RobotController.cpp
 *
 *  Created on: 28 sept. 2018
 *      Author: lucie
 *
 *      29 Jan. 2019:
 *      	- changed the constructor and the create method to add a pointer on the terrain m_terrainBody
 *      	- in the robotPushing(Robot& r) method, added a check to pass the eventual other (contacted) robot in bridge state
 *				if (! (contactList->other==m_terrainBody)){
 *					Robot* r_other = static_cast<Robot*>( contactList->other->GetUserData() );
 *					setRobotState(*r_other, BRIDGE);
 *				}
 *      13 Feb. 2019:
 *      	- changed the way the bridge dissolution is checked to be more efficient:
 *      		-added members : m_nbRobInBridge updated each time a new robot enter or leave the bridge (in setRobotState(.) and step(.))
 *
 *      	- Improved the contact handler findContactRobots(b2Contact* contact) by changing the way we determine which robots are involved:
 *      	  instead of looping over the robots of the robotController, call getUserData() directly from the contacted body
 *
 *
 *
 */

#include "RobotController.h"
#include "Box2D/Box2D.h"
#include "constants.h"
#include "helpers.h"
#include <iostream>

RobotController::RobotController() {
}

RobotController::RobotController(config::sController controllerParam, config::sRobot robotParam, b2Body* terrain, b2Vec2 posGoal, double m_to_px) {
	m_controllerParam = controllerParam;
	m_robotParam = robotParam;
	if (!controllerParam.infinite_robots) {m_robotVector.reserve(controllerParam.max_robot_window); }
	m_M_TO_PX = m_to_px;
	printf("size of vector %ld, \n", m_robotVector.size());
	m_terrainBody = terrain;
	m_posGoal = posGoal;
}

void RobotController::create(config::sController controllerParam, config::sRobot robotParam, b2Body* terrain, b2Vec2 posGoal, double m_to_px) {
	m_controllerParam = controllerParam;
	m_robotParam = robotParam;
	if (!controllerParam.infinite_robots) {m_robotVector.reserve(controllerParam.max_robot_window); }
	m_M_TO_PX = m_to_px;
	printf("size of vector %ld, \n", m_robotVector.size());
	m_terrainBody = terrain;
	m_posGoal = posGoal;
	std::cout << "initial sigma: " << m_controllerParam.sigma << std::endl;
	generator.seed(controllerParam.random_seed);
	n_distribution = std::normal_distribution<float>(0.0, m_controllerParam.sigma);
	u_distribution = std::uniform_real_distribution<float>(0.0, 2*PI);
}

RobotController::~RobotController() {
	// TODO Auto-generated destructor stub
	int i = 0 ;
	for (i=0; i<m_robotVector.size(); i++){
//		if(!(m_robot[i].m_previousGripJoint==nullptr)){
//			m_robot[i].getBody()->GetWorld()->DestroyJoint(m_robot[i].m_previousGripJoint);
			m_robotVector[i]->m_previousGripJoint = nullptr;
//		}
//		if(!(m_robot[i].m_currentGripJoint==nullptr)){
//			m_robot[i].getBody()->GetWorld()->DestroyJoint(m_robot[i].m_currentGripJoint);
			m_robotVector[i]->m_currentGripJoint = nullptr;
//		}
	}
}

void RobotController::setNbRobots(int nb_robots){
	m_controllerParam.max_robot_window = nb_robots;
	m_robotVector.reserve(nb_robots);
}

void RobotController::setScale(double m_to_px){
	m_M_TO_PX = m_to_px;
}

bool RobotController::createRobot(b2World* world, int delay, double posX, double posY, double angle){

	if (!m_controllerParam.infinite_robots && m_robotVector.size() >= m_controllerParam.max_robot_window){
		printf("the max number of robot in the screen has already been reached");
		return false;
	}
	else{
		Robot *r = new Robot();
		m_robotVector.push_back(r);
//		(world, posX, posY)
		m_nbRobots ++;
		m_robotVector[m_robotVector.size()-1]->createBody(world, m_robotParam, posX, posY, angle);
		m_robotVector[m_robotVector.size()-1]->setId(m_nbRobots);
		m_robotVector[m_robotVector.size()-1]->setDelay(delay);
		m_robotVector[m_robotVector.size()-1]->m_age = m_currentIt;
		// printf("Robot created \n");
		return true;
	}
}

Robot* RobotController::getRobot(int pos_id){

	if (pos_id > m_robotVector.size()-1){
		throw std::string("you have only " + std::to_string(m_robotVector.size())
							+ " active robots, you cannot access robot "+ std::to_string(pos_id));
	}
	else{
		return m_robotVector[pos_id];
	}

}

Robot* RobotController::getRobotWithId(int id){
	for (int i=0; i<m_robotVector.size(); i++){
		if(m_robotVector[i]->getId()==id){
			return m_robotVector[i];
		}
	}
	return nullptr;
}

void RobotController::drawRobots(sf::RenderWindow& window, double m_to_px){
	int i =0;
	for (i=0; i<m_robotVector.size(); i++){
		m_robotVector[i]->drawBody(window, m_to_px);
		m_robotVector[i]->drawGripJoint(window, m_to_px);
	}
}

void RobotController::drawRobots(sf::RenderTexture& texture, double m_to_px){
	int i =0;
	for (i=0; i<m_robotVector.size(); i++){
		m_robotVector[i]->drawBody(texture, m_to_px);
		m_robotVector[i]->drawGripJoint(texture, m_to_px);
	}
}


void RobotController::findContactRobots(b2Contact* contact){
	// std::cout << "findContactRobots()" << std::endl;
	int i =0;
	b2Body* bodyContactA = contact->GetFixtureA()->GetBody();
	b2Body* bodyContactB = contact->GetFixtureB()->GetBody();
	Robot* rA = nullptr;
	Robot* rB = nullptr;

	bool robotA = false;
	bool robotB = false;

	bool contactorA = false;
	bool contactorB = false;

	if(!(bodyContactA == m_terrainBody)){
		// printf("!(bodyContactA == m_terrainBody)\n");
		robotA = true;
		rA=static_cast<Robot*>(bodyContactA->GetUserData());
	}

	if(!(bodyContactB == m_terrainBody)){
		// printf("!(bodyContactB == m_terrainBody)\n");
		robotB = true;
		rB=static_cast<Robot*>(bodyContactB->GetUserData());
	}

	if(robotA){
		// printf("robotA\n");
		contactorA = (rA->isMoving()||rA->m_start)
				   && (rA->contactOnGripSide(contact));//||m_robotVector[A].m_start);
	}

	if(robotB){
		// printf("robotB\n");
		bool moving = rB->isMoving();
		bool start = rB->m_start;
		bool cont = rB->contactOnGripSide(contact);
		// std::cout << "rB->isMoving(): " << moving << " | rB->m_start: " << start << " | rB->contactOnGripSide(contact): " << cont << std::endl;
		contactorB = (moving||start)
				    && (cont);
		// contactorB = (rB->isMoving()||rB->m_start)
		// 		    && (rB->contactOnGripSide(contact));//||m_robotVector[A].m_start);
	}

	if (robotA && robotB && (rA==rB)){
		// printf("contact within robot\n");
		return;
	}

	// start Finite-state machine
	// Looks like this is where logic for whether or not to setup grip joints happens
	if (contactorA && contactorB){
		// std::cout << "A && B" << std::endl;

		double angleA = rA->getBody()->GetAngle() - rA->m_referenceAngle;

		double angleB = rB->getBody()->GetAngle() - rB->m_referenceAngle;

		if(abs(angleA) > m_controllerParam.angle_limit && abs(angleB) > m_controllerParam.angle_limit){
			// If two robots make contact and can grip, then whichever robot is on the bottom goes into bridge state
			// Robot A is on top
			if (rA->getPosition().y < rB->getPosition().y && rA->getState() != BRIDGE) {
				// Robot A grips and robot B goes into bridge state
				rA->gripSide(contact, bodyContactB, m_M_TO_PX);
				rA->setContact(true);
				// setRobotState(*rA, WALK);
				setRobotState(*rB, BRIDGE);
			}
			// Robot B is on top
			else if (rB->getPosition().y < rA->getPosition().y && rB->getState() != BRIDGE) {
				// Robot B grips and robot A goes into bridge state
				rB->gripSide(contact, bodyContactA, m_M_TO_PX);
				rB->setContact(true);
				setRobotState(*rA, BRIDGE);
				// setRobotState(*rB, WALK);
			}
			// rA->gripSide(contact, bodyContactB, m_M_TO_PX);
			// rA->setContact(true);
			// rB->gripSide(contact, bodyContactA, m_M_TO_PX);
			// rB->setContact(true);
			// setRobotState(*rA, BRIDGE);
			// setRobotState(*rB, BRIDGE); //TODO becareful changed rule
		}
		else if(abs(angleA) > m_controllerParam.angle_limit && rA->getState() != BRIDGE){
			rA->gripSide(contact, bodyContactB, m_M_TO_PX);
			rA->setContact(true);
			// setRobotState(*rA, WALK); //WALK
			setRobotState(*rB, BRIDGE); //TODO becareful changed rule
		}

		else if(abs(angleB) > m_controllerParam.angle_limit && rB->getState() != BRIDGE){
			rB->gripSide(contact, bodyContactA, m_M_TO_PX);
			rB->setContact(true);
			setRobotState(*rA, BRIDGE);
			// setRobotState(*rB, WALK); //WALK
		}

	}

	else if (contactorB && rB->getState() != BRIDGE){
		// std::cout << "B" << std::endl;

		double angleB = rB->getBody()->GetAngle() - rB->m_referenceAngle;
		// if(abs(angleB) > m_controllerParam.angle_limit|| angleB==0){
			// if (m_robotVector[B].m_start){m_robotVector[B].m_start = false;}
			rB->gripSide(contact, bodyContactA, m_M_TO_PX);
			setRobotState(*rB, WALK);

			if (robotA){
				setRobotState(*rA, BRIDGE);
			}
		// }
	}

	else if (contactorA && rA->getState() != BRIDGE){
		// std::cout << "A" << std::endl;

		double angleA = rA->getBody()->GetAngle() - rA->m_referenceAngle;
//		std::cout<<"angle robot: "<<moduloAngle(m_robotVector[A].getBody()->GetAngle())*RAD_TO_DEG<<std::endl;
//		std::cout<<"angle A: "<<angleA*RAD_TO_DEG<<std::endl;

		// if(abs(angleA) > m_controllerParam.angle_limit|| angleA==0){
	//		if (m_robotVector[A].m_start){m_robotVector[A].m_start = false;}
			rA->gripSide(contact, bodyContactB, m_M_TO_PX);
			setRobotState(*rA, WALK);

			if (robotB){
				setRobotState(*rB, BRIDGE);
			}
		// }
	}
	else {
		// std::cout << "No valid contacts" << std::endl;
	}
	//printf("end of contact\n");
}

void RobotController::setRobotState(Robot& robot, e_state robotState){

	e_state S = robotState;
	switch(S){
		case WALK:

		    if(!(robot.m_start)){
			robot.setDelay(int(m_controllerParam.walk_delay*FPS));
		    }
			robot.m_moving = false;
			robot.setContact(true);
			robot.setState(WALK);
//			m_bridgeEntry = true;
//			robot.setSpeed(2*PI);
//			robot.m_referenceAngle = robot.getMotor(robot.m_movingSide)->GetJointAngle();
//			robot.allowMotorRotation(LEFT);

//			robot.blockMotorRotation(LEFT);
//			robot.blockMotorRotation(RIGHT);

//			robot.turnOffMotor(LEFT);
//			robot.turnOffMotor(RIGHT);
			//printf("case WALK, \n");
			break;

		case BRIDGE:
			robot.setDelay(int(m_controllerParam.bridge_delay*FPS));
			robot.m_moving = false;
			m_bridgeEntry = true;

			if (robot.getId() > m_idLastRobot){
				m_idLastRobot = robot.getId();
				m_stableTime =  m_currentIt/FPS;
			}
			if (!(robot.getState()== BRIDGE)){
				m_nbRobInBridge++;
				robot.m_bridgeAge = m_currentIt;

				if(!robot.getMotor(robot.m_movingSide)){
					printf("Pourquoi putain de nul \n");
				}

//				robot.blockMotorRotation(LEFT);
//				robot.blockMotorRotation(RIGHT);

				// TODO: DONT HARDCODE THIS!!!
				float motor_limit_deg = 5;
				robot.limitMotorRotation(LEFT, motor_limit_deg/RAD_TO_DEG);
				robot.limitMotorRotation(RIGHT, motor_limit_deg/RAD_TO_DEG);

				/*works even when consider the motor of the moving wheel instead of the one of the wheel that is attached because of the condition
				* that the wheels cannot rotate --> abs(angle of left wheel) == abs(angle of right wheel)*/
//				if(robot.getMotor(robot.m_movingSide)->GetJointAngle()){
//					robot.m_referenceAngleJoint = robot.getMotor(robot.m_movingSide)->GetJointAngle();
//				}
//				else{
//					robot.m_referenceAngleJoint = robot.getBody()->GetAngle();
//				}
//				robot.limitMotorRotation(robot.m_movingSide,30/RAD_TO_DEG);
				robot.setState(BRIDGE);
			}

			break;
	}
}



void RobotController::createGripRobots(Robot& robot){
	if (robot.isContact()){
		if(robot.m_start){
			robot.m_start = false;
		}
		robot.setContact(false);
		side s = robot.m_movingSide;
		b2World* world = robot.getBody()->GetWorld();
		if(robot.nbJoint(s)== 0){

			b2PrismaticJointDef gripJointDef= robot.getJointDef(s);
//			printf("create grip 1\n");
			if((&gripJointDef == nullptr)){
				return;
			}
			b2PrismaticJoint* gripJoint = static_cast<b2PrismaticJoint*>(world->CreateJoint( &gripJointDef ));
//			printf("create grip 2\n");
//			robot.setContact(false);
			robot.incrementNbJoint(s);

//				robot.m_referenceAngle = robot.getMotor(robot.m_movingSide)->GetJointAngle();
			robot.m_previousGripJoint = robot.m_currentGripJoint;
			robot.m_currentGripJoint = gripJoint;
			robot.m_referenceAngle = robot.getBody()->GetAngle();

		}
		else{
			printf("already a joint \n");
		}
	}
}

void RobotController::wait_delay(Robot& robot){
	if (robot.getDelay()==0){// && (!robot.isMoving())){
		robot.m_ready = true;
	}

	else {
		//robot.m_ready = false;
		//printf("delai: %d tick left\n", robot.getDelay());
		int delay = robot.getDelay()-1;
		robot.setDelay(delay);
		//printf("delai: %d tick left\n", robot.getDelay());
	}

}

void RobotController::addRobot(Robot* robot){
	m_robotVector.push_back(robot);
}

void RobotController::removeRobot(){
	if(!m_robotToDestroy.empty()){
		// printf("remove 0 \n");

		for (int i=0; i<m_robotToDestroy.size(); i++){
			// printf("m_robotToDestroy.size(): %ld\n", m_robotToDestroy.size());
			// printf("Grabbing id for i: %d\n", i);
			int id = m_robotToDestroy[i];
			// printf("id is %d\n", id);
			std::vector<Robot*>::iterator pRob =  m_robotVector.begin()+id;

			// printf("Setup pRob\n");
			// This if statement is a quick-fix. Not sure why pRob is ever equal to
			// m_robotVector.end() in the first place. It's a rare case that happens
			// right at the end of the simulator and causes a segfault without this
			// check
			if (pRob != m_robotVector.end()) delete *pRob;
			// printf("Deleted pRob\n");
			if(!(id == m_robotVector.size()-1)){
				// printf("remove 0b \n");
				std::swap(m_robotVector[id], m_robotVector.back()); // check if same ?
				// printf("remove 0c \n");
			}

			// printf("remove 1 \n");
			m_robotVector.pop_back();
			// printf("remove 2 \n");

		}
		// printf("About to clear m_robotToDestroy\n");
		m_robotToDestroy.clear();
		// printf("Successfully destroyed robot\n");
	}
}

void RobotController::robotOut(int left_x, int right_x, int top_y, int bottom_y, int id){
// 	int pos = (m_robotVector[id]->getBody()->GetWorldCenter()).x * m_M_TO_PX;
// 	// std::cout << "pos: " << pos << " | end_x: " << end_x << std::endl;
// 	if(pos > end_x){
// 		// std::cout << "Planning to delete Robot " << id << std::endl;
// 		m_robotToDestroy.push_back(id);
// //		removeRobot(id);
// 	}

	// Remove the robot if it is out of bounds of the simulation
	int pos_x = (m_robotVector[id]->getBody()->GetWorldCenter()).x * m_M_TO_PX;
	int pos_y = (m_robotVector[id]->getBody()->GetWorldCenter()).y * m_M_TO_PX;

	if (pos_x < left_x || pos_x > right_x || pos_y < top_y || pos_y > bottom_y) {
		m_robotToDestroy.push_back(id);
	}
}

void RobotController::invertMovingWheel(Robot& robot){
	if(robot.m_movingSide == LEFT){
		robot.m_movingSide = RIGHT;
	}
	else if(robot.m_movingSide == RIGHT){
		robot.m_movingSide = LEFT;
	}
//	printf("invert moving whell \n \n");
}

void RobotController::destroyJoints( Robot& robot, side s){
//	printf("controller.destroyJoint \n");
//	if(!(robot.m_previousGripJoint == nullptr)){
	if(robot.nbJoint(s)>0){
	if(robot.m_previousGripJoint){
		robot.getBody()->GetWorld()->DestroyJoint(robot.m_previousGripJoint);
		robot.m_previousGripJoint = nullptr;
		robot.resetNbJoint(s);
//		printf("a joint has been deleted \n");
	}
	}
}

bool RobotController::checkGrabbed(Robot& robot){
	if(robot.isGrabbed()){
		robot.setDelay(int(m_controllerParam.bridge_delay*60));
		robot.setState(BRIDGE);
		return true;
	}
	return false;
}

bool RobotController::isBridgeStable(){
	if (m_stableTime > 0){
		if (m_currentIt/FPS > (m_stableTime + m_controllerParam.stability_condition)){
				return true;
			}
	}
	return false;
}

bool RobotController::isBridgeDissolved(){
	if (getNbRobots(BRIDGE) < 1 && !m_isDissolved){
		m_isDissolved = true;
		return true;
	}
	return false;
}

double RobotController::getStabilizationTime(){
	return m_stableTime;
}

double RobotController::getDissolutionTime(){
	return m_dissolutionTime;
}

// Set speed for all robots
void RobotController::SetGlobalSpeed(double desired_speed){
	for (int i=0; i<m_robotVector.size(); i++){
		if (m_robotVector[i]->m_regrip_state == true) {
			m_robotVector[i]->m_speed_before_regrip = desired_speed;
		}
		else {
			m_robotVector[i]->setSpeed(desired_speed);
		}
	}
}

// float RobotController::calculateSpeed( robot, goal_pos )
// or goal_pos could be an attribute of RobotController
// and it's public and it's defined in config and set by Demo.cpp
// also option for dynamic speed vs fixed speed in config
// maybe change delay instead of speed (this is more portable for real robot)

b2Vec2 RobotController::perturbGoal(b2Vec2 goal_pos) {
	if (m_controllerParam.sigma == 0.0) {
		b2Vec2 perturbed_goal = b2Vec2(goal_pos.x, goal_pos.y);
		return perturbed_goal;
	}
	// Generate random number for radius of perturbation
	float r = n_distribution(generator);
	// Generate angle for perturbation
	float theta = u_distribution(generator);
	// std::cout << "r: " << r << " | theta: " << theta << std::endl;
	// Create new goal position as perturbed goal position
	b2Vec2 perturbed_goal = b2Vec2(goal_pos.x+r*cos(theta), goal_pos.y+r*sin(theta));
	return perturbed_goal;
}

void RobotController::calculateSpeedsToGoal(b2Vec2 m_goal_pos, float m_elapsedTime, std::string simulation_state){
	if (m_controllerParam.control_policy == "s_curve") {
		// Calculates the speed for every robot so that the robots move toward the desired goal
		for (int i=0; i<m_robotVector.size(); i++){
			float xd = m_goal_pos.x + m_controllerParam.param3;
			float k = m_controllerParam.param1;
			float final_speed = m_controllerParam.param2;
			float L = 2*3.14; // m_robotParam.speed;
			float x0 = xd - 1/k * log(L/final_speed-1);
			float desired_speed = L/(1+pow(2.72, k*(m_robotVector[i]->getPosition().x - x0)));
			float desired_y = m_goal_pos.y;
			if(m_robotVector[i]->getPosition().y < desired_y && m_robotVector[i]->getPosition().x > xd-2*m_robotParam.body_length){
				desired_speed = m_robotParam.speed;
			}

			m_robotVector[i]->setSpeed(desired_speed);
		}
	}
	else if (m_controllerParam.control_policy == "reactivebuild_no_comms") {
		// std::cout << "reactivebuild" << std::endl;
		for (int i=0; i<m_robotVector.size(); i++) {
			// start w. robot's current speed
			float new_speed = m_robotVector[i]->getSpeed();
			if (m_robotVector[i]->getId() == 9) {
				std::cout << "Robot pos: (" << m_robotVector[i]->getPosition().x << "," << m_robotVector[i]->getPosition().y << ")" << " | Goal Pos: (" << m_goal_pos.x << "," << m_goal_pos.y << ")" << std::endl;
			}
			// If the robot already reached the goal, then move at the constant speed
			if(m_robotVector[i]->getPosition().y < m_goal_pos.y && m_robotVector[i]->getPosition().x > m_goal_pos.x){
				new_speed = m_robotParam.speed;
			}
			// Otherwise, calculate the robot speed
			// On first iteration, just go at constant speed
			// "First iteration" is any iteration before last_position is updated
			// On subsequent iterations, only flip forward if you're moving towards the goal
			// Only change speed every m_pos_update_time
			// Also, if the robot stopped, leave it stopped
			else if ((m_elapsedTime - m_robotVector[i]->m_last_position_time_update) > m_robotVector[i]->m_pos_update_time && new_speed != 0) {
				b2Vec2 prev_vector = m_robotVector[i]->m_last_position - m_goal_pos;
				float prev_distance = pow( pow(prev_vector.x, 2.0) + pow(prev_vector.y, 2.0), 0.5);
				b2Vec2 curr_vector = m_robotVector[i]->getPosition() - m_goal_pos;
				float curr_distance = pow( pow(curr_vector.x, 2.0) + pow(curr_vector.y, 2.0), 0.5);
				// If robot is moving closer to goal...
				if (curr_distance < prev_distance) {
					new_speed = m_robotParam.speed;
				}
				else {
					new_speed = 0.0;
				}
				// Update last position
				m_robotVector[i]->m_last_position = m_robotVector[i]->getPosition();
				// m_robotVector[i]->m_first_iteration = false;
				m_robotVector[i]->m_last_position_time_update = m_elapsedTime;
				// if (m_robotVector[i]->getId() == 9) {
				// 	std::cout << "prev: " << prev_distance << " | curr: " << curr_distance << std::endl;
				// 	std::cout << "Update last position | m_elapsedTime: " << m_elapsedTime << std::endl;
				// 	std::cout << "speed: " << new_speed << std::endl;
				// }
			}
			m_robotVector[i]->setSpeed(new_speed);
		}
	}
	else if (m_controllerParam.control_policy == "dynamic_reactivebuild_no_comms") {
		for (int i=0; i<m_robotVector.size(); i++) {
			// Grab a perturbed goal
			b2Vec2 measured_goal_pos = perturbGoal(m_goal_pos);
			// Unless sigma is set to 0. Then reset this to the actual goal position.
			if (m_controllerParam.sigma == 0.0) {
				measured_goal_pos = m_goal_pos;
			}
			// start w. robot's current speed
			float new_speed = m_robotVector[i]->getSpeed();
			// If the robot already reached the goal, then move at the constant speed
			if(m_robotVector[i]->getPosition().y < measured_goal_pos.y && m_robotVector[i]->getPosition().x > measured_goal_pos.x){
				new_speed = m_robotParam.speed;
			}
			// Otherwise, calculate the robot speed
			else if ((m_elapsedTime - m_robotVector[i]->m_last_position_time_update) > m_robotVector[i]->m_pos_update_time && new_speed != 0) {
				// std::cout << "r.x: " << m_robotVector[i]->getPosition().x << " | r.y: " << m_robotVector[i]->getPosition().y << std::endl;
				b2Vec2 prev_vector = m_robotVector[i]->m_last_position - measured_goal_pos;
				float prev_distance = pow( pow(prev_vector.x, 2.0) + pow(prev_vector.y, 2.0), 0.5);
				b2Vec2 curr_vector = m_robotVector[i]->getPosition() - measured_goal_pos;
				// std::cout << "d.x: " << curr_vector.x << " | d.y: " << curr_vector.y << std::endl;
				float curr_distance = pow( pow(curr_vector.x, 2.0) + pow(curr_vector.y, 2.0), 0.5);
				// std::cout << "prev: " << prev_distance << " | curr: " << curr_distance << std::endl;
				// new_speed = m_controllerParam.param1 * (prev_distance - curr_distance);
				std::cout << "new_speed: " << new_speed << std::endl;
				float prev_speed_sign = 1;
				if (new_speed < 0) { prev_speed_sign = -1; }
				std::cout << "prev_speed_sign: " << prev_speed_sign << std::endl;
				float prev_curr_sign = 1;
				if (prev_distance - curr_distance < 0) {prev_curr_sign = -1;}
				std::cout << "prev-curr: " << prev_distance - curr_distance << std::endl;
				new_speed = prev_curr_sign * prev_speed_sign * 1.5*m_robotParam.speed/(1+pow(2.72, -m_controllerParam.param1*(prev_distance - curr_distance))) - m_robotParam.speed/2;
				if (m_robotVector[i]->getId() == 1) {
					std::cout << "x: " << (prev_distance-curr_distance) << " | y: " << new_speed << std::endl;
					// std::cout << "prev: " << prev_distance << " | curr: " << curr_distance << std::endl;
					// std::cout << "Update last position | m_elapsedTime: " << m_elapsedTime << std::endl;
					// std::cout << "Goal x: " << measured_goal_pos.x << " | Goal y: " << measured_goal_pos.y << " | sigma: " << m_controllerParam.sigma << std::endl;
				}
				// // If robot is moving closer to goal...
				// if (curr_distance < prev_distance) {
				// 	// new_speed = m_controllerParam.param1 * (prev_distance - curr_distance);
				// 	new_speed = m_robotParam.speed/(1+pow(2.72, -m_controllerParam.param1*(prev_distance - curr_distance)));
				// 	if (m_robotVector[i]->getId() == 1) {
				// 		std::cout << "x: " << (prev_distance-curr_distance) << " | y: " << new_speed << std::endl;
				// 	}
				// }
				// else {
				// 	new_speed = 0.0;
				// }
				// Update last position
				m_robotVector[i]->m_last_position = m_robotVector[i]->getPosition();
				m_robotVector[i]->m_last_position_time_update = m_elapsedTime;
			}
			// std::cout << "s: " << new_speed << std::endl;
			m_robotVector[i]->setSpeed(new_speed);
		}
	}
	else if (m_controllerParam.control_policy == "slowdown_away_from_goal") {
		for (int i=0; i<m_robotVector.size(); i++) {
			// Grab a perturbed Goal
			b2Vec2 measured_goal_pos = perturbGoal(m_goal_pos);
			// start w. robot's current speed
			float new_speed = m_robotVector[i]->getSpeed();
			bool prev_regrip_state = m_robotVector[i]->m_regrip_state;
			// Robot is trying to regrip
			if (m_robotVector[i]->m_regrip_state) {
				// std::cout << "Attempting regrip from " << m_robotVector[i]->getId() << std::endl;
				// Stop trying to regrip if enough time has passed since start of regrip
				if ((m_elapsedTime - m_robotVector[i]->m_regrip_start_time) >= m_robotVector[i]-> m_regrip_duration) {
					// std::cout << "m_elapsedTime: " m_elapsedTime << " "
					// std::cout << "Ending regrip attempt from " << m_robotVector[i]->getId() << std::endl;
					m_robotVector[i]->m_regrip_state = false;
					new_speed = m_robotVector[i]->m_speed_before_regrip;
					m_robotVector[i]->m_last_regrip_attempt = m_elapsedTime;
				}
				// Update last position and time
				m_robotVector[i]->m_last_position = m_robotVector[i]->getPosition();
				m_robotVector[i]->m_last_position_time_update = m_elapsedTime;
			}
			// Robot is in walking state and enough time has passed that its time for a speed update OR robot just stopped trying to regrip
			else if ( m_robotVector[i]->getState() == WALK    // Robot is walking
				&& (// Robot is not trying to regrip and enough time has passed for an update
				(m_robotVector[i]->m_regrip_state == false && (m_elapsedTime - m_robotVector[i]->m_last_position_time_update) >= m_robotVector[i]->m_pos_update_time))){
				// || // Robot just stopped trying to regrip
				// (prev_regrip_state==true && m_robotVector[i]->m_regrip_state==false))) {
				// std::cout << "Robot is walking " << std::endl;
				// Check if robot is stuck and initiate a regrip
				if ( abs(m_robotVector[i]->m_last_position.x - m_robotVector[i]->getPosition().x) < 0.25
					&& abs(m_robotVector[i]->m_last_position.y - m_robotVector[i]->getPosition().y) < 0.25
					&& m_robotVector[i]->getSpeed() != 0.0
					&& (m_elapsedTime - m_robotVector[i]->m_last_regrip_attempt) > m_robotVector[i]->m_regrip_retry_delay) {
					// std::cout << "Initiating regrip | Id: " << m_robotVector[i]->getId() <<
					// 			" | Last time: " << m_robotVector[i]->m_last_position_time_update <<
					// 			" | Current time: " << m_elapsedTime <<
					// 			" | Diff x: " << abs(m_robotVector[i]->m_last_position.x - m_robotVector[i]->getPosition().x) <<
					// 			" | Diff y: " << abs(m_robotVector[i]->m_last_position.y - m_robotVector[i]->getPosition().y) <<
					// 			" | m_robotVector[i]->getSpeed()" << m_robotVector[i]->getSpeed() <<
					// 			" | m_elapsedTime: " << m_elapsedTime <<
					// 			std::endl;
					// Grab speed before regrip state
					m_robotVector[i]->m_speed_before_regrip = new_speed;
					// Set speed to be slow in opposite direction
					if (new_speed < 0) {
						new_speed = 0.3;
					}
					else {
						new_speed = -0.3;
					}
					// Set regrip state
					m_robotVector[i]->m_regrip_state = true;
					m_robotVector[i]->m_regrip_start_time = m_elapsedTime;
					// Update last position and time
					m_robotVector[i]->m_last_position = m_robotVector[i]->getPosition();
					m_robotVector[i]->m_last_position_time_update = m_elapsedTime;
				}
				// Update speeds dynamically if simulation is not yet in dissolution state
				else if (simulation_state != "dissolution") {
					// Robot has reached the goal
					if(m_robotVector[i]->getPosition().y < measured_goal_pos.y && m_robotVector[i]->getPosition().x > measured_goal_pos.x){
						new_speed = m_robotParam.speed;
					}
					// Robot has not reached the goal
					else {
						// Calculate distance to goal since last time
						b2Vec2 prev_vector = m_robotVector[i]->m_last_position - measured_goal_pos;
						float prev_distance = pow( pow(prev_vector.x, 2.0) + pow(prev_vector.y, 2.0), 0.5);
						b2Vec2 curr_vector = m_robotVector[i]->getPosition() - measured_goal_pos;
						float curr_distance = pow( pow(curr_vector.x, 2.0) + pow(curr_vector.y, 2.0), 0.5);
						// Slowdown if the robot is moving away from the goal. Otherwise speed up
						if (curr_distance > prev_distance) {
							if (new_speed > 0) {
								new_speed = new_speed - m_controllerParam.param1 * (curr_distance - prev_distance);
							}
							else if (new_speed < 0) {
								new_speed = new_speed + m_controllerParam.param1 * (curr_distance - prev_distance);
							}
						}
					}
					m_robotVector[i]->m_last_position = m_robotVector[i]->getPosition();
					m_robotVector[i]->m_last_position_time_update = m_elapsedTime;
				}
				// Set speed to global speed if simulation is in dissolution state
				else {
					new_speed = m_robotParam.speed;
					m_robotVector[i]->m_last_position = m_robotVector[i]->getPosition();
					m_robotVector[i]->m_last_position_time_update = m_elapsedTime;
				}
				if (m_robotVector[i]->getId() == 12){
					// std::cout << "new_speed: " << new_speed << " | Id: " << m_robotVector[i]->getId() << std::endl;
				}
			}
			// Enfore speed limits
			if (new_speed > m_robotParam.speed) {
				new_speed = m_robotParam.speed;
			}
			else if (new_speed < -m_robotParam.speed) {
				new_speed = m_robotParam.speed;
			}
			// Actually set the robot to the new speed
			m_robotVector[i]->setSpeed(new_speed);
			if (m_robotVector[i]->getId() == 5 || m_robotVector[i]->getId() == 15) {
				// std::cout << "Actual speed " << m_robotVector[i]->getSpeed() <<
				// " | Id: " << m_robotVector[i]->getId() <<
				// " | m_elapsedTime " << m_elapsedTime << std::endl;
			}
		}
	}
}
			// // Continue regrippping if the robot is trying to regrip
			// if (m_robotVector[i]->m_regrip_state && (m_elapsedTime - m_robotVector[i]->m_regrip_start_time) >= m_robotVector[i]-> m_regrip_duration ) {
			// 	m_robotVector[i]->m_regrip_state = false;
			// 	// Update last position
			// 	m_robotVector[i]->m_last_position = m_robotVector[i]->getPosition();
			// 	m_robotVector[i]->m_last_position_time_update = m_elapsedTime;
			// 	std::cout << "Regrip attempt finished from " << m_robotVector[i]->getId() << std::endl;
			// }
			// // Attempt to regrip if the robot is stuck. Move opposite direction for a set time. Before going back to default behavior.
			// if ( abs(m_robotVector[i]->getPosition().y - m_robotVector[i]->m_last_position.y) <= 0.025
			//     && abs(m_robotVector[i]->getPosition().x - m_robotVector[i]->m_last_position.x) <= 0.025
			// 	&& (m_elapsedTime - m_robotVector[i]->m_last_position_time_update) >= m_robotVector[i]->m_pos_update_time
			// 	&& m_robotVector[i]->getState() == WALK) {
			// 		std::cout << "Regrip attempt triggered from " << m_robotVector[i]->getId() << std::endl;
			// 		if (new_speed >= 0) {
			// 			new_speed = -0.5;
			// 		}
			// 		else {
			// 			new_speed = 0.5;
			// 		}
			// 		// Robot is now trying to regrip
			// 		m_robotVector[i]->m_regrip_start_time = m_elapsedTime;
			// 		m_robotVector[i]->m_regrip_state = true;
			// 		// Update last position
			// 		m_robotVector[i]->m_last_position = m_robotVector[i]->getPosition();
			// 		m_robotVector[i]->m_last_position_time_update = m_elapsedTime;
			// 	}
			// If the robot already reached the goal, then move at the constant speed
			// else if(m_robotVector[i]->getPosition().y < measured_goal_pos.y && m_robotVector[i]->getPosition().x > measured_goal_pos.x){
			// 	new_speed = m_robotParam.speed;
			// }
			// Otherwise, calculate the robot speed
			// else if ((m_elapsedTime - m_robotVector[i]->m_last_position_time_update) >= m_robotVector[i]->m_pos_update_time && new_speed != 0) {
			// 	// Calculate distance to goal since last time
			// 	b2Vec2 prev_vector = m_robotVector[i]->m_last_position - measured_goal_pos;
			// 	float prev_distance = pow( pow(prev_vector.x, 2.0) + pow(prev_vector.y, 2.0), 0.5);
			// 	b2Vec2 curr_vector = m_robotVector[i]->getPosition() - measured_goal_pos;
			// 	float curr_distance = pow( pow(curr_vector.x, 2.0) + pow(curr_vector.y, 2.0), 0.5);
			// 	// Slowdown if the robot is moving away from the goal. Otherwise speed stays the same
			// 	if (curr_distance > prev_distance) {
			// 		if (new_speed > 0) {
			// 			new_speed = new_speed - m_controllerParam.param1 * (curr_distance - prev_distance);
			// 		}
			// 		else if (new_speed < 0) {
			// 			new_speed = new_speed + m_controllerParam.param1 * (curr_distance - prev_distance);
			// 		}
			// 		// if (new_speed < 0.0) {
			// 		// 	new_speed = 0.0;
			// 		// }
			// 	}
			// 	// Update last position
			// 	m_robotVector[i]->m_last_position = m_robotVector[i]->getPosition();
			// 	m_robotVector[i]->m_last_position_time_update = m_elapsedTime;
			// 	if (m_robotVector[i]->getId() == 1) {
			// 		// std::cout << "new_speed: " << new_speed << std::endl;
			// 	}
			// }
	// 	}
	// }


bool RobotController::checkTowering(){
	for (int i=0; i<m_robotVector.size(); i++){
		if (m_robotVector[i]->getPosition().y < 0.0){
			return true;
		}
	}
	return false;
}

void RobotController::step(int left_x, int right_x, int top_y, int bottom_y){

	// TODO: Add in built-in counting for robots that reached the goal

	for (int i=0; i<m_robotVector.size(); i++){

		if(m_robotVector[i]->getId()==-1){
			printf("continue for loop \n");
			continue;
		}

		// Update the map for robots that reached the goal if necessary
		if (m_robotVector[i]->getPosition().x > m_posGoal.x && m_robotVector[i]->getPosition().y < m_posGoal.y) {
			// Robot reached the goal. Set its id to true in the map
			m_robots_reached_goal[m_robotVector[i]->getId()] = true;
		}

	    createGripRobots(*m_robotVector[i]);
		wait_delay(*m_robotVector[i]);
		checkGrabbed(*m_robotVector[i]); //TODO check if more optimized if check only for robots in bridge state


	    if(m_robotVector[i]->isReady()){
	    //if (true) {
	    	if(m_robotVector[i]->getState()==BRIDGE){
	    		m_nbRobInBridge--;
	    		if(m_nbRobInBridge==0){
	    			m_dissolutionTime = m_currentIt*FPS;
	    		}
	    	}

		    m_robotVector[i]->m_ready=false;
//		    m_robotVector[i]->m_pushing_delay = - int(m_controllerParam.time_before_pushing*FPS+(m_currentIt-m_robotVector[i]->m_age)/600);

			m_robotVector[i]->setState(WALK);
			m_robotVector[i]->m_bridgeAge = 0;

			m_robotVector[i]->m_moving=true;
			if (m_robotVector[i]->checkGripp(m_robotVector[i]->m_movingSide)){
				invertMovingWheel(*m_robotVector[i]);
			}
			// else{
				// printf("\n wrong moving wheel \n");}
			destroyJoints(*m_robotVector[i], m_robotVector[i]->m_movingSide);
			m_robotVector[i]->moveBodyWithMotor();
//	    	}
	    }

	    if(m_robotVector[i]->isMoving()){
	    	//Update the robot speed
		m_robotVector[i]->moveBodyWithMotor();
		// Figure out if the robot is pushing
		if(m_robotVector[i]->getDelay() == int(- m_controllerParam.time_before_pushing*FPS)){ //m_robotVector[i]->m_pushing_delay){ //
////			    m_robot[i].m_ready=false;
//	    		invertMovingWheel(m_robotVector.at(i));
	    		// printf("\n Robot moving for too long, it is pushing \n");
	    		// if(robotPushing(*m_robotVector[i])){
	    		// 	setRobotState(*m_robotVector[i],WALK);
	    		// }
	    	}
	    }
//	    m_robot[i].drawJoint(window);
		// Remove any robot that has gone out of bounds of the simulator
		robotOut(left_x, right_x, top_y, bottom_y, i);
//	    printf(joint);
	}
	m_currentIt ++;

}

bool RobotController::robotPushing(Robot& r){
	std::cout << "robotPushing()" << std::endl;

	b2ContactEdge* contactList = r.getWheel(r.m_movingSide)->GetContactList();
	b2Contact* contact = contactList->contact;
	for (int i=0; i<10; i++){
		if(contact->IsTouching() &&!(contactList->other == r.getBody()) && (r.contactOnGripSide(contact))){
			double angle = abs(r.getBody()->GetAngle() - r.m_referenceAngle); ///////////////Added abs !!
			if(angle > m_angle_min ){

				if (! (contactList->other==m_terrainBody)){
					Robot* r_other = static_cast<Robot*>( contactList->other->GetUserData() );
					if(r_other->getId()>0){
						setRobotState(*r_other, BRIDGE);
						r.setContact(true);
						r.gripSide(contact, contactList->other, m_M_TO_PX);
						r.setDelay(m_controllerParam.walk_delay);
						return true;
					}
				}
				else{
					r.setContact(true);
					r.gripSide(contact, contactList->other, m_M_TO_PX);
					r.setDelay(m_controllerParam.walk_delay);
					return true;
				}
			}
		}
		if(contactList->next == nullptr){
			r.setDelay(0);
			return false;
		}
		contactList = contactList->next;
		contact = contactList->contact;
	}
	r.setDelay(0);
	return false;
}

bool RobotController::robotStacking(Robot* r, float posX){
	float x=r->getBody()->GetPosition().x;
	if(x < (m_robotParam.body_length)){
	// if(x<0.0){
		return true;
	}
	return false;
}
void RobotController::setBridgeStability(bool stable){
	m_isStable = stable;
}
int RobotController::getNbRobots(e_state s){
	int n = 0;
	for (int i=0; i<m_robotVector.size(); i++){
		if(m_robotVector[i]->getState() == s){
			n++;
		}
	}
	return n;
}

int RobotController::getNbActiveRobots(){
	return m_robotVector.size();
}

int RobotController::getNbRobotsBlocked(){
	float h = INT_MAX;
	int id = 0;
	int nb_blocked = 0;
	for (int i=0; i<m_robotVector.size(); i++){
		if(m_robotVector[i]->getState() == BRIDGE){
			if(m_robotVector[i]->getPosition().y < h){
				h=m_robotVector[i]->getPosition().y;
				id=m_robotVector[i]->getId();
			}
		}
	}
	for (int i=0; i<m_robotVector.size(); i++){
		if(m_robotVector[i]->getState() == WALK){
			if(m_robotVector[i]->getPosition().y > h && m_robotVector[i]->getId()<id){
				nb_blocked ++;
			}
		}
	}
	return nb_blocked;
}

int RobotController::getNbRobotsReachedGoal() {
	return m_robots_reached_goal.size();
}

b2Vec2 RobotController::getAvgPos(){
	b2Vec2 avg_pos = b2Vec2(0.0,0.0);
	// sum all of the positions
	for (int i=0; i<m_robotVector.size(); i++){
		avg_pos = avg_pos + m_robotVector[i]->getPosition();
	}
	// divide to get the average
	avg_pos.x = avg_pos.x/m_robotVector.size();
	avg_pos.y = avg_pos.y/m_robotVector.size();
	return avg_pos;
}
