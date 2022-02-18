/*
 * robot.cpp
 *
 *  Created on: 26 sept. 2018
 *      Author: lucie
 *
 *      29 Jan 2019:
 *      	- Add a userData on the robot object in the box2D bodies
 */

#include "Robot.h"

#include "Config.h"
#include "helpers.h"
#include <iostream>

Robot::Robot() {
	// TODO Auto-generated constructor stub
	//m_robotBody = createBody(world, posX, posY);

}
Robot::Robot(b2World* world, config::sRobot robotParameters, double posX, double posY, double angle) {
	// TODO Auto-generated constructor stub
	//m_robotBody = createBody(world, posX, posY);
	m_state = WALK;
	createBody(world, robotParameters, posX, posY, angle);
//	m_M_TO_PX = config::WINDOW_X_PX / ((2*8+3));

}
Robot::~Robot() {
	// printf(" destructor called \n");
	// TODO Auto-generated destructor stub

	// std::cout << "destruction of robot id: " << m_id << std::endl;
	m_id=-1;

//	if(m_robotBody){
		b2World* world = (m_robotBody->GetWorld());
		m_robotBody->GetWorld()->DestroyBody(m_leftWheel);
		m_robotBody->GetWorld()->DestroyBody(m_rightWheel);
		m_robotBody->GetWorld()->DestroyBody(m_robotBody);
//	}

	m_robotBody = nullptr;
	m_leftWheel = nullptr;
	m_rightWheel = nullptr;

	m_rightWheelMotor = nullptr;
	m_leftWheelMotor = nullptr;

	m_previousGripJoint = nullptr;
	m_currentGripJoint = nullptr;
}

void Robot::createBody(b2World* world, config::sRobot robotParameters, double posX, double posY, double angle){

	m_robotParameters = robotParameters;
	m_robotParameters.wheel_radius = (m_robotParameters.body_length - 0.02)/4;
	m_robotParameters.wheel_distance = (m_robotParameters.body_length - 2*m_robotParameters.wheel_radius);
	m_robotParameters.attach_height = m_robotParameters.wheel_radius/2;

	/* Attach between the wheel -------------------------- */
	b2BodyDef BodyDef;
	BodyDef.position = b2Vec2(posX, posY);
	BodyDef.angle = angle;
	BodyDef.type = b2_dynamicBody;
	BodyDef.bullet = true;
	m_robotBody = world->CreateBody(&BodyDef);
	m_robotBody->SetUserData( this );

	b2PolygonShape boxShape;
	boxShape.SetAsBox(m_robotParameters.wheel_distance/2,m_robotParameters.attach_height/2);
	b2FixtureDef boxFixtureDef;
	boxFixtureDef.shape = &boxShape;
	boxFixtureDef.density = 2.f;
	m_robotBody->CreateFixture(&boxFixtureDef);

	/* Box2D left wheel  ---------------------------------*/
	BodyDef.position = m_robotBody->GetWorldPoint(b2Vec2(-m_robotParameters.wheel_distance/2, 0));
	BodyDef.fixedRotation = true;
//	BodyDef.angularDamping = 1.0;
	m_leftWheel = world->CreateBody(&BodyDef);
	m_leftWheel->SetUserData( this );

		// Fixture
	b2CircleShape mcircleShape;
	mcircleShape.m_p.Set(0, 0); //position, relative to body position
	mcircleShape.m_radius = m_robotParameters.wheel_radius; //radius
	b2FixtureDef leftFixtureDef;
	leftFixtureDef.density = 1.f;
	leftFixtureDef.shape = &mcircleShape;
	m_leftWheel->CreateFixture(&leftFixtureDef);

		// Joint
	b2RevoluteJointDef revoluteJointDef;
	revoluteJointDef.bodyA = m_leftWheel;
	revoluteJointDef.bodyB = m_robotBody;
	revoluteJointDef.localAnchorB.Set(-m_robotParameters.wheel_distance/2,0);
	revoluteJointDef.localAnchorA.Set(0,0);
//	revoluteJointDef.referenceAngle = 0;
	revoluteJointDef.collideConnected = false;
	m_leftWheelMotor = static_cast<b2RevoluteJoint*>(world->CreateJoint( &revoluteJointDef ));
	m_leftWheelMotor->SetMaxMotorTorque(300.0f);
	m_leftWheelMotor->EnableMotor(true);
	m_leftWheelMotor->SetMotorSpeed(0.0);

	double a = m_leftWheelMotor->GetJointAngle();
	m_leftWheelMotor->EnableLimit(true);
	m_leftWheelMotor->SetLimits( a-0.01, a+0.01 );

	/* Box2D right wheel -----------------------------------*/
	BodyDef.position = m_robotBody->GetWorldPoint(b2Vec2(m_robotParameters.wheel_distance/2, 0));//b2Vec2(posX+m_robotParameters.wheel_distance/2, posY);
	m_rightWheel = world->CreateBody(&BodyDef);
	m_rightWheel->SetUserData( this );

		// Fixture
	b2FixtureDef rightFixtureDef;
	mcircleShape.m_p.Set(0, 0); //left wheel
	rightFixtureDef.shape = &mcircleShape;
	rightFixtureDef.density = 1.f;
	m_rightWheel->CreateFixture(&rightFixtureDef);

		// Joint
	b2RevoluteJointDef revoluteJointDef2;
	revoluteJointDef2.bodyA = m_rightWheel;
	revoluteJointDef2.bodyB = m_robotBody;
	revoluteJointDef2.localAnchorB.Set(m_robotParameters.wheel_distance/2,0);
	revoluteJointDef2.localAnchorA.Set(0,0);
//	revoluteJointDef2.referenceAngle = 0;
	revoluteJointDef2.collideConnected = false;
	m_rightWheelMotor = static_cast<b2RevoluteJoint*>(world->CreateJoint( &revoluteJointDef2 ));
	m_rightWheelMotor->SetMaxMotorTorque(300.0f);
	m_rightWheelMotor->EnableMotor(true);
	m_rightWheelMotor->SetMotorSpeed(0.0);

	a = m_rightWheelMotor->GetJointAngle();
	m_rightWheelMotor->EnableLimit(true);
	m_rightWheelMotor->SetLimits( a-0.01, a+0.01 );

}

