/**
 *  @file Terrain.h
 *	@brief Implementation of the Terrain class.
 *	@details General class for the Terrain creation including method for the Box2D implementation (Physics) and the SFMLF synchronization (graphics).
 *	The default terrain is linear ground of width 3*m_runaway located at m_posY from the top of the window.
 *	The m_width, m_height and m_angle members do not represent actual dimension in the case of the default terrain.
 *  @date 28 sept. 2018
 *  @author lucie houel
 */

#ifndef TERRAIN_H_
#define TERRAIN_H_

#include "Box2D/Box2D.h"
#include "SFML/Graphics.hpp"
#include "SFML/Window.hpp"

#include "Config.h"

/**@enum e_terrain_type
 * Different types of terrain used to do the simulation
 */
enum e_terrain_type {DEFAULT, V_TERRAIN, V2BL_TERRAIN, RAMP, BOX, V_STEPPER, CLIFF};

class Terrain {
public:
	Terrain();
	Terrain(b2World* world, sf::RenderWindow& window,  config::sTerrain terrainParam, int WINDOW_X_PX, double bodyLength=1);
	virtual ~Terrain();

	/*Default terrain is linear ground of width 2*m_runaway located at m_posY from the top of the window*/
	/** The function create MUST be called if the terrain object has been created via the default constructor (ex when created dynamically with new),
	 * otherwise no need to use it
	 * @param world is a pointer on the Box2D world object
	 * @param window is the SFML window
	 * @param terrainParam are the terrain parameters (cf Config.h)
	 * @param WINDOW_X_PX is the x-size of the window. it is used to calculate the scale to convert from meters to pixels
	 * @param bodylength is the size of a robot. it is used to convert the dimension from body length to m */
	//TODO remove WINDOW_X_PX parameters and deduce it from window with window.getSize().x
	virtual void create(b2World* world, sf::RenderWindow& window, config::sTerrain terrainParam, int WINDOW_X_PX, double bodyLength);

	/**
	 * Create the Box2D static body of the terrain
	 * @param world is a pointer on the Box2D world
	 */
	virtual void createBody(b2World* world);

	/**
	 * Draw the body on the window using the SFML library
	 * @param window is the SFML window where the terrain will be drawn
	 */
	virtual void drawBody(sf::RenderWindow& window);

	/**
	 * @return the type of the terrain. Can be DEFAULT, V_TERRAIN, V2BL_TERRAIN, RAMP, BOX or V_STEPPER
	 */
	virtual e_terrain_type getType();

	/** @return the position of the Top left corner of the V in the box2D world coordinates [m]*/
	virtual b2Vec2 getTopLeftCorner();
	/** @return the position of the Top right corner of the V in the box2D world coordinates [m]*/
	virtual b2Vec2 getTopRightCorner();
	/** @return the position of the bottom of the V in the box2D world coordinates [m]*/
	virtual b2Vec2 getBottom();

	/** @return the length of the V when it makes sense, 0 otherwise [m] */
	double getVLength(){return 0;};

	/** @return a pointer on the Box2D body of the terrain*/
	b2Body* getBody();

	/**As the terrain is the first thing that has to be created in the world,
	 * this method allow to obtain a right scale factor to do the conversion between the real dimensions and the simulated ones
	 * based on and adapted to the terrain dimensions
	 * @return the scale to do the conversion from real dimensions (m) to simulated ones (pixels)*/
	const double getScale();

protected:
	b2Body* m_groundBody; //Pointer on the Box2D body of the terrain
	double m_bodyLength; //Copy of the body length unit. Used to convert the body length units given by the user to real ones (in m)
	double m_M_TO_PX; //Scale to do the conversion from real dimensions (m) to simulated ones (pixels)
	double m_width; //Width of the terrain when it makes sense. In the case of the default terrain it doesn't represent an actual dimension
	double m_height; //Height of the terrain. In the case of the default terrain it doesn't represent an actual dimension.
	double m_runaway; //Runaway of the terrain. In the case of the default terrain the length of the ground is of 3*m_runaway (with the width of the window of 2*m_runaway)
	double m_angle; //Angle of the obstacle in the terrain. In the case of the default terrain it doesn't make sense and is set to 0
	double m_posY=2.5; //In the case of the default terrain it represents the distance from the top of the window/world to the ground.

	config::sTerrain m_terrainParam; //Terrain configuration parameters described in Config.h file
};

#endif /* TERRAIN_H_ */
