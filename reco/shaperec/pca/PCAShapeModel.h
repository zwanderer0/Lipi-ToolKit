/*****************************************************************************************
* Copyright (c) 2006 Hewlett-Packard Development Company, L.P.
* Permission is hereby granted, free of charge, to any person obtaining a copy of 
* this software and associated documentation files (the "Software"), to deal in 
* the Software without restriction, including without limitation the rights to use, 
* copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the 
* Software, and to permit persons to whom the Software is furnished to do so, 
* subject to the following conditions:
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
* INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
* PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT 
* HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
* CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE 
* OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. 
*****************************************************************************************/

/************************************************************************
 * SVN MACROS
 *
 * $LastChangedDate: 2008-07-18 15:00:39 +0530 (Fri, 18 Jul 2008) $
 * $Revision: 561 $
 * $Author: sharmnid $
 *
 ************************************************************************/
/************************************************************************
 * FILE DESCR: Definition of PCAShapeModel which holds the reference model information of a
 *			   particular shape
 *
 * CONTENTS:   
 *
 * AUTHOR:     Balaji R.
 *
 * DATE:       December 23, 2004
 * CHANGE HISTORY:
 * Author		Date			Description of change
 ************************************************************************/

#ifndef __PCASHAPEMODEL_H
#define __PCASHAPEMODEL_H

#include "LTKTypes.h"

using namespace std;

/**
 * @class PCAShapeModel
 * <p> This class stores the eigen vectors of a shape with shape label shapeId in model.<p> 
 */

class PCAShapeModel
{
private:

	int m_shapeId;				//	shape id of the shape

	float2DVector m_model;		//	corresponding shape model

	floatVector m_eigenValues;	// eigen values correponding to the chosen eigen vectors + the next greatest eigen value

public:

	/** @name Constructors and Destructor */
	//@{


	/**
	 * Default Constructor.
	 */

	PCAShapeModel();

	/**
	* Destructor
	*/

	~PCAShapeModel();

	// @}

	/**
	 * @name Getter Functions
	 */
	// @{

	/**
	 * This function returns the shape Id of the model
	 * @param void
	 *
	 * @return shape Id of the model.
	 */

	int getShapeId() const;

	/**
	 * This function returns the Eigen Vectors of the model
	 * @param void
	 *
	 * @return Eigen Vectors of the model.
	 */

	const float2DVector& getModel() const;

	/**
	 * This function returns the Eigen Values of the model
	 * @param void
	 *
	 * @return Eigen Values of the model.
	 */

	const floatVector& getEigenValues() const;

	// @}

	/**
	 * @name Setter Functions
	 */

	// @{

	/**
	 * This function sets shape Id of the model
	 * @param shapeId Identification tag of the shape
	 */

	int setShapeId(int shapeId);

	/**
	 * This function sets Eigen Vectors of the model
	 * @param modelParams Eigen Vectors of the model
	 */

	int setModel(const float2DVector& modelParams);

	/**
	 * This function sets Eigen Values of the model
	 * @param modelParams Eigen Values of the model
	 */

	int setEigenValues(const floatVector& eigenValues);

	// @}

};

#endif

//#ifndef __PCASHAPEMODEL_H
//#define __PCASHAPEMODEL_H