void Robot::drawBody(sf::RenderWindow& window, double m_to_pix){

	/* Left wheel */
	sf::CircleShape left_wheelShape(m_to_pix* m_robotParameters.wheel_radius);
	if (m_state == BRIDGE){
//	if (m_isGrabbed){
//	if (m_moving){
		if(m_movingSide == LEFT){
			left_wheelShape.setFillColor(sf::Color(237,201,217));
		}
		else{
			left_wheelShape.setFillColor(sf::Color(172,197,217));
		}
	}
	else{
		if(m_movingSide == LEFT){
			left_wheelShape.setFillColor(sf::Color(246,125,150));
		}
		else{
			left_wheelShape.setFillColor(sf::Color(58,105,186));
		}
	}
	double bodyAngle = m_leftWheel->GetAngle();
	double topLeftX = m_leftWheel->GetWorldPoint(b2Vec2(-m_robotParameters.wheel_radius,-m_robotParameters.wheel_radius)).x;
	double topLeftY = m_leftWheel->GetWorldPoint(b2Vec2(-m_robotParameters.wheel_radius,-m_robotParameters.wheel_radius)).y;

	left_wheelShape.setPosition(m_to_pix*topLeftX,m_to_pix*topLeftY); //body.getPosition
	left_wheelShape.setRotation(bodyAngle*RAD_TO_DEG);

	sf::Vertex angle_left[2];
	b2Vec2 start = m_leftWheel->GetWorldCenter();
	b2Vec2 end = m_leftWheel->GetWorldPoint(b2Vec2(0,m_robotParameters.wheel_radius));
	angle_left[0]= sf::Vertex(sf::Vector2f(start.x*m_to_pix, start.y*m_to_pix), sf::Color::Black);
	angle_left[1]=	sf::Vertex(sf::Vector2f(end.x*m_to_pix, end.y*m_to_pix), sf::Color::Black);

	/* Right wheel */
	sf::CircleShape right_wheelShape(m_to_pix* m_robotParameters.wheel_radius);
//	if (m_isGrabbed){
	if (m_state == BRIDGE){
//	if (m_moving){
		if(m_movingSide == RIGHT){
			right_wheelShape.setFillColor(sf::Color(237,201,217));
		}
		else{
			right_wheelShape.setFillColor(sf::Color(172,197,217));
		}

	}
	else{
		if(m_movingSide == RIGHT){
			right_wheelShape.setFillColor(sf::Color(246,125,150));
		}
		else{
			right_wheelShape.setFillColor(sf::Color(58,105,186));
		}
	}
	bodyAngle = m_rightWheel->GetAngle();
	topLeftX = m_rightWheel->GetWorldPoint(b2Vec2(-m_robotParameters.wheel_radius,-m_robotParameters.wheel_radius)).x;
	topLeftY = m_rightWheel->GetWorldPoint(b2Vec2(-m_robotParameters.wheel_radius,-m_robotParameters.wheel_radius)).y;

	right_wheelShape.setPosition(m_to_pix*topLeftX,m_to_pix*topLeftY); //body.getPosition
	right_wheelShape.setRotation(bodyAngle*RAD_TO_DEG);

	sf::Vertex angle_right[2];
	start = m_rightWheel->GetWorldCenter();
	end = m_rightWheel->GetWorldPoint(b2Vec2(0, m_robotParameters.wheel_radius));
	angle_right[0] = sf::Vertex(sf::Vector2f(start.x*m_to_pix, start.y*m_to_pix), sf::Color::Black);
	angle_right[1] = sf::Vertex(sf::Vector2f(end.x*m_to_pix, end.y*m_to_pix), sf::Color::Black);

	/* wheel Attach */
	sf::RectangleShape attachShape(sf::Vector2f(m_robotParameters.wheel_distance*m_to_pix, m_robotParameters.attach_height*m_to_pix));
	attachShape.setFillColor(sf::Color::Black);
	bodyAngle = m_robotBody->GetAngle();
	topLeftX = m_robotBody->GetWorldPoint(b2Vec2(-m_robotParameters.wheel_distance/2,-m_robotParameters.attach_height/2)).x;
	topLeftY = m_robotBody->GetWorldPoint(b2Vec2(-m_robotParameters.wheel_distance/2,-m_robotParameters.attach_height/2)).y;

	attachShape.setPosition(m_to_pix*topLeftX,m_to_pix*topLeftY);
	attachShape.setRotation(bodyAngle*RAD_TO_DEG);

	window.draw(attachShape);

	window.draw(left_wheelShape);
	window.draw(right_wheelShape);

//	window.draw(angle_right, 2, sf::Lines);
//	window.draw(angle_left, 2, sf::Lines);

//	window.draw(m_shapeLeft);
//	window.draw(m_shapeRight);

}
void Robot::drawBody(sf::RenderTexture& texture, double m_to_pix){

	/* Left wheel */
	sf::CircleShape left_wheelShape(m_to_pix* m_robotParameters.wheel_radius);
	if (m_state == BRIDGE){
//	if (m_isGrabbed){
//	if (m_moving){
		if(m_movingSide == LEFT){
			left_wheelShape.setFillColor(sf::Color(237,201,217));
		}
		else{
			left_wheelShape.setFillColor(sf::Color(172,197,217));
		}
	}
	else{
		if(m_movingSide == LEFT){
			left_wheelShape.setFillColor(sf::Color(246,125,150));
		}
		else{
			left_wheelShape.setFillColor(sf::Color(58,105,186));
		}
	}
	double bodyAngle = m_leftWheel->GetAngle();
	double topLeftX = m_leftWheel->GetWorldPoint(b2Vec2(-m_robotParameters.wheel_radius,-m_robotParameters.wheel_radius)).x;
	double topLeftY = m_leftWheel->GetWorldPoint(b2Vec2(-m_robotParameters.wheel_radius,-m_robotParameters.wheel_radius)).y;

	left_wheelShape.setPosition(m_to_pix*topLeftX,m_to_pix*topLeftY); //body.getPosition
	left_wheelShape.setRotation(bodyAngle*RAD_TO_DEG);

	sf::Vertex angle_left[2];
	b2Vec2 start = m_leftWheel->GetWorldCenter();
	b2Vec2 end = m_leftWheel->GetWorldPoint(b2Vec2(0,m_robotParameters.wheel_radius));
	angle_left[0]= sf::Vertex(sf::Vector2f(start.x*m_to_pix, start.y*m_to_pix), sf::Color::Black);
	angle_left[1]=	sf::Vertex(sf::Vector2f(end.x*m_to_pix, end.y*m_to_pix), sf::Color::Black);

	/* Right wheel */
	sf::CircleShape right_wheelShape(m_to_pix* m_robotParameters.wheel_radius);
//	if (m_isGrabbed){
	if (m_state == BRIDGE){
//	if (m_moving){
		if(m_movingSide == RIGHT){
			right_wheelShape.setFillColor(sf::Color(237,201,217));
		}
		else{
			right_wheelShape.setFillColor(sf::Color(172,197,217));
		}

	}
	else{
		if(m_movingSide == RIGHT){
			right_wheelShape.setFillColor(sf::Color(246,125,150));
		}
		else{
			right_wheelShape.setFillColor(sf::Color(58,105,186));
		}
	}
	bodyAngle = m_rightWheel->GetAngle();
	topLeftX = m_rightWheel->GetWorldPoint(b2Vec2(-m_robotParameters.wheel_radius,-m_robotParameters.wheel_radius)).x;
	topLeftY = m_rightWheel->GetWorldPoint(b2Vec2(-m_robotParameters.wheel_radius,-m_robotParameters.wheel_radius)).y;

	right_wheelShape.setPosition(m_to_pix*topLeftX,m_to_pix*topLeftY); //body.getPosition
	right_wheelShape.setRotation(bodyAngle*RAD_TO_DEG);

	sf::Vertex angle_right[2];
	start = m_rightWheel->GetWorldCenter();
	end = m_rightWheel->GetWorldPoint(b2Vec2(0, m_robotParameters.wheel_radius));
	angle_right[0] = sf::Vertex(sf::Vector2f(start.x*m_to_pix, start.y*m_to_pix), sf::Color::Black);
	angle_right[1] = sf::Vertex(sf::Vector2f(end.x*m_to_pix, end.y*m_to_pix), sf::Color::Black);

	/* wheel Attach */
	sf::RectangleShape attachShape(sf::Vector2f(m_robotParameters.wheel_distance*m_to_pix, m_robotParameters.attach_height*m_to_pix));
	attachShape.setFillColor(sf::Color::Black);
	bodyAngle = m_robotBody->GetAngle();
	topLeftX = m_robotBody->GetWorldPoint(b2Vec2(-m_robotParameters.wheel_distance/2,-m_robotParameters.attach_height/2)).x;
	topLeftY = m_robotBody->GetWorldPoint(b2Vec2(-m_robotParameters.wheel_distance/2,-m_robotParameters.attach_height/2)).y;

	attachShape.setPosition(m_to_pix*topLeftX,m_to_pix*topLeftY);
	attachShape.setRotation(bodyAngle*RAD_TO_DEG);

	texture.draw(attachShape);

	texture.draw(left_wheelShape);
	texture.draw(right_wheelShape);

//	window.draw(angle_right, 2, sf::Lines);
//	window.draw(angle_left, 2, sf::Lines);

//	window.draw(m_shapeLeft);
//	window.draw(m_shapeRight);

}
void Robot::drawGripJoint(sf::RenderWindow& window, double m_to_px){


	if(m_previousGripJoint){

		b2Vec2 pA = m_previousGripJoint->GetAnchorA();
		float delta = 1/2;


		sf::Vertex edge[2];
		edge[0] = sf::Vertex(sf::Vector2f(pA.x*m_to_px, m_to_px*pA.y), sf::Color::Black);

		if((m_previousGripJoint->GetBodyA() == m_leftWheel) || (m_previousGripJoint->GetBodyB() == m_leftWheel)){
			float xB = delta*m_leftWheel->GetLocalPoint(pA).x;
			float yB = delta*m_leftWheel->GetLocalPoint(pA).y;
			b2Vec2 pB = m_leftWheel->GetWorldPoint(b2Vec2(xB, yB));
			edge[1] = sf::Vertex(sf::Vector2f(m_to_px*pB.x, m_to_px*pB.y), sf::Color::Black);
		}

		else if(m_previousGripJoint->GetBodyA() == m_rightWheel || (m_previousGripJoint->GetBodyB() == m_rightWheel)){
			float xB = delta*m_rightWheel->GetLocalPoint(pA).x;
			float yB = delta*m_rightWheel->GetLocalPoint(pA).y;
			b2Vec2 pB = m_rightWheel->GetWorldPoint(b2Vec2(xB, yB));
			edge[1] = sf::Vertex(sf::Vector2f(m_to_px*pB.x, m_to_px*pB.y), sf::Color::Black);
		}

		sf::CircleShape p1;
		p1.setRadius(6);
		p1.setFillColor(sf::Color::Black);
		p1.setPosition(m_to_px*pA.x-3,pA.y*m_to_px-3);

		window.draw(edge, 2, sf::Lines);
		window.draw(p1);
	}


	 /* Same with current joint -------------------*/

	if(m_currentGripJoint){
		b2Vec2 pA = m_currentGripJoint->GetAnchorA();
		float delta = 1/2;

		sf::Vertex edge[2];
		edge[0] = sf::Vertex(sf::Vector2f(pA.x*m_to_px, m_to_px*pA.y), sf::Color::Black);

		if((m_currentGripJoint->GetBodyA() == m_leftWheel) || (m_currentGripJoint->GetBodyB() == m_leftWheel)){
			float xB = delta*m_leftWheel->GetLocalPoint(pA).x;
			float yB = delta*m_leftWheel->GetLocalPoint(pA).y;
			b2Vec2 pB = m_leftWheel->GetWorldPoint(b2Vec2(xB, yB));
			edge[1] = sf::Vertex(sf::Vector2f(m_to_px*pB.x, m_to_px*pB.y), sf::Color::Black);
		}

		else if(m_currentGripJoint->GetBodyA() == m_rightWheel || (m_currentGripJoint->GetBodyB() == m_rightWheel)){
			float xB = delta*m_rightWheel->GetLocalPoint(pA).x;
			float yB = delta*m_rightWheel->GetLocalPoint(pA).y;
			b2Vec2 pB = m_rightWheel->GetWorldPoint(b2Vec2(xB, yB));
			edge[1] = sf::Vertex(sf::Vector2f(m_to_px*pB.x, m_to_px*pB.y), sf::Color::Black);
		}

		sf::CircleShape p1;
		p1.setRadius(6);
		p1.setFillColor(sf::Color::Black);
		p1.setPosition(m_to_px*pA.x-3,pA.y*m_to_px-3);

		window.draw(edge, 2, sf::Lines);
		window.draw(p1);
	}


}
void Robot::drawGripJoint(sf::RenderTexture& texture, double m_to_px){


	if(m_previousGripJoint){

		b2Vec2 pA = m_previousGripJoint->GetAnchorA();
		float delta = 1/2;


		sf::Vertex edge[2];
		edge[0] = sf::Vertex(sf::Vector2f(pA.x*m_to_px, m_to_px*pA.y), sf::Color::Black);

		if((m_previousGripJoint->GetBodyA() == m_leftWheel) || (m_previousGripJoint->GetBodyB() == m_leftWheel)){
			float xB = delta*m_leftWheel->GetLocalPoint(pA).x;
			float yB = delta*m_leftWheel->GetLocalPoint(pA).y;
			b2Vec2 pB = m_leftWheel->GetWorldPoint(b2Vec2(xB, yB));
			edge[1] = sf::Vertex(sf::Vector2f(m_to_px*pB.x, m_to_px*pB.y), sf::Color::Black);
		}

		else if(m_previousGripJoint->GetBodyA() == m_rightWheel || (m_previousGripJoint->GetBodyB() == m_rightWheel)){
			float xB = delta*m_rightWheel->GetLocalPoint(pA).x;
			float yB = delta*m_rightWheel->GetLocalPoint(pA).y;
			b2Vec2 pB = m_rightWheel->GetWorldPoint(b2Vec2(xB, yB));
			edge[1] = sf::Vertex(sf::Vector2f(m_to_px*pB.x, m_to_px*pB.y), sf::Color::Black);
		}

		sf::CircleShape p1;
		p1.setRadius(6);
		p1.setFillColor(sf::Color::Black);
		p1.setPosition(m_to_px*pA.x-3,pA.y*m_to_px-3);

		texture.draw(edge, 2, sf::Lines);
		texture.draw(p1);
	}


	 /* Same with current joint -------------------*/

	if(m_currentGripJoint){
		b2Vec2 pA = m_currentGripJoint->GetAnchorA();
		float delta = 1/2;

		sf::Vertex edge[2];
		edge[0] = sf::Vertex(sf::Vector2f(pA.x*m_to_px, m_to_px*pA.y), sf::Color::Black);

		if((m_currentGripJoint->GetBodyA() == m_leftWheel) || (m_currentGripJoint->GetBodyB() == m_leftWheel)){
			float xB = delta*m_leftWheel->GetLocalPoint(pA).x;
			float yB = delta*m_leftWheel->GetLocalPoint(pA).y;
			b2Vec2 pB = m_leftWheel->GetWorldPoint(b2Vec2(xB, yB));
			edge[1] = sf::Vertex(sf::Vector2f(m_to_px*pB.x, m_to_px*pB.y), sf::Color::Black);
		}

		else if(m_currentGripJoint->GetBodyA() == m_rightWheel || (m_currentGripJoint->GetBodyB() == m_rightWheel)){
			float xB = delta*m_rightWheel->GetLocalPoint(pA).x;
			float yB = delta*m_rightWheel->GetLocalPoint(pA).y;
			b2Vec2 pB = m_rightWheel->GetWorldPoint(b2Vec2(xB, yB));
			edge[1] = sf::Vertex(sf::Vector2f(m_to_px*pB.x, m_to_px*pB.y), sf::Color::Black);
		}

		sf::CircleShape p1;
		p1.setRadius(6);
		p1.setFillColor(sf::Color::Black);
		p1.setPosition(m_to_px*pA.x-3,pA.y*m_to_px-3);

		texture.draw(edge, 2, sf::Lines);
		texture.draw(p1);
	}


}

