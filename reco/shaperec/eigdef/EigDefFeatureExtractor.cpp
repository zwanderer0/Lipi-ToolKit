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
 * FILE DESCR: Implementation of EigDef feature extraction module
 *
 * CONTENTS:  
 *	extractFeatures
 *
 * AUTHOR:     Balaji R.
 *
 * DATE:       December 23, 2004
 * CHANGE HISTORY:
 * Author		Date			Description
 ************************************************************************/

#include "LTKTypes.h"

#include "EigDefFeatureExtractor.h"

#include "LTKLoggerUtil.h"

#include "LTKTrace.h"

/**********************************************************************************
* AUTHOR		: Balaji R.
* DATE			: 23-DEC-2004
* NAME			: EigDefFeatureExtractor
* DESCRIPTION	: Default Constructor
* ARGUMENTS		: 
* RETURNS		: 
* NOTES			:
* CHANGE HISTROY
* Author			Date				Description
*************************************************************************************/

EigDefFeatureExtractor::EigDefFeatureExtractor()
{
	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << 
        "Entered default constructor of EigDefFeatureExtractor"  << endl;

	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << 
        "Exiting default constructor of EigDefFeatureExtractor"  << endl;
}

/**********************************************************************************
* AUTHOR		: Balaji R.
* DATE			: 23-DEC-2004
* NAME			: extractFeatures
* DESCRIPTION	: extracts features from the input trace group
* ARGUMENTS		: traceGroup - the trace group from which features are to be extracted
*				  featureVector - extracted features
* RETURNS		: SUCCESS on successful feature extraction
* NOTES			:
* CHANGE HISTROY
* Author			Date				Description
*************************************************************************************/

int EigDefFeatureExtractor::extractFeatures(LTKTraceGroup traceGroup, floatVector& featureVector)
{
	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << 
        "Entered EigDefFeatureExtractor::extractFeatures"  << endl;

	LTKTrace trace;					// a trace of the trace group

	floatVector channel;		//	a channel of a trace

	int channelIndex;				//	variable to loop over all required channels

	int	traceIndex;					//	variable to loop over all traces of the trace group

	int pointIndex;					//	variable to loop over all points of trace

	//	initializing the feature vector

	featureVector.clear();

	featureVector.push_back(1.00);

	//	getting the indices of the required channels

	int requiredChannelIndices[2];

	requiredChannelIndices[0] = traceGroup.getTraceAt(0).getChannelIndex("X");

	requiredChannelIndices[1] = traceGroup.getTraceAt(0).getChannelIndex("Y");

	//	extracting the feature vector

	for(channelIndex = 0 ; channelIndex < 2; ++channelIndex)
	{
		for(traceIndex = 0; traceIndex < traceGroup.getNumTraces(); ++traceIndex)
		{
			trace = traceGroup.getTraceAt(traceIndex);

			channel = trace.getChannelValues(requiredChannelIndices[channelIndex]);
			
			//featureVector.insert(featureVector.end(), channel.begin(), channel.end());
			copy(channel.begin(),channel.end(),inserter(featureVector,featureVector.end()));
			channel.clear();
		}
	}
 
	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << 
        "Exiting EigDefFeatureExtractor::extractFeatures"  << endl;

	return SUCCESS;

}


/**********************************************************************************
* AUTHOR		: Balaji R.
* DATE			: 23-DEC-2004
* NAME			: ~EigDefFeatureExtractor
* DESCRIPTION	: destructor
* ARGUMENTS		: 
* RETURNS		: 
* NOTES			:
* CHANGE HISTROY
* Author			Date				Description
*************************************************************************************/

EigDefFeatureExtractor::~EigDefFeatureExtractor()
{
	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << 
        "Entered destructor of EigDefFeatureExtractor"  << endl;

	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << 
        "Exiting destructor of EigDefFeatureExtractor"  << endl;
}
	
	
	
	
	
	
	

		
