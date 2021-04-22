/*
 * BoxTerrain.cpp
 *
 *  Created on: 27 sept. 2018
 *      Author: lucie
 */

#include "BoxTerrain.h"

BoxTerrain::BoxTerrain(b2World* world, config::sWindow windowParam, config::sTerrain terrainParam, double bodyLength)
: Terrain(world, windowParam, terrainParam, bodyLength){
	// TODO Auto-generated constructor stub
	m_M_TO_PX = m_windowSize.x /  (1.2*m_width);
	m_posY=m_windowSize.y/m_M_TO_PX - (m_windowSize.y/m_M_TO_PX-m_height)/2;
	m_window_x = m_windowSize.x;
	m_window_y = m_windowSize.y;

}

BoxTerrain::~BoxTerrain() {
	// TODO Auto-generated destructor stub
}

// void BoxTerrain::create(b2World* world, sf::RenderWindow& window, config::sTerrain terrainParam, int WINDOW_X_PX, double bodyLength){
// 	Terrain::create(world, window, terrainParam, WINDOW_X_PX, bodyLength);
// 	m_M_TO_PX = WINDOW_X_PX /  (1.2*m_width);
// 	printf("m_M_TO_PX: %f, \n", m_M_TO_PX);
// 	m_posY=window.getSize().y/m_M_TO_PX - (window.getSize().y/m_M_TO_PX-m_height)/2;
// 	m_window_x = window.getSize().x;
// 	m_window_y = window.getSize().y;
// }

void BoxTerrain::createBody(b2World* world){

    b2BodyDef BodyDef;
    BodyDef.position = b2Vec2(0, 0);
    BodyDef.type = b2_staticBody;
    m_groundBody = world->CreateBody(&BodyDef);

	b2EdgeShape edgeShape;
	float h = m_window_y/m_M_TO_PX-m_height;//window_y_px/m_to_pix - wall_h_m;
	float l = m_window_x/m_M_TO_PX-m_width;//window_x_px/m_to_pix - wall_w_m;

	b2FixtureDef topEdgeFixtureDef;
	edgeShape.Set( b2Vec2(l/2,h/2), b2Vec2(m_window_x/m_M_TO_PX - l/2,h/2) );
	topEdgeFixtureDef.shape = &edgeShape;
	m_groundBody->CreateFixture(&topEdgeFixtureDef);

	b2FixtureDef bottomEdgeFixtureDef;
	edgeShape.Set( b2Vec2(l/2,m_window_y/m_M_TO_PX - h/2), b2Vec2(m_window_x/m_M_TO_PX - l/2,m_window_y/m_M_TO_PX - h/2) );
	bottomEdgeFixtureDef.shape = &edgeShape;
	m_groundBody->CreateFixture(&bottomEdgeFixtureDef);

	b2FixtureDef leftEdgeFixtureDef;
	edgeShape.Set( b2Vec2(l/2,h/2), b2Vec2(l/2,m_window_y/m_M_TO_PX - h/2) );
	leftEdgeFixtureDef.shape = &edgeShape;
	m_groundBody->CreateFixture(&leftEdgeFixtureDef);

	b2FixtureDef rightEdgeFixtureDef;
	edgeShape.Set( b2Vec2(m_window_x/m_M_TO_PX - l/2,m_window_y/m_M_TO_PX - h/2), b2Vec2(m_window_x/m_M_TO_PX - l/2,h/2) );
	rightEdgeFixtureDef.shape = &edgeShape;
	m_groundBody->CreateFixture(&rightEdgeFixtureDef);

}

sf::VertexArray BoxTerrain::getLines() {
	float h = m_windowSize.y - m_height*m_M_TO_PX;
	float l = m_windowSize.x - m_width*m_M_TO_PX;

	sf::VertexArray lines(sf::LinesStrip, 5);
	lines[0].position = sf::Vector2f(l/2, h/2);
	lines[1].position = sf::Vector2f(l/2, m_windowSize.y-h/2);
	lines[2].position = sf::Vector2f(m_windowSize.x-l/2, m_windowSize.y-h/2);
	lines[3].position = sf::Vector2f(m_windowSize.x-l/2, h/2);
	lines[4].position = sf::Vector2f(l/2, h/2);

	lines[0].color = sf::Color::Black;
	lines[1].color = sf::Color::Black;
	lines[2].color = sf::Color::Black;
	lines[3].color = sf::Color::Black;
	lines[4].color = sf::Color::Black;

	return lines;
}

void BoxTerrain::drawBody(sf::RenderWindow& window) {
	sf::VertexArray lines = getLines();
	window.draw(lines);
}

void BoxTerrain::drawBody(sf::RenderTexture& texture) {
	sf::VertexArray lines = getLines();
	texture.draw(lines);
}

e_terrain_type BoxTerrain::getType(){
	return BOX;
}

