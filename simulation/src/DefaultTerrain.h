/**
 *  @file Terrain.h
 *	@brief Implementation of the Terrain class.
 *	@details General class for the Terrain creation including method for the Box2D implementation (Physics) and the SFMLF synchronization (graphics).
 *	The default terrain is linear ground of width 3*m_runaway located at m_posY from the top of the window.
 *	The m_width, m_height and m_angle members do not represent actual dimension in the case of the default terrain.
 *  @date 28 sept. 2018
 *  @author lucie houel
 */

// Have it so that we set the goal position inside the terrain
// Maybe even the terrain config so that it can be grabbed from
// anywhere
// have some way of redefining goal positions

#ifndef DEFAULT_TERRAIN_H_
#define DEFAULT_TERRAIN_H_

#include "Terrain.h"

class DefaultTerrain : public Terrain{
public:
	DefaultTerrain(b2World* world, config::sWindow windowParam,  config::sTerrain terrainParam, double bodyLength);
	virtual ~DefaultTerrain();

	/*Default terrain is linear ground of width 2*m_runaway located at m_posY from the top of the window*/
	/** The function create MUST be called if the terrain object has been created via the default constructor (ex when created dynamically with new),
	 * otherwise no need to use it
	 * @param world is a pointer on the Box2D world object
	 * @param terrainParam are the terrain parameters (cf Config.h)
	 * @param WINDOW_X_PX is the x-size of the window. it is used to calculate the scale to convert from meters to pixels
	 * @param bodylength is the size of a robot. it is used to convert the dimension from body length to m */
	//TODO remove WINDOW_X_PX parameters and deduce it from window with window.getSize().x
	// virtual void create(b2World* world, config::sTerrain terrainParam, int WINDOW_X_PX, double bodyLength);

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

	/**
	 * Draw the body on the texture using the SFML library
	 * @param texture is the SFML texture where the terrain will be drawn
	 */
	void drawBody(sf::RenderTexture& texture);

	/**
	 * @return the type of the terrain. Can be DEFAULT, V_TERRAIN, V2BL_TERRAIN, RAMP, BOX or V_STEPPER
	 */
	e_terrain_type getType();

private:
	sf::VertexArray getLines();
};

#endif /* DEFAULT_TERRAIN_H_ */
