/**
 *  @file Ramp.h
 *	@brief Implementation of the Ramp class which inherit from the Terrain class.
 *	@details Class for the Ramp creation including method for the Box2D implementation (Physics) and the SFMLF synchronization (graphics).
 *	The Ramp Terrain is an horizontal descending ramp.
 *	The top ground is located at m_posY from the top of the window/world and the ramp at a distance m_runaway from the left end of the window/world.
 *	The Ramp is described by a height of m_height (and an angle of 90°).
 *  @date 12 Feb. 2019
 *  @author lucie houel
 */
//TODO: implement the possibility to change the ramp angle

#ifndef RAMP_H_
#define RAMP_H_

#include "Terrain.h"

class Ramp: public Terrain{
public:
	Ramp();
	Ramp(b2World* world, sf::RenderWindow& window,  config::sTerrain terrainParam, int WINDOW_X_PX, double bodyLength=1); // If want to define the dimension relatively to the body length
	virtual ~Ramp();

	/*Default terrain is linear ground of width 2*m_runaway located at m_posY from the top of the window*/
	/** The function create MUST be called if the terrain object has been created via the default constructor (ex when created dynamically with new),
	 * otherwise no need to use it
	 * @param world is a pointer on the Box2D world object
	 * @param window is the SFML window
	 * @param terrainParam are the terrain parameters (cf Config.h)
	 * @param WINDOW_X_PX is the x-size of the window. it is used to calculate the scale to convert from meters to pixels
	 * @param bodylength is the size of a robot. it is used to convert the dimension from body length to m */
	void create(b2World* world, sf::RenderWindow& window,  config::sTerrain terrainParam, int WINDOW_X_PX, double bodyLength=1);

	/**
	 * Create the Box2D static body of the terrain
	 * @param world is a pointer on the Box2D world
	 */
	void createBody(b2World* world);

	/**
	 * Draw the body on the window using the SFML library
	 * @param window is the SFML window where the terrain will be drawn
	 */
	void drawBody(sf::RenderWindow& window);

	/** return the terrain type, from this class it returns V2BL_TERRAIN */
	e_terrain_type getType();

	/** @return the position of the Top left corner of the V in the box2D world coordinates [m]*/
	b2Vec2 getTopLeftCorner();
	/** @return the position of the Top right corner of the V in the box2D world coordinates [m]*/
	b2Vec2 getTopRightCorner();
	/** @return the position of the bottom of the V in the box2D world coordinates [m]*/
	b2Vec2 getBottom();
//
//	double getVLength();
};

#endif /* RAMP_H_ */