void Robot::drawJoint(sf::RenderWindow& window, double m_to_px){
	 b2JointEdge* jointEdge = m_leftWheel->GetJointList();

	 if(jointEdge == nullptr){
		 return;
	 }

	 b2Joint* joint = jointEdge->joint;
	 b2Vec2 pA = joint->GetAnchorA();
	 b2Vec2 pB = joint->GetAnchorB();

	 sf::Vertex edge[2];
	 edge[0] = sf::Vertex(sf::Vector2f(pA.x*m_to_px, m_to_px*pA.y), sf::Color::Black);
	 edge[1] = sf::Vertex(sf::Vector2f(m_to_px*pB.x, m_to_px*pB.y), sf::Color::Black);

	 sf::CircleShape p1;
	 sf::CircleShape p2;
	 p1.setRadius(6);
	 p1.setFillColor(sf::Color::Black);
	 p1.setPosition(m_to_px*pA.x-3,pA.y*m_to_px-3);
	 p2.setRadius(6);
	 p2.setFillColor(sf::Color::Black);
	 p2.setPosition(m_to_px*pB.x-3,pB.y*m_to_px-3);

	 window.draw(edge, 2, sf::Lines);
	 window.draw(p1);
	 window.draw(p2);

	 if(!(jointEdge->next == nullptr)){
		 joint = jointEdge->next->joint;
		 pA = joint->GetAnchorA();
		 pB = joint->GetAnchorB();

		 edge[0] = sf::Vertex(sf::Vector2f(pA.x*m_to_px, m_to_px*pA.y), sf::Color::Black);
		 edge[1] = sf::Vertex(sf::Vector2f(m_to_px*pB.x, m_to_px*pB.y), sf::Color::Black);

		 p1.setPosition(m_to_px*pA.x-3,pA.y*m_to_px-3);
		 p2.setPosition(m_to_px*pB.x-3,pB.y*m_to_px-3);

		 window.draw(edge, 2, sf::Lines);
		 window.draw(p1);
		 window.draw(p2);

		 if(!(jointEdge->next->next == nullptr)){
			 joint = jointEdge->next->next->joint;
			 pA = joint->GetAnchorA();
			 pB = joint->GetAnchorB();

			 edge[0] = sf::Vertex(sf::Vector2f(pA.x*m_to_px, m_to_px*pA.y), sf::Color::Black);
			 edge[1] = sf::Vertex(sf::Vector2f(m_to_px*pB.x, m_to_px*pB.y), sf::Color::Black);
			 p1.setPosition(m_to_px*pA.x-3,pA.y*m_to_px-3);
			 p2.setPosition(m_to_px*pB.x-3,pB.y*m_to_px-3);

			 window.draw(edge, 2, sf::Lines);
			 window.draw(p1);
			 window.draw(p2);
		 }
	 }

	 if(!(jointEdge->prev == nullptr)){
		 joint = jointEdge->prev->joint;
		 pA = joint->GetAnchorA();
		 pB = joint->GetAnchorB();

		 edge[0] = sf::Vertex(sf::Vector2f(pA.x*m_to_px, m_to_px*pA.y), sf::Color::Black);
		 edge[1] = sf::Vertex(sf::Vector2f(m_to_px*pB.x, m_to_px*pB.y), sf::Color::Black);
		 window.draw(edge, 2, sf::Lines);
		 p1.setPosition(m_to_px*pA.x-3,pA.y*m_to_px-3);
		 p2.setPosition(m_to_px*pB.x-3,pB.y*m_to_px-3);

		 window.draw(p1);
		 window.draw(p2);
	 }

	 /* Same with right side -------------------*/


	 jointEdge = m_rightWheel->GetJointList();
	 joint = jointEdge->joint;
	 pA = joint->GetAnchorA();
	 pB = joint->GetAnchorB();

	 edge[0] = sf::Vertex(sf::Vector2f(pA.x*m_to_px, m_to_px*pA.y), sf::Color::Black);
	 edge[1] = sf::Vertex(sf::Vector2f(m_to_px*pB.x, m_to_px*pB.y), sf::Color::Black);

	 p1.setPosition(m_to_px*pA.x-3,pA.y*m_to_px-3);
	 p2.setPosition(m_to_px*pB.x-3,pB.y*m_to_px-3);

	 window.draw(edge, 2, sf::Lines);
	 window.draw(p1);
	 window.draw(p2);

	 if(!(jointEdge->next == nullptr)){
	 		 joint = jointEdge->next->joint;
	 		 pA = joint->GetAnchorA();
	 		 pB = joint->GetAnchorB();

	 		 edge[0] = sf::Vertex(sf::Vector2f(pA.x*m_to_px, m_to_px*pA.y), sf::Color::Black);
	 		 edge[1] = sf::Vertex(sf::Vector2f(m_to_px*pB.x, m_to_px*pB.y), sf::Color::Black);

	 		 p1.setPosition(m_to_px*pA.x-3,pA.y*m_to_px-3);
	 		 p2.setPosition(m_to_px*pB.x-3,pB.y*m_to_px-3);

	 		 window.draw(edge, 2, sf::Lines);
	 		 window.draw(p1);
	 		 window.draw(p2);

	 		 if(!(jointEdge->next->next == nullptr)){
	 			 joint = jointEdge->next->next->joint;
	 			 pA = joint->GetAnchorA();
	 			 pB = joint->GetAnchorB();

	 			 edge[0] = sf::Vertex(sf::Vector2f(pA.x*m_to_px, m_to_px*pA.y), sf::Color::Black);
	 			 edge[1] = sf::Vertex(sf::Vector2f(m_to_px*pB.x, m_to_px*pB.y), sf::Color::Black);
	 			 p1.setPosition(m_to_px*pA.x-3,pA.y*m_to_px-3);
	 			 p2.setPosition(m_to_px*pB.x-3,pB.y*m_to_px-3);

	 			 window.draw(edge, 2, sf::Lines);
	 			 window.draw(p1);
	 			 window.draw(p2);
	 		 }
	 	 }

	 if(!(jointEdge->prev == nullptr)){
		 joint = jointEdge->prev->joint;
		 pA = joint->GetAnchorA();
		 pB = joint->GetAnchorB();

		 edge[0] = sf::Vertex(sf::Vector2f(pA.x*m_to_px, m_to_px*pA.y), sf::Color::Black);
		 edge[1] = sf::Vertex(sf::Vector2f(m_to_px*pB.x, m_to_px*pB.y), sf::Color::Black);
		 window.draw(edge, 2, sf::Lines);
		 p1.setPosition(m_to_px*pA.x-3,pA.y*m_to_px-3);
		 p2.setPosition(m_to_px*pB.x-3,pB.y*m_to_px-3);

		 window.draw(p1);
		 window.draw(p2);
	 }
}
void Robot::drawJoint(sf::RenderTexture& texture, double m_to_px){
	 b2JointEdge* jointEdge = m_leftWheel->GetJointList();

	 if(jointEdge == nullptr){
		 return;
	 }

	 b2Joint* joint = jointEdge->joint;
	 b2Vec2 pA = joint->GetAnchorA();
	 b2Vec2 pB = joint->GetAnchorB();

	 sf::Vertex edge[2];
	 edge[0] = sf::Vertex(sf::Vector2f(pA.x*m_to_px, m_to_px*pA.y), sf::Color::Black);
	 edge[1] = sf::Vertex(sf::Vector2f(m_to_px*pB.x, m_to_px*pB.y), sf::Color::Black);

	 sf::CircleShape p1;
	 sf::CircleShape p2;
	 p1.setRadius(6);
	 p1.setFillColor(sf::Color::Black);
	 p1.setPosition(m_to_px*pA.x-3,pA.y*m_to_px-3);
	 p2.setRadius(6);
	 p2.setFillColor(sf::Color::Black);
	 p2.setPosition(m_to_px*pB.x-3,pB.y*m_to_px-3);

	 texture.draw(edge, 2, sf::Lines);
	 texture.draw(p1);
	 texture.draw(p2);

	 if(!(jointEdge->next == nullptr)){
		 joint = jointEdge->next->joint;
		 pA = joint->GetAnchorA();
		 pB = joint->GetAnchorB();

		 edge[0] = sf::Vertex(sf::Vector2f(pA.x*m_to_px, m_to_px*pA.y), sf::Color::Black);
		 edge[1] = sf::Vertex(sf::Vector2f(m_to_px*pB.x, m_to_px*pB.y), sf::Color::Black);

		 p1.setPosition(m_to_px*pA.x-3,pA.y*m_to_px-3);
		 p2.setPosition(m_to_px*pB.x-3,pB.y*m_to_px-3);

		 texture.draw(edge, 2, sf::Lines);
		 texture.draw(p1);
		 texture.draw(p2);

		 if(!(jointEdge->next->next == nullptr)){
			 joint = jointEdge->next->next->joint;
			 pA = joint->GetAnchorA();
			 pB = joint->GetAnchorB();

			 edge[0] = sf::Vertex(sf::Vector2f(pA.x*m_to_px, m_to_px*pA.y), sf::Color::Black);
			 edge[1] = sf::Vertex(sf::Vector2f(m_to_px*pB.x, m_to_px*pB.y), sf::Color::Black);
			 p1.setPosition(m_to_px*pA.x-3,pA.y*m_to_px-3);
			 p2.setPosition(m_to_px*pB.x-3,pB.y*m_to_px-3);

			 texture.draw(edge, 2, sf::Lines);
			 texture.draw(p1);
			 texture.draw(p2);
		 }
	 }

	 if(!(jointEdge->prev == nullptr)){
		 joint = jointEdge->prev->joint;
		 pA = joint->GetAnchorA();
		 pB = joint->GetAnchorB();

		 edge[0] = sf::Vertex(sf::Vector2f(pA.x*m_to_px, m_to_px*pA.y), sf::Color::Black);
		 edge[1] = sf::Vertex(sf::Vector2f(m_to_px*pB.x, m_to_px*pB.y), sf::Color::Black);
		 texture.draw(edge, 2, sf::Lines);
		 p1.setPosition(m_to_px*pA.x-3,pA.y*m_to_px-3);
		 p2.setPosition(m_to_px*pB.x-3,pB.y*m_to_px-3);

		 texture.draw(p1);
		 texture.draw(p2);
	 }

	 /* Same with right side -------------------*/


	 jointEdge = m_rightWheel->GetJointList();
	 joint = jointEdge->joint;
	 pA = joint->GetAnchorA();
	 pB = joint->GetAnchorB();

	 edge[0] = sf::Vertex(sf::Vector2f(pA.x*m_to_px, m_to_px*pA.y), sf::Color::Black);
	 edge[1] = sf::Vertex(sf::Vector2f(m_to_px*pB.x, m_to_px*pB.y), sf::Color::Black);

	 p1.setPosition(m_to_px*pA.x-3,pA.y*m_to_px-3);
	 p2.setPosition(m_to_px*pB.x-3,pB.y*m_to_px-3);

	 texture.draw(edge, 2, sf::Lines);
	 texture.draw(p1);
	 texture.draw(p2);

	 if(!(jointEdge->next == nullptr)){
	 		 joint = jointEdge->next->joint;
	 		 pA = joint->GetAnchorA();
	 		 pB = joint->GetAnchorB();

	 		 edge[0] = sf::Vertex(sf::Vector2f(pA.x*m_to_px, m_to_px*pA.y), sf::Color::Black);
	 		 edge[1] = sf::Vertex(sf::Vector2f(m_to_px*pB.x, m_to_px*pB.y), sf::Color::Black);

	 		 p1.setPosition(m_to_px*pA.x-3,pA.y*m_to_px-3);
	 		 p2.setPosition(m_to_px*pB.x-3,pB.y*m_to_px-3);

	 		 texture.draw(edge, 2, sf::Lines);
	 		 texture.draw(p1);
	 		 texture.draw(p2);

	 		 if(!(jointEdge->next->next == nullptr)){
	 			 joint = jointEdge->next->next->joint;
	 			 pA = joint->GetAnchorA();
	 			 pB = joint->GetAnchorB();

	 			 edge[0] = sf::Vertex(sf::Vector2f(pA.x*m_to_px, m_to_px*pA.y), sf::Color::Black);
	 			 edge[1] = sf::Vertex(sf::Vector2f(m_to_px*pB.x, m_to_px*pB.y), sf::Color::Black);
	 			 p1.setPosition(m_to_px*pA.x-3,pA.y*m_to_px-3);
	 			 p2.setPosition(m_to_px*pB.x-3,pB.y*m_to_px-3);

	 			 texture.draw(edge, 2, sf::Lines);
	 			 texture.draw(p1);
	 			 texture.draw(p2);
	 		 }
	 	 }

	 if(!(jointEdge->prev == nullptr)){
		 joint = jointEdge->prev->joint;
		 pA = joint->GetAnchorA();
		 pB = joint->GetAnchorB();

		 edge[0] = sf::Vertex(sf::Vector2f(pA.x*m_to_px, m_to_px*pA.y), sf::Color::Black);
		 edge[1] = sf::Vertex(sf::Vector2f(m_to_px*pB.x, m_to_px*pB.y), sf::Color::Black);
		 texture.draw(edge, 2, sf::Lines);
		 p1.setPosition(m_to_px*pA.x-3,pA.y*m_to_px-3);
		 p2.setPosition(m_to_px*pB.x-3,pB.y*m_to_px-3);

		 texture.draw(p1);
		 texture.draw(p2);
	 }
}

