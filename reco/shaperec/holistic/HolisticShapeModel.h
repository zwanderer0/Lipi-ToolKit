/****************************************************************************
* Copyright (c) HP Labs India. All rights reserved.
* This source code is a property of HP Labs India. and is copyright protected.
* UNATHORISED ACCESS, COPYING, REPRODUCTION OR DISTRIBUTION of this program
* is strictly PROHIBITED.
****************************************************************************/

/************************************************************************
 * SVN MACROS
 *
 * $LastChangedDate: 2008-07-18 15:00:39 +0530 (Fri, 18 Jul 2008) $
 * $Revision: 561 $
 * $Author: sharmnid $
 *
 ************************************************************************/

/************************************************************************
 * FILE DESCR: Definition of HolisticShapeModel which holds the reference model information of a
 *			   particular shape
 *
 * CONTENTS:   
 *
 * AUTHOR:     Mehul Patel
 *
 * DATE:       March 29, 2005
 * CHANGE HISTORY:
 * Author		Date			Description of change
 ************************************************************************/

#ifndef __HOLISTICSHAPEMODEL_H
#define __HOLISTICSHAPEMODEL_H

#include "LTKTypes.h"

using namespace std;

/**
 * @class HolisticShapeModel
 * <p> This class stores the Holistic Features of a shape 
 *		with shape label shapeId and model.<p> 
 */

class HolisticShapeModel
{
private:

	int m_shapeId;				//	shape id of the shape

	float2DVector m_model;		//	corresponding holistic features

public:

	/** @name Constructors and Destructor */
	//@{


	/**
	 * Default Constructor.
	 */

	HolisticShapeModel();

	/**
	* Destructor
	*/

	~HolisticShapeModel();

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

	int getShapeId(void) const;

	/**
	 * This function returns the Eigen Vectors of the model
	 * @param void
	 *
	 * @return Eigen Vectors of the model.
	 */

	const float2DVector& getModel(void) const;

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

	// @}

};

#endif

//#ifndef __HOLISTICSHAPEMODEL_H
//#define __HOLISTICSHAPEMODEL_H
