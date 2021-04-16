/*
 * BoxTerrain.h
 *
 *  Created on: 27 sept. 2018
 *      Author: lucie
 */

#ifndef BOXTERRAIN_H_
#define BOXTERRAIN_H_

#include "Terrain.h"

class BoxTerrain: public Terrain {
public:
	BoxTerrain();
	BoxTerrain(	b2World* world, sf::RenderWindow& window, config::sTerrain terrainParam, int WINDOW_X_PX, double bodyLength=1);
	virtual ~BoxTerrain();

	void create(b2World* world, sf::RenderWindow& window, config::sTerrain terrainParam, int WINDOW_X_PX, double bodyLength=1);

	/// Create the Box2D body for the static object of the scene.
	/// @param m_to_pix, window_x_px, window_y_px: configuration of the window, they are usually defined in the config file.
	/// @param wall_w_m, wall_h_m: configuration of the walls, they are usually defined in the config file.
	void createBody(b2World* world);

	/// Draw the shapes corresponding to the Box2D body created previously.
	/// @param m_to_pix, window_x_px, window_y_px: configuration of the window, they are usually defined in the config file.
	/// @param wall_w_m, wall_h_m: configuration of the walls, they are usually defined in the config file.
	void drawBody(	sf::RenderWindow& window);

	void drawBody(  sf::RenderTexture& texture);

	e_terrain_type getType();
private:
	int m_window_x=0;
	int m_window_y=0;

	sf::VertexArray getLines();
};

#endif /* BOXTERRAIN_H_ */
