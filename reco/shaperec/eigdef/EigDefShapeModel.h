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
 * FILE DESCR: Definition of EigDefShapeModel which holds the reference model information of a
 *			   particular shape
 *
 * CONTENTS:   
 *
 * AUTHOR:     Balaji R.
 *
 * DATE:       December 23, 2004
 * CHANGE HISTORY:
 * Author		Date			Description
 ************************************************************************/

#ifndef __EigDefSHAPEMODEL_H
#define __EigDefSHAPEMODEL_H

#include "LTKTypes.h"

using namespace std;

/**
 * @class EigDefShapeModel
 * <p> This class stores the eigen vectors of a shape with shape label shapeId in model.<p> 
 */

class EigDefShapeModel
{
private:

	int m_shapeId;				//	shape id of the shape

	float2DVector m_model;		//	corresponding shape model

	floatVector m_eigenValues;	// eigen values correponding to the chosen eigen vectors + the next greatest eigen value

	floatVector m_mean;			// class mean
	
	//float2DVector m_prototypes;			// prptotypes of each class
		
	floatVector m_deformMean;		// deform mean

public:

	/** @name Constructors and Destructor */
	//@{


	/**
	 * Default Constructor.
	 */

	EigDefShapeModel();

	/**
	* Destructor
	*/

	~EigDefShapeModel();

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
	 * This function returns the mean of the model
	 * @param void
	 *
	 * @return mean of the model.
	 */

	

	const floatVector& getMean() const;

	const floatVector& getDeformMean() const;
	
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
	/**
	 * This function sets the meanVector of the class
	 * 
	 */
	
	
	int setMean(const floatVector& meanVector);

	int setDeformMean(const floatVector& deformMeanVector);
		// @}
};

#endif

//#ifndef __EigDefSHAPEMODEL_H
//#define __EigDefSHAPEMODEL_H
