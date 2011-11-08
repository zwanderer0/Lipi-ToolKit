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
 * FILE DESCR: Implementation of HolisticShapeModel which holds the reference model information 
 *			   of a particular shape
 *
 * CONTENTS: 
 *	setShapeId
 *	setModel
 *	getShapeId
 *	getModel
 *
 * AUTHOR:     Mehul Patel
 *
 * DATE:       March 29, 2005
 * CHANGE HISTORY:
 * Author		Date			Description of change
 ************************************************************************/

#include "HolisticShapeModel.h"

#include "LTKLoggerUtil.h"

/**********************************************************************************
* AUTHOR		: Mehul Patel
* DATE			: 29-MAR-2005
* NAME			: HolisticShapeModel
* DESCRIPTION	: Default Constructor
* ARGUMENTS		: 
* RETURNS		: 
* NOTES			:
* CHANGE HISTROY
* Author			Date				Description of change
*************************************************************************************/

HolisticShapeModel::HolisticShapeModel()
{
	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << "Entered default constructor of HolisticShapeModel"  << endl;

	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << "Exiting default constructor of HolisticShapeModel"  << endl;
}

/**********************************************************************************
* AUTHOR		: Mehul Patel
* DATE			: 29-MAR-2005
* NAME			: setShapeId
* DESCRIPTION	: sets the shape id of the shape model
* ARGUMENTS		: shapeId - shape id of the shape model
* RETURNS		: SUCCESS on successful set operation
* NOTES			:
* CHANGE HISTROY
* Author			Date				Description of change
*************************************************************************************/

int HolisticShapeModel::setShapeId(int shapeId)
{
	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << "Entered HolisticShapeModel::setShapeId"  << endl;

	m_shapeId = shapeId;

	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << "m_shapeId = " << m_shapeId  << endl;

	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << "Exiting HolisticShapeModel::setShapeId"  << endl;

	return SUCCESS;
}

/**********************************************************************************
* AUTHOR		: Mehul Patel
* DATE			: 29-MAR-2005
* NAME			: setModel
* DESCRIPTION	: sets the model parameters of the shape model
* ARGUMENTS		: modelParams - model parameters of the shape model
* RETURNS		: SUCCESS on successful set operation
* NOTES			:
* CHANGE HISTROY
* Author			Date				Description of change
*************************************************************************************/

int HolisticShapeModel::setModel(const float2DVector& modelParams)
{
	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << 
        "Entered HolisticShapeModel::setModel"  << endl;

	m_model = modelParams;

	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << 
        "Exiting HolisticShapeModel::setModel"  << endl;

	return SUCCESS;
}

/**********************************************************************************
* AUTHOR		: Mehul Patel
* DATE			: 29-MAR-2005
* NAME			: getShapeId
* DESCRIPTION	: gets the shape id of the shape model
* ARGUMENTS		: 
* RETURNS		: shape id of the shape model
* NOTES			:
* CHANGE HISTROY
* Author			Date				Description of change
*************************************************************************************/

int HolisticShapeModel::getShapeId(void) const
{
	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << 
        "Entered HolisticShapeModel::getShapeId"  << endl;

	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << 
        "Exiting HolisticShapeModel::getShapeId with return value "  << m_shapeId  << endl;

	return m_shapeId;
}

/**********************************************************************************
* AUTHOR		: Mehul Patel
* DATE			: 29-MAR-2005
* NAME			: getModel
* DESCRIPTION	: gets the model parameters of the shape model
* ARGUMENTS		: 
* RETURNS		: reference to the model parameters of the shape model
* NOTES			:
* CHANGE HISTROY
* Author			Date				Description of change
*************************************************************************************/

const float2DVector& HolisticShapeModel::getModel(void) const
{
	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << "Entered HolisticShapeModel::getModel"  << endl;

	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << 
        "Exiting HolisticShapeModel::getModel with reference to the Holistic model"  << endl;

	return m_model;
}

/**********************************************************************************
* AUTHOR		: Mehul Patel
* DATE			: 29-MAR-2005
* NAME			: ~HolisticShapeModel
* DESCRIPTION	: destructor
* ARGUMENTS		: 
* RETURNS		: 
* NOTES			:
* CHANGE HISTROY
* Author			Date				Description of change
*************************************************************************************/

HolisticShapeModel::~HolisticShapeModel()
{
	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << 
        "Entered destructor of HolisticShapeModel"  << endl;

	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << 
        "Exiting destructor of HolisticShapeModel"  << endl;
} 
