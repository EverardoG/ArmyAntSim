/**
 *  @file V2BLTerrain.h
 *	@brief Implementation of the V2BLTerrain class which inherit from the Terrain class.
 *	@details Class for the V2BLTerrain creation including method for the Box2D implementation (Physics) and the SFMLF synchronization (graphics).
 *	The V2BL Terrain is a linear ground with a V shaped gap in the middle. The bottom of the V gap is truncated so that the bottom width is 2 Body length.
 *	The ground is located at m_posY from the top of the window/world and the left corner of the gap at a distance m_runaway from the left end of the window/world.
 *	The V gap is described by a given depth m_height and either a given angle m_angle (if terrainParam.v_angle > 0) or a given width m_width.
 *  @date 18 Feb. 2019
 *  @author lucie houel
 */

#ifndef V2BLTERRAIN_H_
#define V2BLTERRAIN_H_

#include "Terrain.h"

class V2BLTerrain: public Terrain  {
public:
	// V2BLTerrain();
	V2BLTerrain(b2World* world, config::sWindow windowParam,  config::sTerrain terrainParam, double bodyLength); // If want to define the dimension relatively to the body length
	virtual ~V2BLTerrain();

	/*Default terrain is linear ground of width 2*m_runaway located at m_posY from the top of the window*/
	/** The function create MUST be called if the terrain object has been created via the default constructor (ex when created dynamically with new),
	 * otherwise no need to use it
	 * @param world is a pointer on the Box2D world object
	 * @param window is the SFML window
	 * @param terrainParam are the terrain parameters (cf Config.h)
	 * @param WINDOW_X_PX is the x-size of the window. it is used to calculate the scale to convert from meters to pixels
	 * @param bodylength is the size of a robot. it is used to convert the dimension from body length to m */
	// void create(b2World* world, sf::RenderWindow& window,  config::sTerrain terrainParam, int WINDOW_X_PX, double bodyLength=1);
	//TODO remove WINDOW_X_PX parameters and deduce it from window with window.getSize().x

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

	void drawBody(sf::RenderTexture& texture);

	/** return the terrain type, from this class it returns V2BL_TERRAIN */
	e_terrain_type getType();

	/** @return the position of the Top left corner of the V in the box2D world coordinates [m]*/
	b2Vec2 getTopLeftCorner();
	/** @return the position of the Top right corner of the V in the box2D world coordinates [m]*/
	b2Vec2 getTopRightCorner();
	/** @return the position of the bottom of the V in the box2D world coordinates [m]*/
	b2Vec2 getBottom();

private:
	double m_bottom=0; //Distance of the truncated bottom of the V from the actual bottom of the V when it's not truncated
};

#endif /* V2BLTERRAIN_H_ */
