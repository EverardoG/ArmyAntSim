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
	m_M_TO_PX = WINDOW_X_PX /  (1.2*m_width);
	m_posY=window.getSize().y/m_M_TO_PX - (window.getSize().y/m_M_TO_PX-m_height)/2;
	m_window_x = window.getSize().x;
	m_window_y = window.getSize().y;

}

CliffTerrain::~CliffTerrain() {
	// TODO Auto-generated destructor stub
}

void CliffTerrain::create(b2World* world, sf::RenderWindow& window, config::sTerrain terrainParam, int WINDOW_X_PX, double bodyLength){
	Terrain::create(world, window, terrainParam, WINDOW_X_PX, bodyLength);
	m_M_TO_PX = WINDOW_X_PX /  (1.2*m_width);
	printf("m_M_TO_PX: %f, \n", m_M_TO_PX);
	m_posY=window.getSize().y/m_M_TO_PX - (window.getSize().y/m_M_TO_PX-m_height)/2;
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


		// float h = window.getSize().y - m_height*m_M_TO_PX;
	// float l = window.getSize().x - m_width*m_M_TO_PX;

	// sf::VertexArray lines(sf::LinesStrip, 5);
	// lines[0].position = sf::Vector2f(l/2, h/2);
	// lines[1].position = sf::Vector2f(l/2, window.getSize().y-h/2);
	// lines[2].position = sf::Vector2f(window.getSize().x-l/2, window.getSize().y-h/2);
	// lines[3].position = sf::Vector2f(window.getSize().x-l/2, h/2);
	// lines[4].position = sf::Vector2f(l/2, h/2);

	// Define points for the rectangle frame
	Point topLeftPoint = b2Vec2(l/2,h/2);
	Point topRightPoint = b2Vec2(m_window_x/m_M_TO_PX - l/2,h/2);
	Point bottomRightPoint = b2Vec2(m_window_x/m_M_TO_PX - l/2,m_window_y/m_M_TO_PX - h/2);
	Point bottomLeftPoint = b2Vec2(l/2,m_window_y/m_M_TO_PX - h/2);

	// Create a closed rectangle polygon
	Polygon rectanglePolygon{
		topLeftPoint,
		topRightPoint,
		bottomRightPoint,
		bottomLeftPoint,
		topLeftPoint
	};

	// Create the Box2D edges for the rectangle
	for (int ptcount = 0; ptcount < rectanglePolygon.size()-1; ptcount++) {
		b2FixtureDef edgeFixtureDef;
		edgeShape.Set(rectanglePolygon[ptcount], rectanglePolygon[ptcount+1]);
		edgeFixtureDef.shape = &edgeShape;
		m_groundBody->CreateFixture(&edgeFixtureDef);
	}
	std::cout << "allPolygons.emplace_back()" << std::endl;
	// Store the rectanglePolygon for rendering
	allPolygons.emplace_back(rectanglePolygon);
}

void CliffTerrain::drawBody(sf::RenderWindow& window){
	// std::cout << "for (Polygon polygon : allPolygons) {" << std::endl;

	for (Polygon polygon : allPolygons) {
	// 	std::cout << "Polygon" << std::endl;
	// // 	// Create our lines object
	// // 	std::cout << "sf:: VertexArray lines(sf::LinesStrip, allPolygons.size());" << std::endl;
		sf::VertexArray lines(sf::LinesStrip, polygon.size());
	// 	std::cout << "lines of size " << polygon.size() << std::endl;

	// // 	// Populate lines with points from the polygon
	// // 	std::cout << "for (int ptcount = 0; ptcount < polygon.size(); ptcount++) {" << std::endl;
		for (int ptcount = 0; ptcount < polygon.size(); ptcount++) {
	// 		// std::cout << "lines[ptcount].position = sf::Vector2f(polygon[ptcount].x, polygon[ptcount].y);" << std::endl;
			lines[ptcount].position = sf::Vector2f(polygon[ptcount].x*m_M_TO_PX, polygon[ptcount].y*m_M_TO_PX);
			lines[ptcount].color = sf::Color::Black;
			std::cout << "Line " << ptcount << ": " << "x = " << polygon[ptcount].x << " | y = " << polygon[ptcount].y << std::endl;
		}

	float h = window.getSize().y - m_height*m_M_TO_PX;
	float l = window.getSize().x - m_width*m_M_TO_PX;

	// sf::VertexArray lines(sf::LinesStrip, 5);
	// lines[0].position = sf::Vector2f(l/2, h/2);
	std::cout << "Line " << 0 << ": " << "x = " << l/2 << " | y = " << h/2 << std::endl;

	// lines[1].position = sf::Vector2f(l/2, window.getSize().y-h/2);
	std::cout << "Line " << 1 << ": " << "x = " << l/2 << " | y = " << window.getSize().y-h/2 << std::endl;

	// lines[2].position = sf::Vector2f(window.getSize().x-l/2, window.getSize().y-h/2);
	std::cout << "Line " << 2 << ": " << "x = " << window.getSize().x-l/2 << " | y = " << window.getSize().y-h/2 << std::endl;

	// lines[3].position = sf::Vector2f(window.getSize().x-l/2, h/2);
	std::cout << "Line " << 3 << ": " << "x = " << window.getSize().x-l/2 << " | y = " << h/2 << std::endl;

	// lines[4].position = sf::Vector2f(l/2, h/2);
	std::cout << "Line " << 4 << ": " << "x = " << l/2 << " | y = " << h/2 << std::endl;


	// lines[0].color = sf::Color::Black;
	// lines[1].color = sf::Color::Black;
	// lines[2].color = sf::Color::Black;
	// lines[3].color = sf::Color::Black;
	// lines[4].color = sf::Color::Black;



	// 	std::cout << "window.draw(lines);" << std::endl;
		window.draw(lines);
	}
	std::cout << "Out of Polygon loop" << std::endl;

	// float h = window.getSize().y - m_height*m_M_TO_PX;
	// float l = window.getSize().x - m_width*m_M_TO_PX;

	// sf::VertexArray lines(sf::LinesStrip, 5);
	// lines[0].position = sf::Vector2f(l/2, h/2);
	// lines[1].position = sf::Vector2f(l/2, window.getSize().y-h/2);
	// lines[2].position = sf::Vector2f(window.getSize().x-l/2, window.getSize().y-h/2);
	// lines[3].position = sf::Vector2f(window.getSize().x-l/2, h/2);
	// lines[4].position = sf::Vector2f(l/2, h/2);

	// lines[0].color = sf::Color::Black;
	// lines[1].color = sf::Color::Black;
	// lines[2].color = sf::Color::Black;
	// lines[3].color = sf::Color::Black;
	// lines[4].color = sf::Color::Black;

	// window.draw(lines);

}

e_terrain_type CliffTerrain::getType(){
	return CLIFF;
}

