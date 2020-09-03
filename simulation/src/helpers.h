/*
 * helpers.h
 *
 *  Created on: 2 oct. 2018
 *      Author: lucie
 */

#ifndef HELPERS_H_
#define HELPERS_H_

#include "Box2D/Box2D.h"
#include "constants.h"

void worldToBodyCoordinates(float *posX, float *posY, b2Body* Body);
float distance(float x_start, float y_start, float x_end, float y_end);
double moduloAngle(double angle, double modulo = 2*PI);

#endif /* HELPERS_H_ */