void Robot::moveBodyWithMotor(){

	if (m_movingSide == RIGHT){
		m_rightWheelMotor->EnableMotor(false);
		allowMotorRotation(RIGHT);

		allowMotorRotation(LEFT);
		m_leftWheelMotor->EnableMotor(true);
		m_leftWheelMotor->SetMotorSpeed(m_angularSpeed);
		// TODO: Adjustable torque
		// m_leftWheelMotor->SetMaxMotorTorque(30.0f);
		m_leftWheelMotor->SetMaxMotorTorque(m_robotParameters.torque);
	}

	else{
		m_leftWheelMotor->EnableMotor(false);
		allowMotorRotation(LEFT);

		allowMotorRotation(RIGHT);
		m_rightWheelMotor->EnableMotor(true);
		m_rightWheelMotor->SetMotorSpeed(m_angularSpeed);
		// m_rightWheelMotor->SetMaxMotorTorque(30.0f);
		m_leftWheelMotor->SetMaxMotorTorque(m_robotParameters.torque);
	}

//	m_shape3.setRadius(10);
//	m_shape3.setFillColor(sf::Color(150, 10, 150));
//	m_shape3.setPosition(config::M_TO_PX*(m_robotBody->GetWorldCenter()).x-10,(m_robotBody->GetWorldCenter()).y*config::M_TO_PX-10);


}
void Robot::moveBodyWithForce(){
	b2Vec2 center(0,0);
	b2Vec2 strength(0,0);
	double torque =0 ;
	double v=6;

	allowMotorRotation(LEFT);
	allowMotorRotation(RIGHT);

	if (m_movingSide == RIGHT){
		center.Set(m_robotParameters.wheel_distance/2,0);
		strength.Set((m_robotBody->GetWorldVector(b2Vec2(0,v))).x, (m_robotBody->GetWorldVector(b2Vec2(0,v))).y);
		m_robotBody->ApplyForceToCenter( strength, true);
//		torque=m_leftWheelMotor->GetReactionTorque(60.f);
	}

	else if (m_movingSide == LEFT){

		center.Set(-m_robotParameters.wheel_distance/2,0);
		strength.Set((m_robotBody->GetWorldVector(b2Vec2(0,-v))).x, (m_robotBody->GetWorldVector(b2Vec2(0,-v))).y);
		m_robotBody->ApplyForceToCenter( strength, true);
//		torque=m_rightWheelMotor->GetReactionTorque(60.f);
	}

}
void Robot::moveBodyWithImpulse(){
	b2Vec2 center(0,0);
	b2Vec2 strength(0,0);
	double torque =0 ;
	double v=4;

	allowMotorRotation(LEFT);
	allowMotorRotation(RIGHT);
    double vel = m_robotBody->GetAngularVelocity();

    double velChange = m_angularSpeed - vel;
    double impulse = m_robotBody->GetInertia() * velChange *10; //disregard time factor
    /* to reach the target speed, should calculate the real inertia with the wheel attached to the body,
     *  but here act more like a Proportionnal command*/
//    printf("impulse: %f, \n", impulse);
	m_robotBody->ApplyAngularImpulse( impulse, true);
}

