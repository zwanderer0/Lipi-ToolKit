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
 * FILE DESCR: Implementation of EigDefShapeModel which holds the reference model information of a
 *			   particular shape
 *
 * CONTENTS: 
 *	setShapeId
 *	setModel
 *	getShapeId
 *	getModel
 *
 * AUTHOR:     Balaji R.
 *
 * DATE:       December 23, 2004
 * CHANGE HISTORY:
 * Author		Date			Description
 ************************************************************************/

#include "EigDefShapeModel.h"

#include "LTKLoggerUtil.h"

/**********************************************************************************
* AUTHOR		: Balaji R.
* DATE			: 23-DEC-2004
* NAME			: EigDefShapeModel
* DESCRIPTION	: Default Constructor
* ARGUMENTS		: 
* RETURNS		: 
* NOTES			:
* CHANGE HISTROY
* Author			Date				Description
*************************************************************************************/

EigDefShapeModel::EigDefShapeModel()
{
	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << 
        "Entered default constructor of EigDefShapeModel"  << endl;

	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << 
        "Exiting default constructor of EigDefShapeModel"  << endl;
}

/**********************************************************************************
* AUTHOR		: Balaji R.
* DATE			: 23-DEC-2004
* NAME			: setShapeId
* DESCRIPTION	: sets the shape id of the shape model
* ARGUMENTS		: shapeId - shape id of the shape model
* RETURNS		: SUCCESS on successful set operation
* NOTES			:
* CHANGE HISTROY
* Author			Date				Description
*************************************************************************************/

int EigDefShapeModel::setShapeId(int shapeId)
{
	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << 
        "Entered EigDefShapeModel::setShapeId"  << endl;

	m_shapeId = shapeId;

	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << 
        "m_shapeId = " << m_shapeId  << endl;

	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << 
        "Exiting EigDefShapeModel::setShapeId"  << endl;

	return SUCCESS;
}

/**********************************************************************************
* AUTHOR		: Balaji R.
* DATE			: 23-DEC-2004
* NAME			: setModel
* DESCRIPTION	: sets the model parameters of the shape model
* ARGUMENTS		: modelParams - model parameters of the shape model
* RETURNS		: SUCCESS on successful set operation
* NOTES			:
* CHANGE HISTROY
* Author			Date				Description
*************************************************************************************/

int EigDefShapeModel::setModel(const float2DVector& modelParams)
{
	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << 
        "Entered EigDefShapeModel::setModel"  << endl;

	m_model = modelParams;

	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << 
        "Exiting EigDefShapeModel::setModel"  << endl;

	return SUCCESS;
}

/**********************************************************************************
* AUTHOR		: Balaji R.
* DATE			: 31-MAR-2005
* NAME			: setEigenValues
* DESCRIPTION	: sets the eigen values of the model
* ARGUMENTS		: eigenValues - eigenValues of the model
* RETURNS		: SUCCESS on successful set operation
* NOTES			:
* CHANGE HISTROY
* Author			Date				Description
*************************************************************************************/

int EigDefShapeModel::setEigenValues(const floatVector& eigenValues)
{
	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << 
        "Entered EigDefShapeModel::setEigenValues"  << endl;

	m_eigenValues = eigenValues;

	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << 
        "Exiting EigDefShapeModel::setEigenValues"  << endl;

	return SUCCESS;
}

/**********************************************************************************/

int EigDefShapeModel::setMean(const floatVector& meanVector)
{
	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << 
        "Entered EigDefShapeModel::setMean"  << endl;

	m_mean = meanVector;

	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << 
        "Exiting EigDefShapeModel::setMean"  << endl;

	return SUCCESS;
}


/**********************************************************************************/

int EigDefShapeModel::setDeformMean(const floatVector& deformMeanVector)
{
	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << 
        "Entered EigDefShapeModel::setMean"  << endl;

	m_deformMean = deformMeanVector;

	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << 
        "Exiting EigDefShapeModel::setMean"  << endl;

	return SUCCESS;
}

/**********************************************************************************
* AUTHOR		: Balaji R.
* DATE			: 23-DEC-2004
* NAME			: getShapeId
* DESCRIPTION	: gets the shape id of the shape model
* ARGUMENTS		: 
* RETURNS		: shape id of the shape model
* NOTES			:
* CHANGE HISTROY
* Author			Date				Description
*************************************************************************************/

int EigDefShapeModel::getShapeId() const
{
	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << 
        "Entered EigDefShapeModel::getShapeId"  << endl;

	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << 
        "Exiting EigDefShapeModel::getShapeId with return value "  << m_shapeId  << endl;

	return m_shapeId;
}

/**********************************************************************************
* AUTHOR		: Balaji R.
* DATE			: 23-DEC-2004
* NAME			: getModel
* DESCRIPTION	: gets the model parameters of the shape model
* ARGUMENTS		: 
* RETURNS		: reference to the model parameters of the shape model
* NOTES			:
* CHANGE HISTROY
* Author			Date				Description
*************************************************************************************/

const float2DVector& EigDefShapeModel::getModel() const
{
	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << 
        "Entered EigDefShapeModel::getModel"  << endl;

	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << 
        "Exiting EigDefShapeModel::getModel with reference to the EigDef model"  << endl;

	return m_model;
}

/**********************************************************************************
* AUTHOR		: Balaji R.
* DATE			: 31-MAR-2005
* NAME			: getEigenValues
* DESCRIPTION	: gets the eigen values
* ARGUMENTS		: 
* RETURNS		: reference to eigen values are returned
* NOTES			:
* CHANGE HISTROY
* Author			Date				Description
*************************************************************************************/

const floatVector& EigDefShapeModel::getEigenValues() const
{
	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << 
        "Entered EigDefShapeModel::getEigenValues"  << endl;

	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << 
        "Exiting EigDefShapeModel::getEigenValues with reference to the EigDef model"  << endl;

	return m_eigenValues;
}


const floatVector& EigDefShapeModel::getMean() const
{
	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << 
        "Entered EigDefShapeModel::getMean"  << endl;

	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << 
        "Exiting EigDefShapeModel::getMean with reference to the EigDef model"  << endl;

	return m_mean;
}


const floatVector& EigDefShapeModel::getDeformMean() const
{
	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << 
        "Entered EigDefShapeModel::getMean"  << endl;

	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << 
        "Exiting EigDefShapeModel::getMean with reference to the EigDef model"  << endl;

	return m_deformMean;
}


/**********************************************************************************
* AUTHOR		: Balaji R.
* DATE			: 23-DEC-2004
* NAME			: ~EigDefShapeModel
* DESCRIPTION	: destructor
* ARGUMENTS		: 
* RETURNS		: 
* NOTES			:
* CHANGE HISTROY
* Author			Date				Description
*************************************************************************************/

EigDefShapeModel::~EigDefShapeModel()
{
	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << 
        "Entered destructor of EigDefShapeModel"  << endl;

	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << 
        "Exiting destructor of EigDefShapeModel"  << endl;
} 

