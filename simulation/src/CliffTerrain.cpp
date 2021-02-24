/*
 * CliffTerrain.cpp
 *
 *  Created on: 6 feb. 2021
 *      Author: ever
 */

#include "CliffTerrain.h"

CliffTerrain::CliffTerrain(){};

CliffTerrain::CliffTerrain(b2World* world, sf::RenderWindow& window, config::sTerrain terrainParam, int WINDOW_X_PX, double bodyLength)
: Terrain(world, window, terrainParam, WINDOW_X_PX, bodyLength){
	// TODO Auto-generated constructor stub
	m_M_TO_PX = WINDOW_X_PX /  25.0 * bodyLength;
	std::cout << "window.getSize().y : " << window.getSize().y << std::endl;
	std::cout << "m_M_TO_PX : " << m_M_TO_PX << std::endl;
	std::cout << "window.getSize().y/m_M_TO_PX : " << window.getSize().y/m_M_TO_PX << std::endl;
	std::cout << "m_height : " << m_height << std::endl;
	std::cout << "(window.getSize().y/m_M_TO_PX-m_height) : " << (window.getSize().y/m_M_TO_PX-m_height) << std::endl;
	std::cout << "(window.getSize().y/m_M_TO_PX-m_height)/2 : " << (window.getSize().y/m_M_TO_PX-m_height)/2 << std::endl;
	std::cout << "window.getSize().y/m_M_TO_PX - (window.getSize().y/m_M_TO_PX-m_height)/2 : " << window.getSize().y/m_M_TO_PX - (window.getSize().y/m_M_TO_PX-m_height)/2 << std::endl;

	// m_posY=window.getSize().y/m_M_TO_PX/2;
	m_posY = 5 * bodyLength;
	// m_posY=0.0;
	m_window_x = window.getSize().x;
	m_window_y = window.getSize().y;

}

CliffTerrain::~CliffTerrain() {
	// TODO Auto-generated destructor stub
}

void CliffTerrain::create(b2World* world, sf::RenderWindow& window, config::sTerrain terrainParam, int WINDOW_X_PX, double bodyLength){
	Terrain::create(world, window, terrainParam, WINDOW_X_PX, bodyLength);

	std::cout << "WINDOW_X_PX : " << WINDOW_X_PX << std::endl;
	std::cout << "m_width : " << m_width << std::endl;

	// m_M_TO_PX = WINDOW_X_PX /  (1.2*m_width);
	m_M_TO_PX = WINDOW_X_PX /  25.0 * bodyLength;
	printf("m_M_TO_PX: %f, \n", m_M_TO_PX);

	std::cout << "window.getSize().y : " << window.getSize().y << std::endl;
	std::cout << "m_M_TO_PX : " << m_M_TO_PX << std::endl;
	std::cout << "window.getSize().y/m_M_TO_PX : " << window.getSize().y/m_M_TO_PX << std::endl;
	std::cout << "m_height : " << m_height << std::endl;
	std::cout << "(window.getSize().y/m_M_TO_PX-m_height) : " << (window.getSize().y/m_M_TO_PX-m_height) << std::endl;
	std::cout << "(window.getSize().y/m_M_TO_PX-m_height)/2 : " << (window.getSize().y/m_M_TO_PX-m_height)/2 << std::endl;
	std::cout << "window.getSize().y/m_M_TO_PX - (window.getSize().y/m_M_TO_PX-m_height)/2 : " << window.getSize().y/m_M_TO_PX - (window.getSize().y/m_M_TO_PX-m_height)/2 << std::endl;

	// m_posY=window.getSize().y/m_M_TO_PX - (window.getSize().y/m_M_TO_PX-m_height)/2;
	// m_posY=window.getSize().y/m_M_TO_PX/2;
	m_posY = 5 * bodyLength;
	// m_posY=0.0;
	m_window_x = window.getSize().x;
	m_window_y = window.getSize().y;
}