void Robot::turnOffMotor(side s){
	if (s == RIGHT){
		m_rightWheelMotor->EnableMotor(false);
	}

	else{
		m_leftWheelMotor->EnableMotor(false);
	}
}
void Robot::blockMotorRotation(side s){
	if (s == RIGHT){
		m_rightWheelMotor->EnableMotor(true);
		m_rightWheelMotor->SetMotorSpeed(0.0);
		m_rightWheelMotor->SetMaxMotorTorque(30.0f);

		double angle = m_rightWheelMotor->GetJointAngle();
		m_rightWheelMotor->EnableLimit(true);
		m_rightWheelMotor->SetLimits( angle-0.1, angle+0.1 );
	}

	else{
		m_leftWheelMotor->EnableMotor(true);
		m_leftWheelMotor->SetMotorSpeed(0.0);
		m_leftWheelMotor->SetMaxMotorTorque(30.0f);
//
		double angle = m_leftWheelMotor->GetJointAngle();
		m_leftWheelMotor->EnableLimit(true);
		m_leftWheelMotor->SetLimits( angle-0.01, angle+0.01 );
	}
}
void Robot::limitMotorRotation(side s, double limit_angle_rad){
	if (s == RIGHT){
		m_rightWheelMotor->EnableMotor(false);
//		m_rightWheelMotor->SetMotorSpeed(0.0);
		m_rightWheelMotor->SetMaxMotorTorque(500.0f);

		m_referenceAngleJoint = m_rightWheelMotor->GetJointAngle();
		m_rightWheelMotor->EnableLimit(true);
		m_rightWheelMotor->SetLimits( m_referenceAngleJoint-limit_angle_rad, m_referenceAngleJoint+limit_angle_rad );
	}

	else{
		m_leftWheelMotor->EnableMotor(false);
//		m_leftWheelMotor->SetMotorSpeed(0.0);
		m_leftWheelMotor->SetMaxMotorTorque(500.0f);

		m_referenceAngleJoint = m_leftWheelMotor->GetJointAngle();
		m_leftWheelMotor->EnableLimit(true);
		m_leftWheelMotor->SetLimits( m_referenceAngleJoint-limit_angle_rad, m_referenceAngleJoint+limit_angle_rad );
	}
}
void Robot::allowMotorRotation(side s){
	if (s == RIGHT){
		m_rightWheelMotor->EnableLimit(false);
	}

	else{
		m_leftWheelMotor->EnableLimit(false);
	}
}


