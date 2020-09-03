/*
 * Ramp.cpp
 *
 *  Created on: 12 Feb 2019
 *      Author: lucie
 */

#include "Ramp.h"

Ramp::Ramp() {
	// TODO Auto-generated constructor stub

}

Ramp::Ramp(b2World* world, sf::RenderWindow& window, config::sTerrain terrainParam, int WINDOW_X_PX, double bodyLength)
: Terrain(world, window, terrainParam, WINDOW_X_PX, bodyLength){
	m_M_TO_PX = WINDOW_X_PX /  (2*m_runaway);
}

Ramp::~Ramp() {
	// TODO Auto-generated destructor stub
}

void Ramp::create(b2World* world, sf::RenderWindow& window, config::sTerrain terrainParam, int WINDOW_X_PX, double bodyLength){
	Terrain::create(world, window, terrainParam, WINDOW_X_PX, bodyLength);
	m_M_TO_PX = WINDOW_X_PX /  (2*m_runaway);
	printf("m_M_TO_PX: %f, \n", m_M_TO_PX);
}

void Ramp::createBody(b2World* world){

    b2BodyDef BodyDef;
    BodyDef.position = b2Vec2(0, 0);
    BodyDef.type = b2_staticBody;
    m_groundBody = world->CreateBody(&BodyDef);

	b2EdgeShape edgeShape;

	b2FixtureDef firstR;
	edgeShape.Set( b2Vec2(0,m_posY), b2Vec2(m_runaway,m_posY) );
	firstR.shape = &edgeShape;
	m_groundBody->CreateFixture(&firstR);

	b2FixtureDef slope;
	edgeShape.Set( b2Vec2(m_runaway,m_posY), b2Vec2(m_runaway,m_posY+m_height) );
	slope.shape = &edgeShape;
	m_groundBody->CreateFixture(&slope);

	b2FixtureDef endR;
	edgeShape.Set( b2Vec2(m_runaway,m_posY+m_height), b2Vec2(3*m_runaway,m_posY+m_height) );
	endR.shape = &edgeShape;
	m_groundBody->CreateFixture(&endR);

}

void Ramp::drawBody(sf::RenderWindow& window){
	sf::VertexArray lines(sf::LinesStrip, 4);
	lines[0].position = sf::Vector2f(0, m_posY*m_M_TO_PX);
	lines[1].position = sf::Vector2f(m_runaway*m_M_TO_PX, m_posY*m_M_TO_PX);
	lines[2].position = sf::Vector2f(m_runaway*m_M_TO_PX, (m_posY+m_height)*m_M_TO_PX);
	lines[3].position = sf::Vector2f(2*m_runaway*m_M_TO_PX, (m_posY+m_height)*m_M_TO_PX);


	lines[0].color = sf::Color::Black;
	lines[1].color = sf::Color::Black;
	lines[2].color = sf::Color::Black;
	lines[3].color = sf::Color::Black;

	window.draw(lines);
}

/** @return the position of the Top left corner of the V in the box2D world coordinates [m]*/
b2Vec2 Ramp::getTopLeftCorner(){
	return b2Vec2(m_runaway, m_posY);
}
/** @return the position of the Top right corner of the V in the box2D world coordinates [m]*/
b2Vec2 Ramp::getTopRightCorner(){
	return b2Vec2(m_runaway, m_posY);
}
/** @return the position of bottom of the V in the box2D world coordinates [m]*/
b2Vec2 Ramp::getBottom(){
	return b2Vec2((m_runaway), (m_posY+m_height));
}

e_terrain_type Ramp::getType(){
	return RAMP;
}