void CliffTerrain::createBody(b2World* world){

    b2BodyDef BodyDef;
    BodyDef.position = b2Vec2(0, 0);
    BodyDef.type = b2_staticBody;
    m_groundBody = world->CreateBody(&BodyDef);

	b2EdgeShape edgeShape;
	float h = m_window_y/m_M_TO_PX-m_height;//window_y_px/m_to_pix - wall_h_m;
	float l = m_window_x/m_M_TO_PX-m_width;//window_x_px/m_to_pix - wall_w_m;

	// Questions:
	// What does l represent?
	// What does h represent?
	// What does m_height represent?
	// What does m_width represent?


		// float h = window.getSize().y - m_height*m_M_TO_PX;
	// float l = window.getSize().x - m_width*m_M_TO_PX;

	// sf::VertexArray lines(sf::LinesStrip, 5);
	// lines[0].position = sf::Vector2f(l/2, h/2);
	// lines[1].position = sf::Vector2f(l/2, window.getSize().y-h/2);
	// lines[2].position = sf::Vector2f(window.getSize().x-l/2, window.getSize().y-h/2);
	// lines[3].position = sf::Vector2f(window.getSize().x-l/2, h/2);
	// lines[4].position = sf::Vector2f(l/2, h/2);

	// Define points for the rectangle frame
	// Point topLeftPoint = b2Vec2(l/2,h/2);
	// Point topRightPoint = b2Vec2(m_window_x/m_M_TO_PX - l/2,h/2);
	// Point bottomRightPoint = b2Vec2(m_window_x/m_M_TO_PX - l/2,m_window_y/m_M_TO_PX - h/2);
	// Point bottomLeftPoint = b2Vec2(l/2,m_window_y/m_M_TO_PX - h/2);
	// Point arbitraryPoint = b2Vec2(1.0, 5.0);

	// Point topLeftPoint = b2Vec2(l/2,h/2);
	// Point topRightPoint = b2Vec2(m_window_x/m_M_TO_PX - l/2,h/2);


	// Define all the points relevant to the cliff edges
	// float cliffTop = m_window_y/m_M_TO_PX - h * 1.5;
	float cliffTop = m_posY;
	float cliffRightEdge = m_runaway;//10 * m_bodyLength;
	float groundBottomEdge = m_posY + (6 * m_bodyLength);

	Point cliffLeftPoint = b2Vec2(0.0, cliffTop);
	Point cliffRightTopPoint = b2Vec2(cliffRightEdge, cliffTop);
	Point cliffRightBottomPoint = b2Vec2(cliffRightEdge, groundBottomEdge);
	Point cliffEndPoint = b2Vec2(m_window_x/m_M_TO_PX, groundBottomEdge);

	// Point arbitraryPoint = b2Vec2(1.0, 5.0);

	// Create the cliff polygon
	Polygon cliffPolygon{
		cliffLeftPoint,  // 0.66 | 4.49
		cliffRightTopPoint, // 9.17 | 4.49
		cliffRightBottomPoint,     // 0.65 | 0.92
		cliffEndPoint,
		// topRightPoint,    // 9.17 | 0.92
		// // arbitraryPoint,   // 1.00 | 3.00
		// topLeftPoint      // 0.66 | 0.92
	};

	// Add the cliff polygon to the vector of all the polygons
	allPolygons.emplace_back(cliffPolygon);

	// Define all the points relevant to the floating island
	// float islandTop = m_window_y/m_M_TO_PX - h * 1.75;
	// float islandBottom = m_window_y/m_M_TO_PX - h * 1.5;
	// float islandRight = m_window_x/m_M_TO_PX - l * 1.0;
	// float islandLeft = m_window_x/m_M_TO_PX - l * 3.0;

	float islandTop = cliffTop;
	float islandBottom = cliffTop + m_bodyLength;
	float islandLeft = cliffRightEdge + 3 * m_bodyLength;
	float islandRight = islandLeft + 4 * m_bodyLength;


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

void CliffTerrain::drawBody(sf::RenderWindow& window){
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

e_terrain_type CliffTerrain::getType(){
	return CLIFF;
}