void Robot::grip(b2Contact* contact, b2Body* otherBody, double m_to_px){ //ou alors fonction retourne un joint ?
	std::cout << "grip()" << std::endl;
	b2WorldManifold worldManifold;
	contact->GetWorldManifold(&worldManifold);
	b2Body* bodyA = contact->GetFixtureA()->GetBody();
	b2Body* bodyB = contact->GetFixtureB()->GetBody();
//	if(bodyA == bodyB){return;}

	double xB = otherBody->GetLocalPoint(worldManifold.points[0]).x;
	double yB = otherBody->GetLocalPoint(worldManifold.points[0]).y;
	//double yB = otherBody->GetLocalPoint(p).y;
	b2Vec2 wheel_center;

	if(bodyA==m_leftWheel || bodyB==m_leftWheel)
	{
//		printf("left wheel \n");
		// Create the joint
		if(!m_movingSide == LEFT){
			printf("error contact side\n");
			m_movingSide = LEFT;
		}
		wheel_center.Set(0, 0); //in local body coordinates
		double xA = m_leftWheel->GetLocalPoint(worldManifold.points[0]).x;
		double yA = m_leftWheel->GetLocalPoint(worldManifold.points[0]).y;
		m_leftGripJointDef.bodyA = m_leftWheel;
		m_leftGripJointDef.bodyB = otherBody;
		m_leftGripJointDef.collideConnected = true;
		m_leftGripJointDef.localAnchorA.Set(xA,yA);
		m_leftGripJointDef.localAnchorB.Set(xB,yB);

		m_shapeLeft.setRadius(10);
		m_shapeLeft.setFillColor(sf::Color(100, 250, 50));
		m_shapeLeft.setPosition(m_to_px*worldManifold.points[0].x-10,worldManifold.points[0].y*m_to_px-10);
	}

	else if(bodyA == m_rightWheel || bodyB == m_rightWheel)
	{
//		printf("right wheel \n");
		if(m_movingSide == LEFT){
//			printf("error contact side\n");
			m_movingSide = RIGHT;
		}
// Create the joint
		wheel_center.Set(0, 0); //in local body coordinates
		double xA = m_rightWheel->GetLocalPoint(worldManifold.points[0]).x;
		double yA = m_rightWheel->GetLocalPoint(worldManifold.points[0]).y;
		m_rightGripJointDef.bodyA = m_rightWheel;
		m_rightGripJointDef.bodyB = otherBody;
		m_rightGripJointDef.collideConnected = true;
		m_rightGripJointDef.localAnchorA.Set(xA,yA);
		m_rightGripJointDef.localAnchorB.Set(xB,yB);

		m_shapeRight.setRadius(10);
		m_shapeRight.setFillColor(sf::Color(100, 250, 50));
		m_shapeRight.setPosition(m_to_px*worldManifold.points[0].x-10,worldManifold.points[0].y*m_to_px-10);
	}

//
//	m_shape2.setRadius(4);
//	m_shape2.setFillColor(sf::Color(50, 150, 50));
//	m_shape2.setPosition(m_M_TO_PX*(m_robotBody->GetWorldPoint(wheel_center).x)-2,(m_robotBody->GetWorldPoint(wheel_center).y)*m_M_TO_PX-2);

	m_normal[0] = sf::Vertex(sf::Vector2f(xB*m_to_px, m_to_px*yB), sf::Color(200, 200, 150));
	m_normal[1] = sf::Vertex(sf::Vector2f(m_to_px*(xB+(worldManifold.normal).x), m_to_px*(yB+(worldManifold.normal).y)), sf::Color(200, 200, 150));

}
bool Robot::gripSide(b2Contact* contact, b2Body* otherBody, double m_to_px){ //ou alors fonction retourne un joint ?
	// std::cout << "gripSide()" << std::endl;
	// if(!contactOnGripSide(contact)){
	// 	return false;
	// }

	// TODO: Make it so that if a robot is grabbed, it can't grab any other robots.
	// This may solve the double grap (or "up-grab?") problem

	b2WorldManifold worldManifold;
	contact->GetWorldManifold(&worldManifold);
	b2Body* bodyA = contact->GetFixtureA()->GetBody();
	b2Body* bodyB = contact->GetFixtureB()->GetBody();

	//b2ManifoldPoint p;
	int i=0;
	int numPoints = contact->GetManifold()->pointCount;


	double xB = otherBody->GetLocalPoint(worldManifold.points[0]).x;
	double yB = otherBody->GetLocalPoint(worldManifold.points[0]).y;
	//double yB = otherBody->GetLocalPoint(p).y;
	b2Vec2 wheel_center;

	b2Body* movingWheel = getWheel(m_movingSide);

	if(bodyA==movingWheel || bodyB==movingWheel)
	{
//		printf("Robot.gripSide : moving side : %d ;0 = right, 1 = left \n", m_movingSide);
		// Create the joint

		wheel_center.Set(0, 0); //in local body coordinates
		double xA = movingWheel->GetLocalPoint(worldManifold.points[0]).x;
		double yA = movingWheel->GetLocalPoint(worldManifold.points[0]).y;

		double x = otherBody->GetLocalVector(worldManifold.normal).x;
		double y = otherBody->GetLocalVector(worldManifold.normal).y;

		if(m_movingSide == LEFT){
			m_leftGripJointDef.bodyB =  movingWheel;
			m_leftGripJointDef.bodyA = otherBody;
			m_leftGripJointDef.collideConnected = true;
			m_leftGripJointDef.localAnchorB.Set(xA,yA);
			m_leftGripJointDef.localAnchorA.Set(xB,yB);

//			m_leftGripJointDef.length = 0.001;
//			m_leftGripJointDef.frequencyHz = 30.0f;
//			m_leftGripJointDef.dampingRatio = 1.f;
//			m_leftGripJointDef.localAxisA.Set(x, y);

			m_leftGripJointDef.enableLimit = true;
			m_leftGripJointDef.lowerTranslation = 0;
			m_leftGripJointDef.upperTranslation = 0.001;

//			m_referenceAngle = m_robotBody->GetAngle();

			m_shapeLeft.setRadius(10);
			m_shapeLeft.setFillColor(sf::Color(100, 250, 50));
			m_shapeLeft.setPosition(m_to_px*(worldManifold.points[0].x+worldManifold.normal.x)-10,(worldManifold.points[0].y+worldManifold.normal.y)*m_to_px-10);

			return true;
		}

		else{
			m_rightGripJointDef.bodyB = movingWheel;
			m_rightGripJointDef.bodyA = otherBody;
			m_rightGripJointDef.collideConnected = true;
			m_rightGripJointDef.localAnchorB.Set(xA,yA);
			m_rightGripJointDef.localAnchorA.Set(xB,yB);

//			m_rightGripJointDef.length = 0.001;
//			m_rightGripJointDef.frequencyHz = 30.0f;
//			m_rightGripJointDef.dampingRatio = 1.f;

			m_rightGripJointDef.localAxisA.Set(x, y);

			m_rightGripJointDef.enableLimit = true;
			m_rightGripJointDef.lowerTranslation = 0;
			m_rightGripJointDef.upperTranslation = 0.001;

//			m_referenceAngle = m_robotBody->GetAngle();

			m_shapeRight.setRadius(10);
			m_shapeRight.setFillColor(sf::Color(100, 250, 50));
			m_shapeRight.setPosition(m_to_px*(worldManifold.points[0].x+worldManifold.normal.x)-10,(worldManifold.points[0].y+worldManifold.normal.y)*m_to_px-10);

			return true;
		}
	}
	return false;
}
void Robot::gripGroundFromPos(){

	//left wheel ------------------------------------------
	b2ContactEdge* contactList = m_leftWheel->GetContactList();

	if((contactList == nullptr))
	{
		return;
	}

	b2Contact* contact = contactList->contact;

	b2Body* bodyA = contact->GetFixtureA()->GetBody();
	b2Body* bodyB = contact->GetFixtureB()->GetBody();
	b2World* world = bodyA->GetWorld();

	if(!(bodyA==m_robotBody || bodyB==m_robotBody ))
	{

		b2WorldManifold worldManifold;
		contact->GetWorldManifold(&worldManifold);

		double xA = bodyA->GetLocalPoint(worldManifold.points[0]).x;
		double yA = bodyA->GetLocalPoint(worldManifold.points[0]).y;
		double xB = bodyB->GetLocalPoint(worldManifold.points[0]).x;
		double yB = bodyB->GetLocalPoint(worldManifold.points[0]).y;
		m_leftGripJointDef.bodyA = bodyA;
		m_leftGripJointDef.bodyB = bodyB;
		m_leftGripJointDef.collideConnected = true;
		m_leftGripJointDef.localAnchorA.Set(xA,yA);
		m_leftGripJointDef.localAnchorB.Set(xB,yB);

		m_leftGripJointDef.enableLimit = true;
		m_leftGripJointDef.lowerTranslation = 0;
		m_leftGripJointDef.upperTranslation = 0.001;

		m_leftGripJointDef.localAxisA.Set(0.0f, -1.0f);
	}

	else {
		contact = contactList->next->contact;

		bodyA = contact->GetFixtureA()->GetBody();
		bodyB = contact->GetFixtureB()->GetBody();

		if(!(bodyA==m_robotBody || bodyB==m_robotBody ))
		{
			b2WorldManifold worldManifold;
			contact->GetWorldManifold(&worldManifold);

			double xA = bodyA->GetLocalPoint(worldManifold.points[0]).x;
			double yA = bodyA->GetLocalPoint(worldManifold.points[0]).y;
			double xB = bodyB->GetLocalPoint(worldManifold.points[0]).x;
			double yB = bodyB->GetLocalPoint(worldManifold.points[0]).y;
			m_leftGripJointDef.bodyA = bodyA;
			m_leftGripJointDef.bodyB = bodyB;
			m_leftGripJointDef.collideConnected = true;
			m_leftGripJointDef.localAnchorA.Set(xA,yA);
			m_leftGripJointDef.localAnchorB.Set(xB,yB);

			m_leftGripJointDef.enableLimit = true;
			m_leftGripJointDef.lowerTranslation = 0;
			m_leftGripJointDef.upperTranslation = 0.001;

			m_leftGripJointDef.localAxisA.Set(0.0f, -1.0f);
		}
	}

	if(!(&m_leftGripJointDef==nullptr)){
		world->CreateJoint( &m_leftGripJointDef );
	}

	//right wheel ----------------------------------
	contactList = m_rightWheel->GetContactList();
	contact = contactList->contact;

	bodyA = contact->GetFixtureA()->GetBody();
	bodyB = contact->GetFixtureB()->GetBody();

	if(!(bodyA==m_robotBody || bodyB==m_robotBody ))
	{

		b2WorldManifold worldManifold;
		contact->GetWorldManifold(&worldManifold);

		double xA = bodyA->GetLocalPoint(worldManifold.points[0]).x;
		double yA = bodyA->GetLocalPoint(worldManifold.points[0]).y;
		double xB = bodyB->GetLocalPoint(worldManifold.points[0]).x;
		double yB = bodyB->GetLocalPoint(worldManifold.points[0]).y;
		m_rightGripJointDef.bodyA = bodyA;
		m_rightGripJointDef.bodyB = bodyB;
		m_rightGripJointDef.collideConnected = true;
		m_rightGripJointDef.localAnchorA.Set(xA,yA);
		m_rightGripJointDef.localAnchorB.Set(xB,yB);

		m_rightGripJointDef.enableLimit = true;
		m_rightGripJointDef.lowerTranslation = 0;
		m_rightGripJointDef.upperTranslation = 0.001;

		m_rightGripJointDef.localAxisA.Set(0.0f, -1.0f);
	}

	else {
		contact = contactList->next->contact;

		bodyA = contact->GetFixtureA()->GetBody();
		bodyB = contact->GetFixtureB()->GetBody();

		if(!(bodyA==m_robotBody || bodyB==m_robotBody ))
		{
			b2WorldManifold worldManifold;
			contact->GetWorldManifold(&worldManifold);

			double xA = bodyA->GetLocalPoint(worldManifold.points[0]).x;
			double yA = bodyA->GetLocalPoint(worldManifold.points[0]).y;
			double xB = bodyB->GetLocalPoint(worldManifold.points[0]).x;
			double yB = bodyB->GetLocalPoint(worldManifold.points[0]).y;
			m_rightGripJointDef.bodyA = bodyA;
			m_rightGripJointDef.bodyB = bodyB;
			m_rightGripJointDef.collideConnected = true;
			m_rightGripJointDef.localAnchorA.Set(xA,yA);
			m_rightGripJointDef.localAnchorB.Set(xB,yB);

			m_rightGripJointDef.enableLimit = true;
			m_rightGripJointDef.lowerTranslation = 0;
			m_rightGripJointDef.upperTranslation = 0.001;

			m_rightGripJointDef.localAxisA.Set(0.0f, -1.0f);
		}
	}

	if(!(&m_rightGripJointDef==nullptr)){
	world->CreateJoint( &m_rightGripJointDef );
	}

}

