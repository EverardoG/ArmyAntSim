/*
 * Vterrain.cpp
 *
 *  Created on: 8 oct. 2018
 *      Author: lucie
 */

#include "Vterrain.h"
#include <iostream>
#include "helpers.h"

Vterrain::Vterrain(){
}

Vterrain::Vterrain(b2World* world, sf::RenderWindow& window, config::sTerrain terrainParam, int WINDOW_X_PX, double bodyLength)
: Terrain(world, window, terrainParam, WINDOW_X_PX, bodyLength){
	m_M_TO_PX = WINDOW_X_PX /  (2*m_runaway+m_width);

}

Vterrain::~Vterrain() {
	// TODO Auto-generated destructor stub
}

void Vterrain::create(b2World* world, sf::RenderWindow& window, config::sTerrain terrainParam, int WINDOW_X_PX, double bodyLength){
	Terrain::create(world, window, terrainParam, WINDOW_X_PX, bodyLength);
	m_M_TO_PX = WINDOW_X_PX /  (2*m_runaway+m_width);
	printf("m_M_TO_PX: %f, \n", m_M_TO_PX);
}

void Vterrain::createBody(b2World* world){

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
	edgeShape.Set( b2Vec2(m_runaway,m_posY), b2Vec2(m_runaway+m_width/2,m_posY+m_height) );
	firstSlope.shape = &edgeShape;
	m_groundBody->CreateFixture(&firstSlope);

	b2FixtureDef secondSlope;
	edgeShape.Set( b2Vec2(m_runaway+m_width/2,m_posY+m_height), b2Vec2(m_runaway+m_width,m_posY) );
	secondSlope.shape = &edgeShape;
	m_groundBody->CreateFixture(&secondSlope);

	b2FixtureDef endR;
	edgeShape.Set( b2Vec2(m_runaway+m_width,m_posY), b2Vec2(4*m_runaway+m_width,m_posY) );
	endR.shape = &edgeShape;
	m_groundBody->CreateFixture(&endR);

//
//    b2BodyDef BodyDef;
//    BodyDef.position = b2Vec2(0, 0);
//    BodyDef.type = b2_staticBody;
//    m_groundBody = world->CreateBody(&BodyDef);
//
//	b2EdgeShape edgeShape;
//
//	b2FixtureDef firstR;
//	edgeShape.Set( b2Vec2(0,m_posY), b2Vec2(m_runaway,m_posY) );
//	firstR.shape = &edgeShape;
//	m_groundBody->CreateFixture(&firstR);
//
//	b2FixtureDef firstSlope;
//	edgeShape.Set( b2Vec2(m_runaway,m_posY), b2Vec2(m_runaway+m_height*sin(m_angle),m_posY+m_height*cos(m_angle)) );
//	firstSlope.shape = &edgeShape;
//	m_groundBody->CreateFixture(&firstSlope);
//
//	b2FixtureDef secondSlope;
//	edgeShape.Set( b2Vec2(m_runaway+m_height*sin(m_angle),m_posY+m_height*cos(m_angle)), b2Vec2(m_runaway+2*m_height*sin(m_angle),m_posY) );
//	secondSlope.shape = &edgeShape;
//	m_groundBody->CreateFixture(&secondSlope);
//
//	b2FixtureDef endR;
//	edgeShape.Set( b2Vec2(m_runaway+2*m_height*sin(m_angle),m_posY), b2Vec2(4*m_runaway+2*m_height*sin(m_angle),m_posY) );
//	endR.shape = &edgeShape;
//	m_groundBody->CreateFixture(&endR);
}

void Vterrain::drawBody(sf::RenderWindow& window){
	sf::VertexArray lines(sf::LinesStrip, 5);
	// m_M_TO_PX is 0 when I declare "Vterrain m_terrain" in Demo::Demo() in Demo.cpp
	// it is non-zero otherwise
	lines[0].position = sf::Vector2f(0, m_posY*m_M_TO_PX);
	lines[1].position = sf::Vector2f(m_runaway*m_M_TO_PX, m_posY*m_M_TO_PX);
	lines[2].position = sf::Vector2f((m_runaway+m_width/2)*m_M_TO_PX, (m_posY+m_height)*m_M_TO_PX);
	lines[3].position = sf::Vector2f((m_runaway+m_width)*m_M_TO_PX, m_posY*m_M_TO_PX);
	lines[4].position = sf::Vector2f((2*m_runaway+m_width)*m_M_TO_PX, m_posY*m_M_TO_PX);

//	lines[0].position = sf::Vector2f(0, m_posY*m_M_TO_PX);
//	lines[1].position = sf::Vector2f(m_runaway*m_M_TO_PX, m_posY*m_M_TO_PX);
//	lines[2].position = sf::Vector2f((m_runaway+m_height*sin(m_angle))*m_M_TO_PX, (m_posY+m_height*cos(m_angle))*m_M_TO_PX);
//	lines[3].position = sf::Vector2f((m_runaway+2*m_height*sin(m_angle))*m_M_TO_PX, m_posY*m_M_TO_PX);
//	lines[4].position = sf::Vector2f((2*m_runaway+2*m_height*sin(m_angle))*m_M_TO_PX, m_posY*m_M_TO_PX);

	lines[0].color = sf::Color::Black;
	lines[1].color = sf::Color::Black;
	lines[2].color = sf::Color::Black;
	lines[3].color = sf::Color::Black;
	lines[4].color = sf::Color::Black;

	window.draw(lines);
}

/** @return the position of the Top left corner of the V in the box2D world coordinates [m]*/
b2Vec2 Vterrain::getTopLeftCorner(){
	return b2Vec2(m_runaway, m_posY);
}
/** @return the position of the Top right corner of the V in the box2D world coordinates [m]*/
b2Vec2 Vterrain::getTopRightCorner(){
	return b2Vec2((m_runaway+m_width), m_posY);
}
/** @return the position of bottom of the V in the box2D world coordinates [m]*/
b2Vec2 Vterrain::getBottom(){
	return b2Vec2((m_runaway+m_width/2), (m_posY+m_height));
}

double Vterrain::getVLength(){
	double l = distance(m_runaway, m_posY, m_runaway+m_width/2, m_posY+m_height);
	l += distance(m_runaway+m_width, m_posY, m_runaway+m_width/2, m_posY+m_height);
	return l;
}

e_terrain_type Vterrain::getType(){
	return V_TERRAIN;
}

