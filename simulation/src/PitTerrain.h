/*
 * PitTerrain.h
 *
 *  Created on: 9 aug. 2021
 *      Author: ever
 */

#ifndef PITTERRAIN_H_
#define PITTERRAIN_H_

#include "Terrain.h"
#include <iostream>

// Point represents a point in 2D space on the simulation as a b2vec
using Point = b2Vec2;

// Polygon is a collection of points that form a polygon when connected
// Like a "Connect the dots" puzzle where the dot coordinates are lined up
// in order in a std::vector
using Polygon = std::vector<Point>;

// AllPolygons holds all the polygons on a particular terrain in no particular order
using AllPolygons = std::vector<Polygon>;

class PitTerrain: public Terrain {
public:
	PitTerrain(	b2World* world, config::sWindow windowParam, config::sTerrain terrainParam, double bodyLength);
	virtual ~PitTerrain();

	/// Create the Box2D body for the static object of the scene.
	/// @param m_to_pix, window_x_px, window_y_px: configuration of the window, they are usually defined in the config file.
	/// @param wall_w_m, wall_h_m: configuration of the walls, they are usually defined in the config file.
	void createBody(b2World* world);

	/// Draw the shapes corresponding to the Box2D body created previously.
	/// @param m_to_pix, window_x_px, window_y_px: configuration of the window, they are usually defined in the config file.
	/// @param wall_w_m, wall_h_m: configuration of the walls, they are usually defined in the config file.
	void drawBody(	sf::RenderWindow& window);

	void drawBody( sf::RenderTexture& texture);

	e_terrain_type getType();
private:
	AllPolygons allPolygons;
};

#endif /* PitTerrain_H_ */
