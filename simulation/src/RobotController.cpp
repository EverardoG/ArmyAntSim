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
		printf("Robot created \n");
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
		robotA = true;
		rA=static_cast<Robot*>(bodyContactA->GetUserData());
	}

	if(!(bodyContactB == m_terrainBody)){
		robotB = true;
		rB=static_cast<Robot*>(bodyContactB->GetUserData());
	}

	if(robotA){
		contactorA = (rA->isMoving()||rA->m_start)
				   && (rA->contactOnGripSide(contact));//||m_robotVector[A].m_start);
	}

	if(robotB){
		contactorB = (rB->isMoving()||rB->m_start)
				   && (rB->contactOnGripSide(contact));//||m_robotVector[A].m_start);
	}

	if (robotA && robotB && (rA==rB)){
		// printf("contact within robot\n");
		return;
	}

	// start Finite-state machine
	if (contactorA && contactorB){

		double angleA = rA->getBody()->GetAngle() - rA->m_referenceAngle;

		double angleB = rB->getBody()->GetAngle() - rB->m_referenceAngle;

		if(abs(angleA) > m_controllerParam.angle_limit && angleB > m_controllerParam.angle_limit){
			rA->gripSide(contact, bodyContactB, m_M_TO_PX);
			rA->setContact(true);
			rB->gripSide(contact, bodyContactA, m_M_TO_PX);
			rB->setContact(true);
			setRobotState(*rA, BRIDGE);
			setRobotState(*rB, BRIDGE); //TODO becareful changed rule
		}
		else if(abs(angleA) > m_controllerParam.angle_limit){
			rA->gripSide(contact, bodyContactB, m_M_TO_PX);
			rA->setContact(true);
			setRobotState(*rA, BRIDGE); //WALK
			setRobotState(*rB, BRIDGE); //TODO becareful changed rule
		}

		else if(abs(angleB) > m_controllerParam.angle_limit){
			rB->gripSide(contact, bodyContactA, m_M_TO_PX);
			rB->setContact(true);
			setRobotState(*rA, BRIDGE);
			setRobotState(*rB, BRIDGE); //WALK
		}

	}

	else if (contactorB){

		double angleB = rB->getBody()->GetAngle() - rB->m_referenceAngle;
		if(abs(angleB) > m_controllerParam.angle_limit){
	//		if (m_robotVector[B].m_start){m_robotVector[B].m_start = false;}
			rB->gripSide(contact, bodyContactA, m_M_TO_PX);
			setRobotState(*rB, WALK);

			if (robotA){
				setRobotState(*rA, BRIDGE);
			}
		}
	}

	else if (contactorA){

		double angleA = rA->getBody()->GetAngle() - rA->m_referenceAngle;
//		std::cout<<"angle robot: "<<moduloAngle(m_robotVector[A].getBody()->GetAngle())*RAD_TO_DEG<<std::endl;
//		std::cout<<"angle A: "<<angleA*RAD_TO_DEG<<std::endl;

		if(abs(angleA) > m_controllerParam.angle_limit|| angleA==0){
	//		if (m_robotVector[A].m_start){m_robotVector[A].m_start = false;}
			rA->gripSide(contact, bodyContactB, m_M_TO_PX);
			setRobotState(*rA, WALK);

			if (robotB){
				setRobotState(*rB, BRIDGE);
			}
		}
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
	if (robot.getDelay()==0 && (!robot.isMoving())){
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
		printf("remove 0 \n");

		for (int i=0; i<m_robotToDestroy.size(); i++){
			printf("m_robotToDestroy.size(): %ld\n", m_robotToDestroy.size());
			printf("Grabbing id for i: %d\n", i);
			int id = m_robotToDestroy[i];
			printf("id is %d\n", id);
			std::vector<Robot*>::iterator pRob =  m_robotVector.begin()+id;

			printf("Setup pRob\n");
			// This if statement is a quick-fix. Not sure why pRob is ever equal to
			// m_robotVector.end() in the first place. It's a rare case that happens
			// right at the end of the simulator and causes a segfault without this
			// check
			if (pRob != m_robotVector.end()) delete *pRob;
			printf("Deleted pRob\n");
			if(!(id == m_robotVector.size()-1)){
				printf("remove 0b \n");
				std::swap(m_robotVector[id], m_robotVector.back()); // check if same ?
				printf("remove 0c \n");
			}

			printf("remove 1 \n");
			m_robotVector.pop_back();
			printf("remove 2 \n");

		}
		printf("About to clear m_robotToDestroy\n");
		m_robotToDestroy.clear();
		printf("Successfully destroyed robot\n");
	}
}

void RobotController::robotOut(double end_x, int id){
	int pos = (m_robotVector[id]->getBody()->GetWorldCenter()).x * m_M_TO_PX;
	// std::cout << "pos: " << pos << " | end_x: " << end_x << std::endl;
	if(pos > end_x){
		std::cout << "Planning to delete Robot " << id << std::endl;
		m_robotToDestroy.push_back(id);
//		removeRobot(id);
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
		m_robotVector[i]->setSpeed(desired_speed);
	}
}

// float RobotController::calculateSpeed( robot, goal_pos )
// or goal_pos could be an attribute of RobotController
// and it's public and it's defined in config and set by Demo.cpp
// also option for dynamic speed vs fixed speed in config
// maybe change delay instead of speed (this is more portable for real robot)

void RobotController::calculateSpeedsToGoal(b2Vec2 m_goal_pos){
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
}

bool RobotController::checkTowering(){
	for (int i=0; i<m_robotVector.size(); i++){
		if (m_robotVector[i]->getPosition().y < 0.0){
			return true;
		}
	}
	return false;
}

void RobotController::step(double end_x){

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
	    		if(robotPushing(*m_robotVector[i])){
	    			setRobotState(*m_robotVector[i],WALK);
	    		}
	    	}
	    }
//	    m_robot[i].drawJoint(window);
		robotOut(end_x, i);
//	    printf(joint);
	}
	m_currentIt ++;

}

bool RobotController::robotPushing(Robot& r){

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
	// if(x < (m_robotParam.body_length + posX)){
	if(x<0.0){
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
