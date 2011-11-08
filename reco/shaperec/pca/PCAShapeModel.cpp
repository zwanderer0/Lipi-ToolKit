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
 * FILE DESCR: Implementation of PCAShapeModel which holds the reference model information of a
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
 * Author		Date			Description of change
 ************************************************************************/

#include "PCAShapeModel.h"

#include "LTKLoggerUtil.h"

/**********************************************************************************
* AUTHOR		: Balaji R.
* DATE			: 23-DEC-2004
* NAME			: PCAShapeModel
* DESCRIPTION	: Default Constructor
* ARGUMENTS		: 
* RETURNS		: 
* NOTES			:
* CHANGE HISTROY
* Author			Date				Description of change
*************************************************************************************/

PCAShapeModel::PCAShapeModel()
{
	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG)<<   "Entered default constructor of PCAShapeModel" <<endl;

	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG)<<   "Exiting default constructor of PCAShapeModel" <<endl;
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
* Author			Date				Description of change
*************************************************************************************/

int PCAShapeModel::setShapeId(int shapeId)
{
	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG)<<   "Entered PCAShapeModel::setShapeId" <<endl;

	m_shapeId = shapeId;

	LOG(LTKLogger::LTK_LOGLEVEL_DEBUG) <<  "m_shapeId = " << m_shapeId << endl;

	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG)<<   "Exiting PCAShapeModel::setShapeId" <<endl;

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
* Author			Date				Description of change
*************************************************************************************/

int PCAShapeModel::setModel(const float2DVector& modelParams)
{
	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG)<<   "Entered PCAShapeModel::setModel" <<endl;

	m_model = modelParams;

	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG)<<   "Exiting PCAShapeModel::setModel" <<endl;

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
* Author			Date				Description of change
*************************************************************************************/

int PCAShapeModel::setEigenValues(const floatVector& eigenValues)
{
	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG)<<   "Entered PCAShapeModel::setEigenValues" <<endl;

	m_eigenValues = eigenValues;

	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG)<<   "Exiting PCAShapeModel::setEigenValues" <<endl;

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
* Author			Date				Description of change
*************************************************************************************/

int PCAShapeModel::getShapeId() const
{
	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG)<<   "Entered PCAShapeModel::getShapeId" <<endl;

	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG)<<   
		"Exiting PCAShapeModel::getShapeId with return value "  << m_shapeId << endl;

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
* Author			Date				Description of change
*************************************************************************************/

const float2DVector& PCAShapeModel::getModel() const
{
	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG)<<   "Entered PCAShapeModel::getModel" <<endl;

	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG)<<   "Exiting PCAShapeModel::getModel with reference to the pca model" <<endl;

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
* Author			Date				Description of change
*************************************************************************************/

const floatVector& PCAShapeModel::getEigenValues() const
{
	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG)<<   "Entered PCAShapeModel::getEigenValues" <<endl;

	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG)<<   "Exiting PCAShapeModel::getEigenValues with reference to the pca model" <<endl;

	return m_eigenValues;
}

/**********************************************************************************
* AUTHOR		: Balaji R.
* DATE			: 23-DEC-2004
* NAME			: ~PCAShapeModel
* DESCRIPTION	: destructor
* ARGUMENTS		: 
* RETURNS		: 
* NOTES			:
* CHANGE HISTROY
* Author			Date				Description of change
*************************************************************************************/

PCAShapeModel::~PCAShapeModel()
{
} 


