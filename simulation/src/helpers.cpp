/*
 * helpers.cpp
 *
 *  Created on: 2 oct. 2018
 *      Author: lucie
 */
#include <math.h>
#include "helpers.h"

void worldToBodyCoordinates(float *posX, float *posY, b2Body* Body)
{
	*posX = (*posX-Body->GetPosition().x)*cos(Body->GetAngle())+(*posY-Body->GetPosition().y)*sin(Body->GetAngle());
	*posY = (*posY-Body->GetPosition().y)*cos(Body->GetAngle())-(*posX-Body->GetPosition().x)*sin(Body->GetAngle());
}

float distance(float x_start, float y_start, float x_end, float y_end){
	float d = sqrt((x_end-x_start)*(x_end-x_start) + (y_end-y_start)*(y_end-y_start) );
	return d;
}

double moduloAngle(double angle, double modulo){
	while(angle < 0){
		angle += 2*PI;
	}
	angle = fmod(angle, (modulo));
	return angle;
}


