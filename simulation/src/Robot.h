/**
 *  @file robot.h
 *	@brief Implement the Robot class and define enum linked to a robot object: e_state and side
 *	@details The methods implemented in this class are relative to a single robot behavior: from movement to grip creation.
 *  @date 26 sept. 2018
 *  @author lucie houel
 */

#ifndef ROBOT_H_
#define ROBOT_H_

#include "Box2D/Box2D.h"
#include "SFML/Graphics.hpp"
#include "SFML/Window.hpp"
#include <math.h>

#include "constants.h"
#include "Config.h"
/*----------------------TODOO
 * create a function to assign a robot to a controller after the robot has been created
 * */

/** @enum e_state
 * Describe the state of the robot, it can be either WALK if the robot is in the walking state or BRIDGE if the robot is in the bridge state
 * */
enum e_state {WALK, BRIDGE};
/** @enum side
 * Used to describe which side of the robot is concerned, for example regarding the moving foot
 * */
enum side {RIGHT, LEFT};


/** @class Robot
 * A Robot object is a set of physical properties (body, fixture, joints) and graphical features
 * that describes the robot.
 * This features are synchronized and the robot is controlled by proper member functions.
 * Those member functions allow to control the grip creation and the robot movement at the joint level.
 *
 * */
class Robot {

public:
	Robot();
	Robot(b2World* world, config::sRobot robotParameters, double posX, double posY, double angle = 0);
	virtual ~Robot();

	/** Create the box2D body of the robot and attach it to the world
	 * @param world: box2D world defined in the main program
	 * @param robotParameters: structure containing the robot parameters: only the body length and speed have to be defined
	 * @param posX, posY : initial position of the robot in this world in meters
	 * @param angle : initial angle of the robot in this world in rad
	 * */
	void createBody(b2World* world, config::sRobot m_robotParameters, double posX, double posY, double angle = 0);

	/** Use SFML to to draw the body of the robot on the window
	 * @param window: active SFML window
	 * @param m_to_pix is the conversion factor used to transform the meters to pixels
	 * */
	void drawBody(sf::RenderWindow& window, double m_to_pix);
	/** Use SFML to to draw all the joints of the robot on the window
	 * @param window: active SFML window
	 * @param m_to_pix is the conversion factor used to transform the meters to pixels
	 * */
	void drawJoint(sf::RenderWindow& window, double m_to_px);
	/** Use SFML to to draw the robot's grippers on the window
	 * @param window: active SFML window
	 * @param m_to_pix is the conversion factor used to transform the meters to pixels
	 * */
	void drawGripJoint(sf::RenderWindow& window, double m_to_px);

	/** First of the three different methods to move the robot body in the clockwise direction,
	 * This is the one used in the current implementation, the other two functions are alternative way to move the body but are currently not used
	 * The speed is applied via the m_angularSpeed class member (except for the force
	 * The impulse method uses a proportional action to reach the angular target speed
	 * */
	void moveBodyWithMotor();
	/** Second of the three different methods to move the robot body in the clockwise direction,
	 * The parameters might need to be adapted to fit the simulation parameters (object size, scale, weight...)
	 * The speed is applied via the m_angularSpeed class member (except for the force
	 * The impulse method uses a proportional action to reach the angular target speed
	 * */
	void moveBodyWithForce();
	/** Third of the three different methods to move the robot body in the clockwise direction,
	 * The parameters might need to be adapted to fit the simulation parameters (object size, scale, weight...)
	 * The speed is applied via the m_angularSpeed class member (except for the force
	 * The impulse method uses a proportional action to reach the angular target speed
	 * */
	void moveBodyWithImpulse();

	/** Turn off wheels' motor
	 * turnOffMotor disable the motor, it means that the rotation is freed
	 * @param s is the motor side can take LEFT or RIGHT value
	 * */
	void turnOffMotor(side s);
	/**
	 * blockMotorRotation keep the motor enable but set an infinitesimal rotation limit and the speed to 0 in order to block totally the rotation
	 * @param s is the motor side can take LEFT or RIGHT value
	 * */
	void blockMotorRotation(side s);
	/**
	 * allowMotorRotation, remove the rotation limit and keep the motor active
	 * @param s is the motor side can take LEFT or RIGHT value
	 * */
	void allowMotorRotation(side s);
	/** Limit the motor rotation between +/- a given limit angle around the reference angle.
	 * The reference angle is defined by the member attribute m_referenceAngleJoint
	 * @param s is the motor side can take LEFT or RIGHT value
	 * @param limit_angle_rad is the value that define the limit angle in rad
	 * */
	void limitMotorRotation(side s, double limit_angle_rad);

