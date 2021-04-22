/*
 * Terrain.cpp
 *
 *  Created on: 12 Feb 2019
 *      Author: lucie
 */

#include "DefaultTerrain.h"

// Terrain::Terrain() {
// 	m_groundBody = nullptr;
// 	// TODO Auto-generated constructor stub
// }

DefaultTerrain::DefaultTerrain(b2World* world, config::sWindow windowParam,  config::sTerrain terrainParam, double bodyLength)
: Terrain(world, windowParam, terrainParam, bodyLength){}

DefaultTerrain::~DefaultTerrain() {
	// TODO Auto-generated destructor stub
}

void DefaultTerrain::createBody(b2World* world){

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

sf::VertexArray DefaultTerrain::getLines() {
	sf::VertexArray lines(sf::LinesStrip, 2);
	lines[0].position = sf::Vector2f(0, m_posY*m_M_TO_PX);
	lines[1].position = sf::Vector2f(2*m_runaway*m_M_TO_PX, m_posY*m_M_TO_PX);

	lines[0].color = sf::Color::Black;
	lines[1].color = sf::Color::Black;
	return lines;
}

void DefaultTerrain::drawBody(sf::RenderWindow& window){
	sf::VertexArray lines = getLines();
	window.draw(lines);
}

void DefaultTerrain::drawBody(sf::RenderTexture& texture){
	sf::VertexArray lines = getLines();
	texture.draw(lines);
}

e_terrain_type DefaultTerrain::getType(){
	return DEFAULT;
}