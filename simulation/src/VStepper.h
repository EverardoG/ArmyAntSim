/*
 * VStepper.h
 *
 *  Created on: 28 Feb 2019
 *      Author: lucie
 */

#ifndef VSTEPPER_H_
#define VSTEPPER_H_

#include "Terrain.h"

class VStepper: public Terrain {
public:
	VStepper();
	VStepper(b2World* world, sf::RenderWindow& window,  config::sTerrain terrainParam, int WINDOW_X_PX, double bodyLength=1);
	virtual ~VStepper();

	void create(b2World* world, sf::RenderWindow& window,  config::sTerrain terrainParam, int WINDOW_X_PX, double bodyLength=1);
	void createBody(b2World* world);
	void drawBody(sf::RenderWindow& window);

	e_terrain_type getType();
};

#endif /* VSTEPPER_H_ */