bool Robot::contactOnGripSide(b2Contact* contact){

	double angle =(getBody()->GetAngle());//-m_referenceAngle);
	while(angle < 0){
		angle += 2*PI;
	}
	angle = fmod(angle, (2*PI));

//	std::cout<<"Before wheel: "<<angle*RAD_TO_DEG<<std::endl;
	b2WorldManifold worldManifold;
	contact->GetWorldManifold(&worldManifold);
	b2Body* bodyA = contact->GetFixtureA()->GetBody();
	b2Body* bodyB = contact->GetFixtureB()->GetBody();

	b2Body* movingWheel = getWheel(m_movingSide);

	if(bodyA==movingWheel || bodyB==movingWheel)
	{

		return true;
		// double x0 = movingWheel->GetLocalPoint(worldManifold.points[0]).x;
		// double y0 = movingWheel->GetLocalPoint(worldManifold.points[0]).y;
		// double y = y0*cos(angle)- x0*sin(angle);
		// if(m_movingSide == RIGHT){
		// 	if(y>0){
		// 		return true;
		// 	}
		// }
		// else{
		// 	if(y<0){
		// 		return true;
		// 	}
		// }
	}

	return false;
}
bool Robot::checkGripp(side s){
		if(s == RIGHT){
			b2JointEdge* jointList = m_rightWheel->GetJointList();
			b2Joint* joint = jointList->joint;
			for (int i=0; i<6; i++){
		//			printf("Robot.isGrabbed 2 \n");
				if(!(joint == m_rightWheelMotor)){
		//			printf("Robot.isGrabbed TRUE \n");
					return true;
				}
				if((jointList->next == nullptr)){
					return false;
				}
				jointList = jointList->next;
				joint = jointList->joint;
			}
			return false;
		}
		else{
			b2JointEdge* jointList = m_leftWheel->GetJointList();
			b2Joint* joint = jointList->joint;
			for (int i=0; i<6; i++){
		//			printf("Robot.isGrabbed 2 \n");
				if(!(joint == m_leftWheelMotor)){
		//			printf("Robot.isGrabbed TRUE \n");
					return true;
				}
				if((jointList->next == nullptr)){
					return false;
				}
				jointList = jointList->next;
				joint = jointList->joint;
			}
			return false;
		}
}
bool Robot::isGrabbed(){
//	printf("Robot.isGrabbed \n");
	bool rightGrabbed = false;
	bool leftGrabbed = false;

	b2JointEdge* jointList = m_rightWheel->GetJointList();
	b2Joint* joint = jointList->joint;

	for (int i=0; i<6; i++){
//			printf("Robot.isGrabbed 2 \n");
		if(!(joint == m_rightWheelMotor || joint == m_currentGripJoint || joint == m_previousGripJoint)){
//			printf("Robot.isGrabbed TRUE \n");
			rightGrabbed = true;
			break;
		}
		if((jointList->next == nullptr)){
			break;
		}
		jointList = jointList->next;
		joint = jointList->joint;
	}

//	printf("Robot.isGrabbed \n");
	jointList = m_leftWheel->GetJointList();
	joint = jointList->joint;

	for (int i=0; i<6; i++){
//			printf("Robot.isGrabbed 2 \n");
		if(!(joint == m_leftWheelMotor || joint == m_currentGripJoint || joint == m_previousGripJoint)){
//			printf("Robot.isGrabbed TRUE \n");
			leftGrabbed = true;
			break;
		}
		if((jointList->next == nullptr)){
			break;
		}
		jointList = jointList->next;
		joint = jointList->joint;
	}

	m_isGrabbed = (leftGrabbed || rightGrabbed);
	return (leftGrabbed || rightGrabbed);
}

