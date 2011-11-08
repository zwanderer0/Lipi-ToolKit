#ifndef NUMERICUTILITY_H_
#define NUMERICUTILITY_H_

#include<iostream>
#include<cmath>
#include"ConstantMacros.h"



using namespace std;

//! NumericUtility File for providing Numeric functions

//************************************
// Method:    addLogValues
// FullName:  addLogValues
// Access:    public 
// Returns:   float
// Qualifier:
// Parameter: float x
// Parameter: float y
//************************************
float addLogValues(float x,float y)
{
		float temp,diff,z;
		float minLogExp = -log(-LogZero);
		if (x<y) 
		{
			temp = x;
			x = y;
			y = temp;
		}	   
		diff = y-x;
		if(diff<minLogExp)   
		{
			if(x>LogThreshold)
				return x;
			else return LogZero;
		}
		else   z = exp(diff);
		return x+log(1.0+z);

}

#endif /*NUMERICUTILITY_H_*/






