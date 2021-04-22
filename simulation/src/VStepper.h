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
	VStepper(b2World* world, config::sWindow windowParam,  config::sTerrain terrainParam, double bodyLength);
	virtual ~VStepper();

	void createBody(b2World* world);
	void drawBody(sf::RenderWindow& window);
	void drawBody(sf::RenderTexture& texture);

	e_terrain_type getType();
};

#endif /* VSTEPPER_H_ */
