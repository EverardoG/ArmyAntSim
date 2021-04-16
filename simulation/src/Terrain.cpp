/*
 * Terrain.cpp
 *
 *  Created on: 12 Feb 2019
 *      Author: lucie
 */

#include "Terrain.h"

Terrain::Terrain() {
	m_groundBody = nullptr;
	// TODO Auto-generated constructor stub
}

Terrain::Terrain(b2World* world, sf::RenderWindow& window,  config::sTerrain terrainParam, int WINDOW_X_PX, double bodyLength){
	m_bodyLength = bodyLength;
	m_groundBody = nullptr;
	if(terrainParam.v_angle > 0){
		m_angle = terrainParam.v_angle;
		m_width = tan(terrainParam.v_angle)*2*terrainParam.v_height;
		m_width =  m_width * bodyLength;
	}
	else{
		m_angle = atan2(terrainParam.v_height, terrainParam.v_width/2);
		m_width =  terrainParam.v_width * bodyLength;
	}

	m_height = terrainParam.v_height * bodyLength;
	m_runaway = terrainParam.runaway * bodyLength;

	m_M_TO_PX = WINDOW_X_PX /  (m_width);
	m_windowSize = window.getSize();
}

Terrain::~Terrain() {
	// TODO Auto-generated destructor stub
}

void Terrain::create(b2World* world, sf::RenderWindow& window, config::sTerrain terrainParam, int WINDOW_X_PX, double bodyLength){
	m_bodyLength = bodyLength;
	if(terrainParam.v_angle > 0){
		m_angle = terrainParam.v_angle;
		m_width = tan(terrainParam.v_angle)*2*terrainParam.v_height;
		m_width =  m_width * bodyLength;
	}
	else{
		m_angle = atan2(terrainParam.v_height, terrainParam.v_width/2);
		m_width =  terrainParam.v_width * bodyLength;
	}

	m_height = terrainParam.v_height * bodyLength;
	m_runaway = terrainParam.runaway * bodyLength;

	m_M_TO_PX = WINDOW_X_PX /  (2*m_runaway);
	printf("m_M_TO_PX parent: %f, \n", m_M_TO_PX);
}

void Terrain::createBody(b2World* world){

    b2BodyDef BodyDef;
    BodyDef.position = b2Vec2(0, 0);
    BodyDef.type = b2_staticBody;
    m_groundBody = world->CreateBody(&BodyDef);

	b2EdgeShape edgeShape;

	b2FixtureDef ground;
	edgeShape.Set( b2Vec2(0,m_posY), b2Vec2(3*m_runaway,m_posY) );
	ground.shape = &edgeShape;
	m_groundBody->CreateFixture(&ground);

}

sf::VertexArray Terrain::getLines() {
	sf::VertexArray lines(sf::LinesStrip, 2);
	lines[0].position = sf::Vector2f(0, m_posY*m_M_TO_PX);
	lines[1].position = sf::Vector2f(2*m_runaway*m_M_TO_PX, m_posY*m_M_TO_PX);

	lines[0].color = sf::Color::Black;
	lines[1].color = sf::Color::Black;
	return lines;
}

void Terrain::drawBody(sf::RenderWindow& window){
	sf::VertexArray lines = getLines();
	window.draw(lines);
}

void Terrain::drawBody(sf::RenderTexture& texture){
	sf::VertexArray lines = getLines();
	texture.draw(lines);
}

b2Body* Terrain::getBody(){
	return m_groundBody;
}

const double Terrain::getScale(){
	return m_M_TO_PX;
}

e_terrain_type Terrain::getType(){
	return DEFAULT;
}

b2Vec2 Terrain::getTopLeftCorner(){return b2Vec2(m_runaway,m_posY);};
b2Vec2 Terrain::getTopRightCorner(){return b2Vec2(m_runaway,m_posY);};
b2Vec2 Terrain::getBottom(){return b2Vec2(m_runaway,m_posY);};