	/** Set the current robot state
	 * @param state is the new robot state, can take three values WALK or BRIDGE
	 * */
	void setState(e_state state);
	/** Access the current robot state
	 * @return the e_state value defining the robot state, can take two values WALK or BRIDGE
	 * */
	e_state getState();

	/** Get the Box2D body part representing the attach between the robot's wheels
	 * @return a pointer on the body of the attach between the two wheels
	 * */
	b2Body* getBody();
	/** Get the Box2D body part representing one of the two wheels of the robot
	 * @param s is the side of the robot (relative to the robot referential), can be either LEFT or RIGHT that define which wheel will be returned
	 * @return a pointer on the box2D wheel on the s side
	 * */
	b2Body* getWheel(side s);

	/** get the box2D definition of the prismatic joint used to create the gripper,
	 * this definition can then be used to create the actual grip joint
	 * @param s is the side of the robot (relative to the robot referential), can be either LEFT or RIGHT
	 * */
	b2PrismaticJointDef getJointDef(side s);

	/** Create the grip joint definition as a Box2D prismatic joint definition object.
	 * gripGroundFromPos is used when one needs to create a static robot:
	 * It directly create the joint object without the intermediate definition.
	 * @param contact: box2D contact object usually obtained via the contact listener
	 * @param otherBody: box2D body in contact with the robot
	 * @param m_to_px: scale conversion between box2d and sfml coordinates, used to draw the contact point
	 *
	 * update m_leftGripJointDef and m_rightGripJointDef depending on the contact side
	 * */
	void grip(b2Contact* contact, b2Body* otherBody, double m_to_px);
	/** Create the grip joint definition as a Box2D prismatic joint definition object.
	 * gripGroundFromPos is used when one needs to create a static robot:
	 * It directly create the joint object without the intermediate definition.
	 * @param contact: box2D contact object usually obtained via the contact listener
	 * @param otherBody: box2D body in contact with the robot
	 * @param m_to_px: scale conversion between box2d and sfml coordinates, used to draw the contact point
	 *
	 * update m_leftGripJointDef and m_rightGripJointDef depending on the contact side
	 * */
	void gripGroundFromPos();
	/** Create the grip joint definition as a Box2D prismatic joint definition object.
	 * gripGroundFromPos is used when one needs to create a static robot:
	 * It directly create the joint object without the intermediate definition.
	 * @param contact: box2D contact object usually obtained via the contact listener
	 * @param otherBody: box2D body in contact with the robot
	 * @param m_to_px: scale conversion between box2d and sfml coordinates, used to draw the contact point
	 *
	 * update m_leftGripJointDef and m_rightGripJointDef depending on the contact side
	 * */
	bool gripSide(b2Contact* contact, b2Body* otherBody, double m_to_px);


	bool contactOnGripSide(b2Contact* contact);

	/** Used in the contact listener to update the m_contact flag if the robot initiate the contact
	 *
	 * update m_contact
	 * */
	void setContact(bool contact);
	/** @return true if the robot has just initiated a contact and didn't create a joint yet.
	 * */
	bool isContact();

	/** setter and getter of the delay m_delay parameter
	 * @param delay is the number of iteration needed to be able to turn the motors on again
	 * */
	void setDelay(int delay);
	/** Get the current robot's delay as a number of iteration left for the robot before being able to be ready.
	 * @return the current robot's delay before being ready
	 * */
	int getDelay();

	/** Check if the robot is currently grabbing something (gripper on)
	 * @param side of the gripper can be either LEFT or RIGHT
	 * @return true if the gripper on the side is active
	 * */
	bool checkGripp(side s);

	/** Check if the robot is currently grabbed by another robot
	 * @return true if it is
	 * */
	bool isGrabbed();

	//void releaseGrip(b2World* world, b2Joint* gripJointDef);

	/** Methods to check the number of grip joints of the robot of a given side, this number should not exceed 1
	 * @param s is the side of the robot: can be either LEFT or RIGHT
	 * */
	int nbJoint(side s);
	/** Methods to increment the number of grip joints of the robot of a given side, this number should not exceed 1
	 * @param s is the side of the robot: can be either LEFT or RIGHT
	 * */
	void incrementNbJoint(side s);
	/** Methods to reset the number of grip joints of the robot of a given side, this number should not exceed 1
	 * @param s is the side of the robot: can be either LEFT or RIGHT
	 * */
	void resetNbJoint(side s);

	/** Get a pointer on the Box2D joint between a foot and the robot body that act as the motor.
	 * @param s side of the robot
	 * @return a pointer on the Box2D joint that act as the motor of the given side s
	 * */
	b2RevoluteJoint* getMotor(side s);

	/**
	 * Destroy the Box2D bodies composing the robot.
	 * */
	void destroyBody();

