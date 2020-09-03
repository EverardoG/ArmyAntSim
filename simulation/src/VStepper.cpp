/*
 * VStepper.cpp
 *
 *  Created on: 28 Feb 2019
 *      Author: lucie
 */

#include "VStepper.h"

VStepper::VStepper() {
	// TODO Auto-generated constructor stub

}

VStepper::VStepper(b2World* world, sf::RenderWindow& window, config::sTerrain terrainParam, int WINDOW_X_PX, double bodyLength)
: Terrain(world, window, terrainParam, WINDOW_X_PX, bodyLength){
	m_M_TO_PX = WINDOW_X_PX /  (4*m_runaway);
	m_posY=10;

}

VStepper::~VStepper() {
	// TODO Auto-generated destructor stub
}

void VStepper::create(b2World* world, sf::RenderWindow& window, config::sTerrain terrainParam, int WINDOW_X_PX, double bodyLength){
	Terrain::create(world, window, terrainParam, WINDOW_X_PX, bodyLength);
	m_M_TO_PX = WINDOW_X_PX /  (3*m_runaway);
	printf("m_M_TO_PX: %f, \n", m_M_TO_PX);
	m_posY=7;
}

void VStepper::createBody(b2World* world){
    b2BodyDef BodyDef;
    BodyDef.position = b2Vec2(0, 0);
    BodyDef.type = b2_staticBody;
    m_groundBody = world->CreateBody(&BodyDef);

	b2EdgeShape edgeShape;

	b2FixtureDef firstR;
	edgeShape.Set( b2Vec2(0,m_posY), b2Vec2(m_runaway,m_posY) );
	firstR.shape = &edgeShape;
	m_groundBody->CreateFixture(&firstR);

	b2FixtureDef firstSlope;
	edgeShape.Set( b2Vec2(m_runaway,m_posY), b2Vec2(m_runaway-m_height/abs(tan(m_angle)),m_posY-m_height) );
	firstSlope.shape = &edgeShape;
	m_groundBody->CreateFixture(&firstSlope);

	b2FixtureDef corner;
	edgeShape.Set( b2Vec2(m_runaway-m_height/abs(tan(m_angle)),m_posY-m_height), b2Vec2(m_runaway-m_height/abs(tan(m_angle)),m_posY-1.2*m_height) );
	corner.shape = &edgeShape;
	m_groundBody->CreateFixture(&corner);

	b2FixtureDef flat;
	edgeShape.Set( b2Vec2(m_runaway-m_height/abs(tan(m_angle)),m_posY-1.2*m_height), b2Vec2(1.5*m_runaway+m_height/abs(tan(0.7*m_angle)),m_posY-1.2*m_height) );
	flat.shape = &edgeShape;
	m_groundBody->CreateFixture(&flat);

	b2FixtureDef corner2;
	edgeShape.Set(b2Vec2(1.5*m_runaway+m_height/abs(tan(0.7*m_angle)),m_posY-1.2*m_height), b2Vec2(1.5*m_runaway+m_height/abs(tan(0.7*m_angle)),m_posY-m_height) );
	corner2.shape = &edgeShape;
	m_groundBody->CreateFixture(&corner2);

	b2FixtureDef secondSlope;
	edgeShape.Set( b2Vec2(1.5*m_runaway+m_height/abs(tan(0.7*m_angle)),m_posY-m_height), b2Vec2(1.5*m_runaway,m_posY) );
	secondSlope.shape = &edgeShape;
	m_groundBody->CreateFixture(&secondSlope);

	b2FixtureDef endR;
	edgeShape.Set( b2Vec2(1.5*m_runaway,m_posY), b2Vec2(3*m_runaway,m_posY) );
	endR.shape = &edgeShape;
	m_groundBody->CreateFixture(&endR);

}

void VStepper::drawBody(sf::RenderWindow& window){
	sf::VertexArray lines(sf::LinesStrip, 8);
	lines[0].position = sf::Vector2f(0, (m_posY)*m_M_TO_PX);
	lines[1].position = sf::Vector2f(m_runaway*m_M_TO_PX, (m_posY)*m_M_TO_PX);
	lines[2].position = sf::Vector2f((m_runaway-m_height/abs(tan(m_angle)))*m_M_TO_PX, (m_posY-m_height)*m_M_TO_PX);
	lines[3].position = sf::Vector2f((m_runaway-m_height/abs(tan(m_angle)))*m_M_TO_PX, (m_posY-1.2*m_height)*m_M_TO_PX);
	lines[4].position = sf::Vector2f((1.5*m_runaway+m_height/abs(tan(0.7*m_angle)))*m_M_TO_PX, (m_posY-1.2*m_height)*m_M_TO_PX);
	lines[5].position = sf::Vector2f((1.5*m_runaway+m_height/abs(tan(0.7*m_angle)))*m_M_TO_PX, (m_posY-m_height)*m_M_TO_PX);
	lines[6].position = sf::Vector2f((1.5*m_runaway)*m_M_TO_PX, (m_posY)*m_M_TO_PX);
	lines[7].position = sf::Vector2f((3*m_runaway)*m_M_TO_PX, (m_posY)*m_M_TO_PX);

	lines[0].color = sf::Color::Black;
	lines[1].color = sf::Color::Black;
	lines[2].color = sf::Color::Black;
	lines[3].color = sf::Color::Black;
	lines[4].color = sf::Color::Black;
	lines[5].color = sf::Color::Black;
	lines[6].color = sf::Color::Black;
	lines[7].color = sf::Color::Black;

	window.draw(lines);
}

e_terrain_type VStepper::getType(){
	return V_STEPPER;
}
