/*
 * IslandTerrain.cpp
 *
 *  Created on: 9 aug. 2021
 *      Author: ever
 */

#include "IslandTerrain.h"

IslandTerrain::IslandTerrain(b2World* world, config::sWindow windowParam, config::sTerrain terrainParam, double bodyLength)
: Terrain(world, windowParam, terrainParam, bodyLength){
	// TODO Auto-generated constructor stub
	m_M_TO_PX = m_windowSize.x /  25.0 * bodyLength;
	m_posY = 10 * bodyLength;

}

IslandTerrain::~IslandTerrain() {
	// TODO Auto-generated destructor stub
}

void IslandTerrain::createBody(b2World* world){

    b2BodyDef BodyDef;
    BodyDef.position = b2Vec2(0, 0);
    BodyDef.type = b2_staticBody;
    m_groundBody = world->CreateBody(&BodyDef);

	b2EdgeShape edgeShape;

	// Define points relevant to the bottom line
	Point leftPoint = b2Vec2(0.0, m_posY);
	Point rightPoint = b2Vec2(m_windowSize.x/ m_M_TO_PX, m_posY);
	// Create the ground polygon
	Polygon groundPolygon{
		leftPoint,
		rightPoint
	};

	// Add the ground polygon to the vector of all the polygons
	allPolygons.emplace_back(groundPolygon);

	// Define all the points relevant to the floating island
//	float islandTop = m_window_y/m_M_TO_PX - h * 1.75;
//	float islandBottom = m_window_y/m_M_TO_PX - h * 1.5;
//	float islandRight = m_window_x/m_M_TO_PX - l * 1.0;
//	float islandLeft = m_window_x/m_M_TO_PX - l * 3.0;

	float islandTop = m_posY - 5 * m_bodyLength;
	float islandLeft = 15 * m_bodyLength;
	float islandRight = 26 * m_bodyLength;
	float islandBottom = islandTop+ m_bodyLength;
//	float islandTop = cliffTop;
//	float islandBottom = cliffTop + m_bodyLength;
//	float islandLeft = cliffRightEdge + 6 * m_bodyLength;
//	// float islandRight = islandLeft + 4 * m_bodyLength;
//	float islandRight = m_windowSize.x*m_M_TO_PX;
//
	m_posGoal = b2Vec2(islandLeft, islandBottom);
//	// std::cout << "IslandTerrain::createBody() " << m_posGoal.x << std::endl;
//
	Point islandLeftTopPoint = b2Vec2(islandLeft, islandTop);
	Point islandRightTopPoint = b2Vec2(islandRight, islandTop);
	Point islandRightBottomPoint = b2Vec2(islandRight, islandBottom);
	Point islandLeftBottomPoint = b2Vec2(islandLeft, islandBottom);

	// Create the island polygon
	Polygon islandPolygon{
		islandLeftTopPoint,
		islandRightTopPoint,
		islandRightBottomPoint,
		islandLeftBottomPoint,
		islandLeftTopPoint,
	};

	// Add the island polygon to the vector of all the polygons
	allPolygons.emplace_back(islandPolygon);

	// Create the Box2D edges for all the polygons
	for (Polygon polygon : allPolygons) {
		for (int ptcount = 0; ptcount < polygon.size()-1; ptcount++) {
			b2FixtureDef edgeFixtureDef;
			edgeShape.Set(polygon[ptcount], polygon[ptcount+1]);
			edgeFixtureDef.shape = &edgeShape;
			m_groundBody->CreateFixture(&edgeFixtureDef);
		}
	}


}

void IslandTerrain::drawBody(sf::RenderWindow& window){
	// std::cout << "for (Polygon polygon : allPolygons) {" << std::endl;

	for (Polygon polygon : allPolygons) {
	// 	std::cout << "Polygon" << std::endl;
	// // 	// Create our lines object
	// // 	std::cout << "sf:: VertexArray lines(sf::LinesStrip, allPolygons.size());" << std::endl;
		sf::VertexArray lines(sf::LinesStrip, polygon.size());
	// 	std::cout << "lines of size " << polygon.size() << std::endl;

	    // Populate lines with points from the polygon
		for (int ptcount = 0; ptcount < polygon.size(); ptcount++) {
			// Be sure to turn meter measurements from points to pixel measurements for rendering
			lines[ptcount].position = sf::Vector2f(polygon[ptcount].x*m_M_TO_PX, polygon[ptcount].y*m_M_TO_PX);
			lines[ptcount].color = sf::Color::Black;
			// std::cout << "Line " << ptcount << ": " << "x = " << polygon[ptcount].x << " | y = " << polygon[ptcount].y << std::endl;
		}

		window.draw(lines);
	}
}

void IslandTerrain::drawBody(sf::RenderTexture& texture){
	// std::cout << "for (Polygon polygon : allPolygons) {" << std::endl;

	for (Polygon polygon : allPolygons) {
	// 	std::cout << "Polygon" << std::endl;
	// // 	// Create our lines object
	// // 	std::cout << "sf:: VertexArray lines(sf::LinesStrip, allPolygons.size());" << std::endl;
		sf::VertexArray lines(sf::LinesStrip, polygon.size());
	// 	std::cout << "lines of size " << polygon.size() << std::endl;

	    // Populate lines with points from the polygon
		for (int ptcount = 0; ptcount < polygon.size(); ptcount++) {
			// Be sure to turn meter measurements from points to pixel measurements for rendering
			lines[ptcount].position = sf::Vector2f(polygon[ptcount].x*m_M_TO_PX, polygon[ptcount].y*m_M_TO_PX);
			lines[ptcount].color = sf::Color::Black;
			// std::cout << "Line " << ptcount << ": " << "x = " << polygon[ptcount].x << " | y = " << polygon[ptcount].y << std::endl;
		}

		texture.draw(lines);
	}
}

e_terrain_type IslandTerrain::getType(){
	return ISLAND;
}

