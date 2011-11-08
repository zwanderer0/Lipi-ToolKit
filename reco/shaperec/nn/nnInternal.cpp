/*****************************************************************************************
* Copyright (c) 2006 Hewlett-Packard Development Company, L.P.
* Permission is hereby granted, free of charge, to any person obtaining a copy of 
* this software and associated documentation files (the "Software"), to deal in 
* the Software without restriction, including without limitation the rights to use, 
* copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the 
* Software, and to permit persons to whom the Software is furnished to do so, 
* subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
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
 * $LastChangedDate: 
 * $Revision: 
 * $Author: 
 *
 ************************************************************************/

#pragma once
#include "NNShapeRecognizer.h"
#include "adaptInternal.h"
#include "LTKLoggerUtil.h"
#include "LTKConfigFileReader.h"
#include "LTKPreprocessorInterface.h"
#include "LTKPreprocDefaults.h"
#include "LTKShapeRecoUtil.h"
#include "LTKTraceGroup.h"
#include "LTKErrors.h"
#include "LTKShapeSample.h"
#include "LTKException.h"
#include "LTKErrorsList.h"
#include "LTKStringUtil.h"
#include "LTKShapeRecoUtil.h"
#include "NNShapeRecognizer.h"
#include "LTKDynamicTimeWarping.h"
#include "LTKPreprocDefaults.h"
#include "LTKHierarchicalClustering.h"
#include "LTKPreprocessorInterface.h"
#include "LTKShapeFeatureExtractorFactory.h"
#include "LTKException.h"
#include "LTKShapeFeatureExtractor.h"
#include "LTKShapeFeature.h"
#include "LTKVersionCompatibilityCheck.h"
extern void *m_libHandlerFE;

/**********************************************************************************
* AUTHOR		: Vandana Roy
* DATE			: 10-Dec-2007
* NAME			: 
* DESCRIPTION	: 
* ARGUMENTS		: 
* RETURNS		: Success : If sample was adapted successfully
*				  Failure : Returns Error Code
* NOTES			:
* CHANGE HISTROY
* Author			Date				Description
*************************************************************************************/
int NNShapeRecognizer::readInternalClassifierConfig()
{
	LTKConfigFileReader *shapeRecoProperties = NULL;

	shapeRecoProperties = new LTKConfigFileReader(m_nnCfgFilePath);

	

	//PROTOTYPE_SELECTION_LVQ
	
//	errorCode = shapeRecoProperties->getConfigValue(PROTOTYPEDISTANCE,
          //  tempStringVar);


	delete shapeRecoProperties;

	return errorCode;
}
/**********************************************************************************
* AUTHOR		: Tarun Madan
* DATE			: 30-Aug-2007
* NAME			: adapt
* DESCRIPTION	: adapt recent recognized sample 
* ARGUMENTS		: shapeID : True shapeID of sample
* RETURNS		: Success : If sample was adapted successfully
*				  Failure : Returns Error Code
* NOTES			:
* CHANGE HISTROY
* Author			Date				Description
*************************************************************************************/
int NNShapeRecognizer::adapt(int shapeId)
{
	try{
		LOG(LTKLogger::LTK_LOGLEVEL_INFO) 
				<< "Enter NNShapeRecognizer::adapt()"<<endl;

		//Validate shapeID
		map<int,int>::iterator m_shapeIDNumPrototypesMapIter;
		if(m_shapeIDNumPrototypesMap.find(shapeId) == m_shapeIDNumPrototypesMap.end())
		{
			LTKReturnError(EINVALID_SHAPEID);
		}

		//Adaptation Code
		LTKAdapt* adaptObj = LTKAdapt::getInstance(this);
		
		int nErrorCode;
		nErrorCode = adaptObj->adapt(shapeId);
		if(nErrorCode !=0)
		{
			LOG(LTKLogger::LTK_LOGLEVEL_ERR) 
				<< "Error during NNShapeRecognizer::adapt()"<<endl;
			LTKReturnError(nErrorCode);
		}

		//Clear Variables cached 
		m_neighborInfoVec.clear();
		m_vecRecoResult.clear();

	}
	catch(...)
	{	
		LOG(LTKLogger::LTK_LOGLEVEL_ERR) 
				<< "Error during NNShapeRecognizer::adapt()"<<endl;
		return FAILURE;
	}

	LOG(LTKLogger::LTK_LOGLEVEL_INFO) 
			<< "Exit NNShapeRecognizer::adapt()"<<endl;
	
	return SUCCESS;
}

/**********************************************************************************
* AUTHOR		: Tarun Madan
* DATE			: 30-Aug-2007
* NAME			: adapt
* DESCRIPTION	: adapt sample passed as argument
* ARGUMENTS		: sampleTraceGroup :  TraceGroup of sample to be adapted
*				  shapeID : True shapeID of sample
* RETURNS		: Success : If sample was adapted successfully
*				  Failure : Returns Error Code
* NOTES			:
* CHANGE HISTROY
* Author			Date				Description
*************************************************************************************/
int NNShapeRecognizer::adapt(const LTKTraceGroup& sampleTraceGroup, int shapeId )
{
	LOG(LTKLogger::LTK_LOGLEVEL_INFO) 
			<< "Enter NNShapeRecognizer::Adapt()"<<endl;
	
	vector<int> vecSubSet;
	vector<LTKShapeRecoResult> vecRecoResult;
	LTKScreenContext objScreenContext;
	int nErrorCode;
	nErrorCode = recognize(
					sampleTraceGroup,
					objScreenContext,
					vecSubSet,
					CONF_THRESHOLD_FILTER_OFF,
					NN_DEF_RECO_NUM_CHOICES,
					vecRecoResult
					);

	if(nErrorCode !=0)
	{
		LOG(LTKLogger::LTK_LOGLEVEL_ERR) 
			<< "Error during call to recognize in NNShapeRecognizer::Adapt()"<<endl;
		LTKReturnError(nErrorCode);
	}

	nErrorCode = adapt(shapeId);
	if(nErrorCode !=0)
	{
		LOG(LTKLogger::LTK_LOGLEVEL_ERR) 
			<< "Error during NNShapeRecognizer::Adapt()"<<endl;
		LTKReturnError(nErrorCode);
	}

	LOG(LTKLogger::LTK_LOGLEVEL_INFO) 
			<< "Exit NNShapeRecognizer::Adapt()"<<endl;

	return SUCCESS;
}

/**********************************************************************************
* AUTHOR		: Tarun Madan
* DATE			: 8-Oct-2007
* NAME			: deleteAdaptInstance
* DESCRIPTION	: delete AdaptInstance
* ARGUMENTS		: 
* RETURNS		: None
* NOTES			:
* CHANGE HISTROY
* Author			Date				Description
*************************************************************************************/
int NNShapeRecognizer::deleteAdaptInstance()
{
	//Implemented as deleteAdaptInstance is called by ~NNShapeRecognizer
	//and adaptObj object is not available in NN.cpp

	LTKAdapt *adaptInstance = LTKAdapt::getInstance(this);
	if(adaptInstance)
	{
		adaptInstance->deleteInstance();
	}

	return SUCCESS;
}