b2RevoluteJoint* Robot::getMotor(side s){
	if (s == LEFT){
		return m_leftWheelMotor;
	}
	else{
		return m_rightWheelMotor;
	}
}

int Robot::nbJoint(side s){
	if (s == LEFT){
		return m_nbJointLeft;
	}
	else{
		return m_nbJointRight;
	}
}
void Robot::incrementNbJoint(side s){
	if (s == LEFT){
		m_nbJointLeft++;
	}
	else{
		m_nbJointRight++;
	}
}
void Robot::resetNbJoint(side s){
	if (s == LEFT){
		m_nbJointLeft = 0;
	}
	else{
		m_nbJointRight = 0;
	}
}

//void Robot::releaseGrip(b2World* world, b2Joint* gripJoint){
//
//}

void Robot::setContact(bool contact){
	m_contact = contact;
}
void Robot::setDelay(int delay){
	//printf("set delay: %d tick \n", delay);
	m_delay = delay;
}
void Robot::setId(int id){
	m_id = id;
}
void Robot::setSpeed(double speed){
	m_angularSpeed = speed;
}
void Robot::setState(e_state state){
	m_state = state;
}

double Robot::getAngle(){
	double a = m_robotBody->GetAngle();
	return a;
}
b2Body* Robot::getBody(){
	return m_robotBody;
}
double Robot::getBodyLength(){
	double bl = 2*m_robotParameters.wheel_radius + m_robotParameters.wheel_distance;
	return bl;
}
int Robot::getDelay(){

	//printf("m_delai: %d tick \n", m_delay);
	return m_delay;
}
int Robot::getId(){
	return m_id;
}
b2PrismaticJointDef Robot::getJointDef(side s){
	if (s == LEFT){
		return m_leftGripJointDef;
	}
	else{
		return m_rightGripJointDef;
	}

}
b2Vec2 Robot::getPosition(){
	b2Vec2 pos = m_robotBody->GetPosition();
	return pos;
}
e_state Robot::getState(){
	return m_state;
}
b2Body* Robot::getWheel(side s){
	if (s == LEFT){
		return m_leftWheel;
	}
	else{
		return m_rightWheel;
	}
}


bool Robot::isContact(){
	return m_contact;
}
bool Robot::isMoving(){
	return m_moving;
}
bool Robot::isReady(){
	return m_ready;
}


void Robot::destroyBody(){
	m_leftWheel->GetWorld()->DestroyBody( m_leftWheel );
	m_rightWheel->GetWorld()->DestroyBody( m_rightWheel );
	m_robotBody->GetWorld()->DestroyBody( m_robotBody );
}