	/**
	 * @return the body length of the robot in meter (and not pixels)
	 * */
	double getBodyLength();

	/**
	 * @set the rotational robot speed, if the moveBodyWithMotor is used, the speed is in rad/s
	 * */
	void setSpeed(double speed);

	/**
	 * @return true if the robot is ready to move, false otherwise
	 * */
	bool isReady();

	/**
	 * @return true if the robot is moving, false otherwise
	 * */
	bool isMoving();

	/**
	 * Set the unique id of the robot. Usually it corresponds to the number of robot created (as opposed to number of active robots)
	 * @param id the unique id of the robot
	 *
	 * update m_id
	 * */
	void setId(int id);

	/**
	 * @return the value of the robot unique id stored in m_id
	 * */
	int getId();

	/** Get the robot center position (located at the center of the robot body) in m respectively to the origin of the Box2D world (top left corner)
	 * @return a b2Vec2 vector with the x and y coordinates of the robot position (m)
	 * */
	b2Vec2 getPosition();

	/** Get the robot angle (angle of the robot body) in rad respectively to the original angle.
	 * This angle might require to be processed to be comprised between 0 and 2Pi
	 * @return the angle of the robot (rad)
	 * */
	double getAngle();

	side m_movingSide = RIGHT; //Side of the foot that is moving (active motor joint)
	bool m_ready = false; //Flag to indicate when the robot is ready to move
	bool m_moving = false; //Flag to indicate that the robot is moving (active motor)
						   //Rq: could be replaced by a test on the value of the robot angular speed
	bool m_start = true; //Flag to indicate that the robot has just been created (necessary to create the first gripper)
	bool m_pushing = false; //Flag to indicate that the robot is pushing
	bool m_isGrabbed = false; //Flag to indicate that the robot is grabbed by another robot
	double m_referenceAngle = -PI ; //Reference angle of the robot (rad) taken at the previous grip creation
	double m_referenceAngleJoint = 0 ; //Reference angle of the joint involved in the previous grip creation at the time of the creation
									   //Currently not used but planned as a backup method
	int m_bridgeAge = 0; //Age of the stable bridge in number of iterations
	int m_age = 0; //Age of the robot in the bridge state. should be reseted when the robot leave the bridge state
	int m_pushing_delay = 0; //Pushing delay before which the robot is not able to grip if it didn't run through the limit angle (in number of iterations)

	/**
	 * Pointers on the two possible gripping joints
	 * */
	b2PrismaticJoint* m_previousGripJoint = nullptr;
	b2PrismaticJoint* m_currentGripJoint = nullptr;


private:

	/** Speed parameters*/
	double m_angularSpeed = 2*PI; //Angular speed of the robot's motors
	config::sRobot m_robotParameters; //Robot configuration parameters as defined in Config.h

	b2Body* m_robotBody; //Pointer on the Box2D body corresponding to the attach between the 2 feet of the robot
	b2Body* m_leftWheel; //Pointer on the Box2D body corresponding to the left foot of the robot
	b2Body* m_rightWheel; //Pointer on the Box2D body corresponding to the right foot of the robot

	/**
	 * Pointers on the two wheel joints
	 * */
	b2RevoluteJoint* m_rightWheelMotor;
	b2RevoluteJoint* m_leftWheelMotor;

	e_state m_state; //State of the robot. Can be either WALK or BRIDGE
	bool m_contact = false; //Flag to indicate if the robot is involved in a contact. Reset to false once the contact has been handled

	/**
	 * Box2D definition of the left and right grip joint.
	 * It is necessary to configure the joints in the contact listener because a new joint cannot be created directly in the listener.
	 * It acts as an intermediary and is used to create the joint later on.
	 */
	b2PrismaticJointDef m_leftGripJointDef; //Box2D definition of the left grip joint
	b2PrismaticJointDef m_rightGripJointDef;

	int m_nbJointLeft = 0; //Number of gripper belonging to the left foot
	int m_nbJointRight = 0; //Number of gripper belonging to the right foot

	int m_delay = 60; // Delay before the robot can be ready to move (in number of iteration). 60 fps => 60 = 1s de delay

	int m_id; //Unique id of the robot

	sf::CircleShape m_shapeRight; //Box2D shape of the right foot
	sf::CircleShape m_shapeLeft; //Box2D shape of the left foot
	sf::CircleShape m_shape3; //Backup shape used to draw the joints
	sf::Vertex m_normal[2]; //Box2D shape used to draw the gripper belonging (black line)

	double m_M_TO_PX; //Copy of the scale to do the conversion from real dimensions (m) to simulated ones (pixels)
};

#endif /* ROBOT_H_ */
