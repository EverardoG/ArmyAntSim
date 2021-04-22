/*
 * V2BLTerrain.cpp
 *
 *  Created on: 18 Feb 2019
 *      Author: lucie
 */

#include "V2BLTerrain.h"
#include "helpers.h"

// V2BLTerrain::V2BLTerrain() {
// 	// TODO Auto-generated constructor stub

// }

V2BLTerrain::V2BLTerrain(b2World* world, config::sWindow windowParam, config::sTerrain terrainParam, double bodyLength)
: Terrain(world, windowParam, terrainParam, bodyLength){
	m_M_TO_PX = m_windowSize.x /  (2*m_runaway+m_width);
	m_bottom = m_bodyLength/tan(m_angle);
}

V2BLTerrain::~V2BLTerrain() {
	// TODO Auto-generated destructor stub
}

// void V2BLTerrain::create(b2World* world, sf::RenderWindow& window, config::sTerrain terrainParam, int WINDOW_X_PX, double bodyLength){
// 	Terrain::create(world, window, terrainParam, WINDOW_X_PX, bodyLength);
// 	m_M_TO_PX = WINDOW_X_PX /  (2*m_runaway+m_width);
// 	printf("m_M_TO_PX: %f, \n", m_M_TO_PX);
// 	m_bottom = m_bodyLength/tan(m_angle);
// }

void V2BLTerrain::createBody(b2World* world){

	m_bottom = m_bodyLength/tan(m_angle);
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
	edgeShape.Set( b2Vec2(m_runaway,m_posY), b2Vec2(m_runaway+m_width/2-m_bodyLength,m_posY+m_height-m_bottom) );
	firstSlope.shape = &edgeShape;
	m_groundBody->CreateFixture(&firstSlope);

	b2FixtureDef bottom;
	edgeShape.Set( b2Vec2(m_runaway+m_width/2-m_bodyLength,m_posY+m_height-m_bottom), b2Vec2(m_runaway+m_width/2+m_bodyLength,m_posY+m_height-m_bottom) );
	bottom.shape = &edgeShape;
	m_groundBody->CreateFixture(&bottom);

	b2FixtureDef secondSlope;
	edgeShape.Set( b2Vec2(m_runaway+m_width/2+m_bodyLength,m_posY+m_height-m_bottom), b2Vec2(m_runaway+m_width,m_posY) );
	secondSlope.shape = &edgeShape;
	m_groundBody->CreateFixture(&secondSlope);

	b2FixtureDef endR;
	edgeShape.Set( b2Vec2(m_runaway+m_width,m_posY), b2Vec2(4*m_runaway+m_width,m_posY) );
	endR.shape = &edgeShape;
	m_groundBody->CreateFixture(&endR);

	printf("bottom =%f, \n ", m_bottom);
	printf("bodylength =%f, \n ", m_bodyLength);

}

void V2BLTerrain::drawBody(sf::RenderWindow& window){
	sf::VertexArray lines(sf::LinesStrip, 6);
	lines[0].position = sf::Vector2f(0, m_posY*m_M_TO_PX);
	lines[1].position = sf::Vector2f(m_runaway*m_M_TO_PX, m_posY*m_M_TO_PX);
	lines[2].position = sf::Vector2f((m_runaway+m_width/2-m_bodyLength)*m_M_TO_PX, (m_posY+m_height-m_bottom)*m_M_TO_PX);
	lines[3].position = sf::Vector2f((m_runaway+m_width/2+m_bodyLength)*m_M_TO_PX, (m_posY+m_height-m_bottom)*m_M_TO_PX);
	lines[4].position = sf::Vector2f((m_runaway+m_width)*m_M_TO_PX, m_posY*m_M_TO_PX);
	lines[5].position = sf::Vector2f((2*m_runaway+m_width)*m_M_TO_PX, m_posY*m_M_TO_PX);

	lines[0].color = sf::Color::Black;
	lines[1].color = sf::Color::Black;
	lines[2].color = sf::Color::Black;
	lines[3].color = sf::Color::Black;
	lines[4].color = sf::Color::Black;
	lines[5].color = sf::Color::Black;

	window.draw(lines);
}

void V2BLTerrain::drawBody(sf::RenderTexture& texture){
	sf::VertexArray lines(sf::LinesStrip, 6);
	lines[0].position = sf::Vector2f(0, m_posY*m_M_TO_PX);
	lines[1].position = sf::Vector2f(m_runaway*m_M_TO_PX, m_posY*m_M_TO_PX);
	lines[2].position = sf::Vector2f((m_runaway+m_width/2-m_bodyLength)*m_M_TO_PX, (m_posY+m_height-m_bottom)*m_M_TO_PX);
	lines[3].position = sf::Vector2f((m_runaway+m_width/2+m_bodyLength)*m_M_TO_PX, (m_posY+m_height-m_bottom)*m_M_TO_PX);
	lines[4].position = sf::Vector2f((m_runaway+m_width)*m_M_TO_PX, m_posY*m_M_TO_PX);
	lines[5].position = sf::Vector2f((2*m_runaway+m_width)*m_M_TO_PX, m_posY*m_M_TO_PX);

	lines[0].color = sf::Color::Black;
	lines[1].color = sf::Color::Black;
	lines[2].color = sf::Color::Black;
	lines[3].color = sf::Color::Black;
	lines[4].color = sf::Color::Black;
	lines[5].color = sf::Color::Black;

	texture.draw(lines);
}

/** @return the position of the Top left corner of the V in the box2D world coordinates [m]*/
b2Vec2 V2BLTerrain::getTopLeftCorner(){
	return b2Vec2(m_runaway, m_posY);
}
/** @return the position of the Top right corner of the V in the box2D world coordinates [m]*/
b2Vec2 V2BLTerrain::getTopRightCorner(){
	return b2Vec2((m_runaway+m_width), m_posY);
}
/** @return the position of bottom of the V in the box2D world coordinates [m]*/
b2Vec2 V2BLTerrain::getBottom(){
	return b2Vec2((m_runaway+m_width/2), (m_posY+m_height));
}

e_terrain_type V2BLTerrain::getType(){
	return V2BL_TERRAIN;
}
