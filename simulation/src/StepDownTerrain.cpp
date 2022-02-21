#include "StepDownTerrain.h"

StepDownTerrain::StepDownTerrain(b2World* world, config::sWindow windowParam, config::sTerrain terrainParam, double bodyLength)
: Terrain(world, windowParam, terrainParam, bodyLength){
	// TODO Auto-generated constructor stub
	m_M_TO_PX = m_windowSize.x /  25.0 * bodyLength;
	m_posY = 5 * bodyLength;

}

StepDownTerrain::~StepDownTerrain() {
	// TODO Auto-generated destructor stub
}

void StepDownTerrain::createBody(b2World* world){

    b2BodyDef BodyDef;
    BodyDef.position = b2Vec2(0, 0);
    BodyDef.type = b2_staticBody;
    m_groundBody = world->CreateBody(&BodyDef);

	b2EdgeShape edgeShape;

	float leftx = 0.0;
	float rightx = m_windowSize.x/ m_M_TO_PX;
	float pit_width = 7;
	float midx = (rightx - leftx) /2 + leftx;
	// Define points for L (left) platform
	Point leftPointL = b2Vec2(leftx, m_posY);
	Point rightPointL = b2Vec2(midx - pit_width/2, m_posY);
	Point rightbottomPointL = b2Vec2(midx - pit_width/2, m_posY + 1.5*m_bodyLength);
    Point leftbottomPointL = b2Vec2(leftx, m_posY + 1.5*m_bodyLength);
	// Create left platform polygon
	Polygon polygonL{
		leftPointL,
		rightPointL,
		rightbottomPointL,
        leftbottomPointL,
        leftPointL
	};
	// Add it to all polygons
	allPolygons.emplace_back(polygonL);

    // Define point for goal position on the island based on where the island would be
    // float islandTop = m_posY - 5 * m_bodyLength;
	// float islandLeft = 15 * m_bodyLength;

    // m_posGoal = b2Vec2(islandLeft, islandTop);

	// Define points for R (right) platform
    float right_platform_top = m_posY + 5 * m_bodyLength;
	Point rightbottomPointR = b2Vec2(m_windowSize.x/m_M_TO_PX, m_windowSize.y/m_M_TO_PX);
	Point leftbottomPointR = b2Vec2(midx + pit_width/2, m_windowSize.y/m_M_TO_PX);
	Point leftPointR = b2Vec2(midx + pit_width/2, right_platform_top);
	Point rightPointR = b2Vec2(m_windowSize.x/m_M_TO_PX, right_platform_top);
	// Create right polygon
	Polygon polygonR{
		leftbottomPointR,
		leftPointR,
		rightPointR,
		rightbottomPointR,
		leftbottomPointR
	};
	// Add it to all polygons
	allPolygons.emplace_back(polygonR);

	m_posGoal = leftPointR;
	// m_posGoal = b2Vec2(m_windowSize.x/m_M_TO_PX-2, m_posY);
	// b2Vec2 m_posGoal1 = b2Vec2(m_windowSize.x/m_M_TO_PX, m_posY-2);
	// Polygon goalp{
	// 	m_posGoal,
	// 	m_posGoal1
	// };
	// allPolygons.emplace_back(goalp);

	// Create the Box2D edges for all the polygons
	for (Polygon polygon : allPolygons) {
		for (int ptcount = 0; ptcount < polygon.size()-1; ptcount++) {
			b2FixtureDef edgeFixtureDef;
			edgeShape.Set(polygon[ptcount], polygon[ptcount+1]);
			edgeFixtureDef.shape = &edgeShape;
			m_groundBody->CreateFixture(&edgeFixtureDef);
		}
	}


	// m_posGoal = b2Vec2(islandLeft, islandTop);

}

void StepDownTerrain::drawBody(sf::RenderWindow& window){
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

void StepDownTerrain::drawBody(sf::RenderTexture& texture){
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

e_terrain_type StepDownTerrain::getType(){
	return ISLAND;
}

