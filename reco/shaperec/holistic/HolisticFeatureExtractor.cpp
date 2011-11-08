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
 * FILE DESCR: Implementation of Holistic feature extraction module
 *
 * CONTENTS:  
 *	extractFeatures
 *
 * AUTHOR:     Mehul Patel
 *
 * DATE:       March 29, 2005
 * CHANGE HISTORY:
 * Author		Date			Description of change
 ************************************************************************/

#include "LTKTypes.h"

#include "HolisticFeatureExtractor.h"

#include "LTKLoggerUtil.h"

#include "LTKTrace.h"

#include "LTKErrors.h"

#include "LTKErrorsList.h"

/**********************************************************************************
* AUTHOR		: Mehul Patel
* DATE			: 29-MAR-2005
* NAME			: HolisticFeatureExtractor
* DESCRIPTION	: Default Constructor
* ARGUMENTS		: 
* RETURNS		: 
* NOTES			:
* CHANGE HISTROY
* Author			Date				Description of change
*************************************************************************************/

HolisticFeatureExtractor::HolisticFeatureExtractor(int numPoints1, int numPoints2, 
		int filterLength1, int filterLength2, float swAngNormFactor, 
		int swAngHookThresh,int horizStrokeAngleThresh)
:m_numResamplePoints1(numPoints1), m_numResamplePoints2(numPoints2), 
m_smoothFilterLength1(filterLength1), m_smoothFilterLength2(filterLength2),
m_swAngNormFactor(swAngNormFactor), m_swAngHookThresh(swAngHookThresh),
m_horizStrokeAngleThresh(horizStrokeAngleThresh)
{
	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << 
        "Entered default constructor of HolisticFeatureExtractor"  << endl;

	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << 
        "Exiting default constructor of HolisticFeatureExtractor"  << endl;
}

/**********************************************************************************
* AUTHOR		: Mehul Patel
* DATE			: 29-MAR-2005
* NAME			: getNumResamplePoints1
* DESCRIPTION	: gets the numPoints to which the traces will be rescaled
* ARGUMENTS		: 
* RETURNS		: m_numResamplePoints1
* NOTES			:
* CHANGE HISTROY
* Author			Date				Description of change
*************************************************************************************/

int HolisticFeatureExtractor::getNumResamplePoints1() const
{
	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << 
        "Entered HolisticFeatureExtractor::getNumResamplePoints1"  << endl;

	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << 
        "Exiting HolisticFeatureExtractor::getNumResamplePoints1 with return value " <<
        m_numResamplePoints1  << endl;

	return m_numResamplePoints1;
}

/**********************************************************************************
* AUTHOR		: Mehul Patel
* DATE			: 29-MAR-2005
* NAME			: getNumResamplePoints2
* DESCRIPTION	: gets the numPoints to which the traces will be rescaled
* ARGUMENTS		: 
* RETURNS		: m_numResamplePoints2
* NOTES			:
* CHANGE HISTROY
* Author			Date				Description of change
*************************************************************************************/

int HolisticFeatureExtractor::getNumResamplePoints2() const
{
	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << 
        "Entered HolisticFeatureExtractor::getNumResamplePoints2"  << endl;

	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << 
        "Exiting HolisticFeatureExtractor::getNumResamplePoints2 with return value " <<
        m_numResamplePoints2  << endl;

	return m_numResamplePoints2;
}

/**********************************************************************************
* AUTHOR		: Mehul Patel
* DATE			: 29-MAR-2005
* NAME			: getSmoothFilterLength1
* DESCRIPTION	: gets the filterLength used to smoothen the traces
* ARGUMENTS		: 
* RETURNS		: m_smoothFilterLength1
* NOTES			:
* CHANGE HISTROY
* Author			Date				Description of change
*************************************************************************************/

int HolisticFeatureExtractor::getSmoothFilterLength1() const
{
	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << 
        "Entered HolisticFeatureExtractor::getSmoothFilterLength1"  << endl;

	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << 
        "Exiting HolisticFeatureExtractor::getSmoothFilterLength1 with return value " <<
        m_smoothFilterLength1  << endl;

	return m_smoothFilterLength1;
}

/**********************************************************************************
* AUTHOR		: Mehul Patel
* DATE			: 29-MAR-2005
* NAME			: getSmoothFilterLength2
* DESCRIPTION	: gets the filterLength used to smoothen the traces
* ARGUMENTS		: 
* RETURNS		: m_smoothFilterLength2
* NOTES			:
* CHANGE HISTROY
* Author			Date				Description of change
*************************************************************************************/

int HolisticFeatureExtractor::getSmoothFilterLength2() const
{
	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << 
        "Entered HolisticFeatureExtractor::getSmoothFilterLength2"  << endl;

	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << 
        "Exiting HolisticFeatureExtractor::getSmoothFilterLength2 with return value " <<
        m_smoothFilterLength2  << endl;

	return m_smoothFilterLength2;
}

/**********************************************************************************
* AUTHOR		: Mehul Patel
* DATE			: 29-MAR-2005
* NAME			: getSwAngNormFactor
* DESCRIPTION	: gets the swept angle normalization factor
* ARGUMENTS		: 
* RETURNS		: m_swAngNormFactor
* NOTES			:
* CHANGE HISTROY
* Author			Date				Description of change
*************************************************************************************/

int HolisticFeatureExtractor::getSwAngNormFactor() const
{
	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << 
        "Entered HolisticFeatureExtractor::getSwAngNormFactor"  << endl;

	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << 
        "Exiting HolisticFeatureExtractor::getSwAngNormFactor with return value " 
        << m_swAngNormFactor << endl;

	return m_swAngNormFactor;
}

/**********************************************************************************
* AUTHOR		: Mehul Patel
* DATE			: 29-MAR-2005
* NAME			: getSwAngHookThresh
* DESCRIPTION	: gets the swept angle hook threshold
* ARGUMENTS		: 
* RETURNS		: m_swAngHookThresh
* NOTES			:
* CHANGE HISTROY
* Author			Date				Description of change
*************************************************************************************/

int HolisticFeatureExtractor::getSwAngHookThresh() const
{
	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << 
        "Entered HolisticFeatureExtractor::getSwAngHookThresh"  << endl;

	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << 
        "Exiting HolisticFeatureExtractor::getSwAngHookThresh with return value " <<
        m_swAngHookThresh  << endl;

	return m_swAngHookThresh;
}


/**********************************************************************************
* AUTHOR		: Mehul Patel
* DATE			: 29-MAR-2005
* NAME			: getHorizStrokeAngleThresh
* DESCRIPTION	: gets the horizontal stroke angle threshold
* ARGUMENTS		: 
* RETURNS		: m_horizStrokeAngleThresh
* NOTES			:
* CHANGE HISTROY
* Author			Date				Description of change
*************************************************************************************/

int HolisticFeatureExtractor::getHorizStrokeAngleThresh() const
{
	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << 
        "Entered HolisticFeatureExtractor::getHorizStrokeAngleThresh"  << endl;

	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << 
        "Exiting HolisticFeatureExtractor::getHorizStrokeAngleThresh with return value " <<
        m_horizStrokeAngleThresh  << endl;

	return m_horizStrokeAngleThresh;
}

/**********************************************************************************
* AUTHOR		: Mehul Patel
* DATE			: 29-MAR-2005
* NAME			: setNumResamplePoints1
* DESCRIPTION	: sets the number of resample points
* ARGUMENTS		: 
* RETURNS		: SUCCESS on successful set operation
* NOTES			:
* CHANGE HISTROY
* Author			Date				Description of change
*************************************************************************************/

int HolisticFeatureExtractor::setNumResamplePoints1(int numPoints)
{
	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << 
        "Entered HolisticFeatureExtractor::setNumResamplePoints1"  << endl;

	this->m_numResamplePoints1 = numPoints;

	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << 
        "m_numResamplePoints1 = " << m_numResamplePoints1  << endl;

	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << 
        "Exiting HolisticFeatureExtractor::setNumResamplePoints1"  << endl;

	return SUCCESS;
}

/**********************************************************************************
* AUTHOR		: Mehul Patel
* DATE			: 29-MAR-2005
* NAME			: setNumResamplePoints2
* DESCRIPTION	: sets the number of resample points
* ARGUMENTS		: 
* RETURNS		: SUCCESS on successful set operation
* NOTES			:
* CHANGE HISTROY
* Author			Date				Description of change
*************************************************************************************/

int HolisticFeatureExtractor::setNumResamplePoints2(int numPoints)
{
	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << 
        "Entered HolisticFeatureExtractor::setNumResamplePoints2"  << endl;

	this->m_numResamplePoints2 = numPoints;

	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << 
        "m_numResamplePoints2 = " << m_numResamplePoints2  << endl;

	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << 
        "Exiting HolisticFeatureExtractor::setNumResamplePoints2"  << endl;

	return SUCCESS;
}

/**********************************************************************************
* AUTHOR		: Mehul Patel
* DATE			: 29-MAR-2005
* NAME			: setSmoothFilterLength1
* DESCRIPTION	: sets the filterLength used to smoothen the traces
* ARGUMENTS		: 
* RETURNS		: SUCCESS on successful set operation
* NOTES			:
* CHANGE HISTROY
* Author			Date				Description of change
*************************************************************************************/

int HolisticFeatureExtractor::setSmoothFilterLength1(int filterLength)
{
	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << 
        "Entered HolisticFeatureExtractor::setSmoothFilterLength1"  << endl;

	this->m_smoothFilterLength1 = filterLength;

	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << 
        "m_smoothFilterLength1 = " << m_smoothFilterLength1  << endl;

	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << 
        "Exiting HolisticFeatureExtractor::setSmoothFilterLength1"  << endl;

	return SUCCESS;
}

/**********************************************************************************
* AUTHOR		: Mehul Patel
* DATE			: 29-MAR-2005
* NAME			: setSmoothFilterLength2
* DESCRIPTION	: sets the filterLength used to smoothen the traces
* ARGUMENTS		: 
* RETURNS		: SUCCESS on successful set operation
* NOTES			:
* CHANGE HISTROY
* Author			Date				Description of change
*************************************************************************************/

int HolisticFeatureExtractor::setSmoothFilterLength2(int filterLength)
{
	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << 
        "Entered HolisticFeatureExtractor::setSmoothFilterLength2"  << endl;

	this->m_smoothFilterLength2 = filterLength;

	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << 
        "m_smoothFilterLength2 = " + m_smoothFilterLength2  << endl;

	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << 
        "Exiting HolisticFeatureExtractor::setSmoothFilterLength2"  << endl;

	return SUCCESS;
}

/**********************************************************************************
* AUTHOR		: Mehul Patel
* DATE			: 29-MAR-2005
* NAME			: setSwAngNormFactor
* DESCRIPTION	: sets the swept angle normalization factor
* ARGUMENTS		: swAngNormFactor - float
* RETURNS		: SUCCESS on successful set operation
* NOTES			:
* CHANGE HISTROY
* Author			Date				Description of change
*************************************************************************************/

int HolisticFeatureExtractor::setSwAngNormFactor(float swAngNormFactor)
{

	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << "Entered HolisticFeatureExtractor::setSwAngNormFactor"  << endl;

	this->m_swAngNormFactor = swAngNormFactor;

	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) <<
        "m_swAngNormFactor = " << m_swAngNormFactor << endl;

	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << 
        "Exiting HolisticFeatureExtractor::setSwAngNormFactor"  << endl;

	return SUCCESS;
}

/**********************************************************************************
* AUTHOR		: Mehul Patel
* DATE			: 29-MAR-2005
* NAME			: setSwAngHookThresh
* DESCRIPTION	: sets the swept angle normalization factor
* ARGUMENTS		: swAngNormFactor - float
* RETURNS		: SUCCESS on successful set operation
* NOTES			:
* CHANGE HISTROY
* Author			Date				Description of change
*************************************************************************************/

int HolisticFeatureExtractor::setSwAngHookThresh(int swAngHookThresh)
{
	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << 
        "Entered HolisticFeatureExtractor::setSwAngHookThresh"  << endl;

	this->m_swAngHookThresh = swAngHookThresh;

	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << 
        "m_swAngHookThresh = " << m_swAngHookThresh  << endl;

	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << 
        "Exiting HolisticFeatureExtractor::setSwAngHookThresh"  << endl;

	return SUCCESS;
}

/**********************************************************************************
* AUTHOR		: Mehul Patel
* DATE			: 29-MAR-2005
* NAME			: setSwAngHookThresh
* DESCRIPTION	: sets the swept angle normalization factor
* ARGUMENTS		: swAngNormFactor - float
* RETURNS		: SUCCESS on successful set operation
* NOTES			:
* CHANGE HISTROY
* Author			Date				Description of change
*************************************************************************************/

int HolisticFeatureExtractor::setHorizStrokeAngleThresh(int horizStrokeAngleThresh)
{
	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << 
        "Entered HolisticFeatureExtractor::setHorizStrokeAngleThresh"  << endl;

	this->m_horizStrokeAngleThresh = horizStrokeAngleThresh;

	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << 
        "m_horizStrokeAngleThresh = " << m_horizStrokeAngleThresh  << endl;

	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << 
        "Exiting HolisticFeatureExtractor::setHorizStrokeAngleThresh"  << endl;

	return SUCCESS;
}


/**********************************************************************************
* AUTHOR		: Mehul Patel
* DATE			: 29-MAR-2005
* NAME			: extractFeatures
* DESCRIPTION	: extracts features from the input trace group
* ARGUMENTS		: traceGroup - the trace group from which features are to be extracted
*				  featureVector - extracted features
* RETURNS		: SUCCESS on successful feature extraction
* NOTES			:
* CHANGE HISTROY
* Author			Date				Description of change
*************************************************************************************/

int HolisticFeatureExtractor::extractFeatures(const LTKTraceGroup& traceGroup, const LTKCaptureDevice& captureDevice, const LTKScreenContext& screenContext, LTKPreprocessorInterface *ltkShapeRecPtr, float2DVector& featureVector)
{
	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << 
        "Entered HolisticFeatureExtractor::extractFeatures"  << endl;

	LTKTrace preprocessedTrace;				// a trace of the trace group

	LTKTrace preprocessedTrace2;			// a trace of the trace group

	LTKTrace preprocessedTrace3;			// a trace of the trace group

	LTKTraceGroup preprocessedTraceGroup;

	LTKTraceGroup preprocessedTraceGroup2;

	LTKTraceGroup preprocessedTraceGroup3;

	int	traceIndex;				//	variable to loop over all traces of the trace group


	// preprocessing the traceGroup in 3 ways to extract 3 kinds of features
		
	preprocess(traceGroup, preprocessedTraceGroup, captureDevice, screenContext, ltkShapeRecPtr);

	preprocess2(traceGroup, preprocessedTraceGroup2, captureDevice, screenContext, ltkShapeRecPtr);

	preprocess3(traceGroup, preprocessedTraceGroup3, captureDevice, screenContext, ltkShapeRecPtr);

	//	extracting the feature vector

	for(traceIndex = 0; traceIndex < traceGroup.getNumTraces(); ++traceIndex)
	{
		preprocessedTrace = preprocessedTraceGroup.getTraceAt(traceIndex);

		preprocessedTrace2 = preprocessedTraceGroup2.getTraceAt(traceIndex);

		preprocessedTrace3 = preprocessedTraceGroup3.getTraceAt(traceIndex);

		// calling the compute features methods 

		floatVector features;

		// calculating features with preprocessedTrace2
		
		features.push_back(computeEER(preprocessedTrace));

		features.push_back(computeOrientation(preprocessedTrace));

		// calculating features with preprocessedTrace2

		float TCL = computeTCL(preprocessedTrace2);

		TCL /= calculateBBoxPerimeter(screenContext);	// normalizing using the perimeter
		
		features.push_back(TCL);

		features.push_back(computePII(preprocessedTrace2, screenContext));

		// calculating features with preprocessedTrace3

		float swAng = computeSweptAngle(preprocessedTrace3);

		// normalizing the swept angle with swAngNormFactor x 360 degrees
		swAng /= (2*PI*m_swAngNormFactor);

		features.push_back(swAng);

		featureVector.push_back(features);

	}//traceIndex

	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << 
        "Exiting HolisticFeatureExtractor::extractFeatures"  << endl;

	return SUCCESS;

}


/**********************************************************************************
* AUTHOR		: Mehul Patel
* DATE			: 29-MAR-2005
* NAME			: computeEER
* DESCRIPTION	: Computes the End-to-end Ratio
* ARGUMENTS		: inTrace - the trace for which EER is to be calculated
* RETURNS		: EER - End-to-end ratio
* NOTES			:
* CHANGE HISTROY
* Author			Date				Description of change
*************************************************************************************/

float HolisticFeatureExtractor::computeEER(const LTKTrace& inTrace)
{
	int numOfPoints;				// total number of points in the trace

	float EEL = 0.0f;				// End-to-end Length

	float TCL = 0.0f;				// Total Curve Length

	float EER = 0.0f;				// End-to-end Ratio
	
	floatVector xData, yData;		// coordinates of the points in the trace
	
	xData = inTrace.getChannelValues("X");
	yData = inTrace.getChannelValues("Y");

	numOfPoints = xData.size();

	EEL = calculateEuclidDist(xData.at(0),yData.at(0),xData.at(numOfPoints-1),yData.at(numOfPoints-1));

	TCL = computeTCL(inTrace);

	EER = EEL/TCL;

	return EER;		
}

/**********************************************************************************
* AUTHOR		: Mehul Patel
* DATE			: 29-MAR-2005
* NAME			: computeOrientation
* DESCRIPTION	: Computes the Orientation for a trace
* ARGUMENTS		: inTrace - the trace for which Orientation is to be calculated
* RETURNS		: orient - Orientation
* NOTES			:
* CHANGE HISTROY
* Author			Date				Description of change
*************************************************************************************/

float HolisticFeatureExtractor::computeOrientation(const LTKTrace& inTrace)
{
	int numOfPoints;				// total number of points in the trace

	float orient = 0.0f;			// orientation
	
	floatVector xData, yData;		// coordinates of the points in the trace
	
	xData = inTrace.getChannelValues("X");
	yData = inTrace.getChannelValues("Y");

	numOfPoints = xData.size();

	orient = calculateSlope(xData.at(0),yData.at(0),xData.at(numOfPoints-1),yData.at(numOfPoints-1));

	// convert all angles to a range 0 to pi
	if (orient<0)
	{
		orient += PI;
	}

	// normalizing it from 0 to 1
	orient /= PI;	

	// make horizontal stroke as 180 degrees instead of 2 values
	if (orient<(m_horizStrokeAngleThresh/180))
	{
		orient = 1 - orient;
	}

	return orient;		
}

/**********************************************************************************
* AUTHOR		: Mehul Patel
* DATE			: 29-MAR-2005
* NAME			: computeTCL
* DESCRIPTION	: Computes the Total Curve Length
* ARGUMENTS		: inTrace - the trace for which TCL is to be calculated
* RETURNS		: TCL - Total Curve Length
* NOTES			:
* CHANGE HISTROY
* Author			Date				Description of change
*************************************************************************************/

float HolisticFeatureExtractor::computeTCL(const LTKTrace& inTrace)
{
	float TCL=0.0f;				// Total Curve Length
	
	int pointIndex;				// Index to loop over all the points 

	floatVector xData, yData;	// coordinates of the points in the trace
	
	xData = inTrace.getChannelValues("X");
	yData = inTrace.getChannelValues("Y");

	for(pointIndex = 0; pointIndex < xData.size()-1; ++pointIndex)
	{
		TCL += calculateEuclidDist(xData.at(pointIndex),yData.at(pointIndex),xData.at(pointIndex+1),yData.at(pointIndex+1));
	}

	return TCL;	
}

/**********************************************************************************
* AUTHOR		: Mehul Patel
* DATE			: 29-MAR-2005
* NAME			: computePII
* DESCRIPTION	: Computes the Position Information Index
* ARGUMENTS		: inTrace - the trace for which TCL is to be calculated
* RETURNS		: PII - Position Information Index
* NOTES			:
* CHANGE HISTROY
* Author			Date				Description of change
*************************************************************************************/
float HolisticFeatureExtractor::computePII(const LTKTrace& inTrace, const LTKScreenContext screenContext)
{
	int pointIndex;				// Index to loop over all the points 

	int numOfPoints;			// total number of points in the trace

	float PII=0.0f;				// Position Information Index
	
	float bboxTop = screenContext.getBboxTop();

	float bboxBottom = screenContext.getBboxBottom();

	float vThresh = (bboxTop + bboxBottom)/2;			// vertical threshold

	floatVector yData;						// y-coordinates of the points in the trace
	
	yData = inTrace.getChannelValues("Y");

	numOfPoints = yData.size();

	for(pointIndex = 0; pointIndex < numOfPoints; ++pointIndex)
	{
		if(yData.at(pointIndex)>vThresh)
		{
			++PII;
		}
	}

	PII /= numOfPoints;

	return PII;
}

/**********************************************************************************
* AUTHOR		: Mehul Patel
* DATE			: 29-MAR-2005
* NAME			: computeSweptAngle
* DESCRIPTION	: Computes the Swept Angle of the given trace
* ARGUMENTS		: inTrace - the trace for which TCL is to be calculated
* RETURNS		: swAng - swept angle
* NOTES			:
* CHANGE HISTROY
* Author			Date				Description of change
*************************************************************************************/

float HolisticFeatureExtractor::computeSweptAngle(const LTKTrace& inTrace)
{
	float swAng=0.0f;			// Swept Angle
	
	int numOfPoints;			// total number of points in the trace
	
	int pointIndex;				// Index to loop over all the points 

	floatVector angles;			// angles made by each line with x-axis

	floatVector angleDiff;		// difference of angles between consecutive lines

	floatVector xData, yData;	// coordinates of the points in the trace
	
	xData = inTrace.getChannelValues("X");
	yData = inTrace.getChannelValues("Y");

	numOfPoints = xData.size();

	for(pointIndex = 0; pointIndex < numOfPoints-1; ++pointIndex)
	{
		float angle;
		
		angle = calculateSlope(xData.at(pointIndex),yData.at(pointIndex),xData.at(pointIndex+1),yData.at(pointIndex+1));
        		
		// converting the angles from 0 to 360 degrees
		if (angle < 0)
		{
			angle += 2*PI;
		}
		
		angles.push_back(angle);
	}

	for(pointIndex = 0; pointIndex < numOfPoints-2; ++pointIndex)
	{
		int sign_angle;

		angleDiff.push_back(angles.at(pointIndex)-angles.at(pointIndex+1));

		sign_angle = (angleDiff.at(pointIndex) < 0) ? -1:1;

		// if greater than 180 then correcting it to a value less than 180

		if (fabs(angleDiff.at(pointIndex)) > PI)
		{
			angleDiff.at(pointIndex) = -sign_angle * (2*PI - fabs(angleDiff.at(pointIndex)));
		}

		// if greater than swAngHookThresh then assuming it to be a hook 
		// and disregarding the angle
		
		if (fabs(angleDiff.at(pointIndex)) > (m_swAngHookThresh*PI/180))
		{
			angleDiff.at(pointIndex) = 0;
		}

		swAng += angleDiff.at(pointIndex);
	}

	// returning the absolute value of swept angle
	
	swAng = fabs(swAng);

	return swAng;	
}

/**********************************************************************************
* AUTHOR		: Mehul Patel
* DATE			: 29-MAR-2005
* NAME			: calculateBBoxPerimeter
* DESCRIPTION	: calculates the perimeter of the bounding box
* ARGUMENTS		: screenContext - consonant box of the trace
* RETURNS		: perimeter of the bounding box
* NOTES			:
* CHANGE HISTROY
* Author			Date				Description of change
*************************************************************************************/
float HolisticFeatureExtractor::calculateBBoxPerimeter(LTKScreenContext screenContext)
{
	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << 
        "Entered HolisticFeatureExtractor::calculateBBoxPerimeter"  << endl;
	
	float bboxHeight = screenContext.getBboxTop() - screenContext.getBboxBottom();

	float bboxWidth = screenContext.getBboxRight() - screenContext.getBboxLeft();

	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << 
        "Exiting HolisticFeatureExtractor::calculateBBoxPerimeter"  << endl;
	
	return (2*(bboxHeight + bboxWidth));
}

/**********************************************************************************
* AUTHOR		: Mehul Patel
* DATE			: 29-MAR-2005
* NAME			: calculateSlope
* DESCRIPTION	: calculates the slope of the line joining two 2-d points
* ARGUMENTS		: x1 - x co-ordinate of point 1
*				  y1 - y co-ordinate of point 1
*				  x2 - x co-ordinate of point 2
*				  y2 - y co-ordinate of point 2	
* RETURNS		: slope of the line joining the two 2-d points
* NOTES			:
* CHANGE HISTROY
* Author			Date				Description of change
*************************************************************************************/

float HolisticFeatureExtractor::calculateSlope(float x1,float y1,float x2,float y2)
{
	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << 
        "Entered HolisticFeatureExtractor::calculateSlope"  << endl;

	float slope=0.0f;

	slope = atan2(y2-y1,x2-x1);		// atan2 returns angles in the range -pi to pi

	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << 
        "Exiting HolisticFeatureExtractor::calculateSlope"  << endl;

	return slope;
}

/**********************************************************************************
* AUTHOR		: Mehul Patel
* DATE			: 29-MAR-2005
* NAME			: calculateEuclidDist
* DESCRIPTION	: calculates the euclidean distance between two 2-d points
* ARGUMENTS		: x1 - x co-ordinate of point 1
*				  x2 - x co-ordinate of point 2
*				  y1 - y co-ordinate of point 1
*				  y2 - y co-ordinate of point 2	
* RETURNS		: euclidean distance between the two 2-d points
* NOTES			:
* CHANGE HISTROY
* Author			Date				Description of change
*************************************************************************************/

float HolisticFeatureExtractor::calculateEuclidDist(float x1,float y1,float x2,float y2)
{
	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << 
        "Entered HolisticFeatureExtractor::calculateEuclidDist"  << endl;

	float euclidDistance;

	euclidDistance = sqrt(((x2 - x1) * (x2 - x1)) + ((y2 - y1) * (y2 - y1)));

	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << 
        "Exiting HolisticFeatureExtractor::calculateEuclidDist"  << endl;

	return euclidDistance;
}

/**********************************************************************************
* AUTHOR		: Mehul Patel
* DATE			: 29-MAR-2005
* NAME			: preprocess
* DESCRIPTION	: preprocess the input trace (size normalization is carried out)
* ARGUMENTS		: inTraceGroup - the tracegroup which has to be preprocessed
*				  outTraceGroup - output preprocessed tracegroup
* RETURNS		: SUCCESS on successful preprocessing
* NOTES			:
* CHANGE HISTROY
* Author			Date				Description of change
*************************************************************************************/
int HolisticFeatureExtractor::preprocess(const LTKTraceGroup& inTraceGroup, LTKTraceGroup& outTraceGroup, const LTKCaptureDevice& captureDevice, const LTKScreenContext& screenContext, LTKPreprocessorInterface *ltkShapeRecPtr)
{
	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << 
        "Entered HolisticFeatureExtractor::preprocess"  << endl;
	
	LTKTraceGroup thinnedTraceGroup;

    LTKTraceGroup dehookedTraceGroup;
	
	LTKTraceGroup smoothenedTraceGroup;

	LTKTraceGroup normalizedTraceGroup;

	LTKTraceGroup resampledTraceGroup;

	// dehooking

	ltkShapeRecPtr->removeDuplicatePoints(inTraceGroup, thinnedTraceGroup);
    
    ltkShapeRecPtr->dehookTraces(thinnedTraceGroup, dehookedTraceGroup);

	//dehookedTraceGroup = inTraceGroup;
	ltkShapeRecPtr->setFilterLength(m_smoothFilterLength1);
	ltkShapeRecPtr->smoothenTraceGroup(dehookedTraceGroup, smoothenedTraceGroup);

	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << "TraceGroup Smoothened"  << endl;

	// size normalization
	
	ltkShapeRecPtr->normalizeSize(smoothenedTraceGroup, normalizedTraceGroup);
	
	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << "Size normalized"  << endl;

	resampleTraceGroup(normalizedTraceGroup, m_numResamplePoints1, resampledTraceGroup, ltkShapeRecPtr ); 

    LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << "Traces resampled"  << endl;

    //  second level size normalization of the resampled trace group

    ltkShapeRecPtr->normalizeSize(resampledTraceGroup, outTraceGroup);

    //LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << "Size normalized again"  << endl;


	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << "Exiting HolisticFeatureExtractor::preprocess"  << endl;

	return SUCCESS;
	
}

/**********************************************************************************
* AUTHOR		: Mehul Patel
* DATE			: 29-MAR-2005
* NAME			: preprocess2
* DESCRIPTION	: preprocess the input trace without size normalization
* ARGUMENTS		: inTraceGroup - the tracegroup which has to be preprocessed
*				  outTraceGroup - output preprocessed tracegroup 
* RETURNS		: SUCCESS on successful preprocessing
* NOTES			:
* CHANGE HISTROY
* Author			Date				Description of change
*************************************************************************************/

int HolisticFeatureExtractor::preprocess2(const LTKTraceGroup& inTraceGroup, LTKTraceGroup& outTraceGroup, const LTKCaptureDevice& captureDevice, const LTKScreenContext& screenContext, LTKPreprocessorInterface *ltkShapeRecPtr)
{

	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << 
        "Entered HolisticFeatureExtractor::preprocess2"  << endl;
	
	LTKTraceGroup thinnedTraceGroup;

    LTKTraceGroup dehookedTraceGroup;
	
	LTKTraceGroup smoothenedTraceGroup;

	// dehooking

	ltkShapeRecPtr->removeDuplicatePoints(inTraceGroup, thinnedTraceGroup);
    
    ltkShapeRecPtr->dehookTraces(thinnedTraceGroup, dehookedTraceGroup);

	//dehookedTraceGroup = inTraceGroup;
	ltkShapeRecPtr->setFilterLength(m_smoothFilterLength1);
	ltkShapeRecPtr->smoothenTraceGroup(dehookedTraceGroup, smoothenedTraceGroup);

	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << 
        "TraceGroup Smoothened"  << endl;

	// resampling

	resampleTraceGroup(smoothenedTraceGroup, m_numResamplePoints1, outTraceGroup, ltkShapeRecPtr); 

    LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << 
        "Traces resampled"  << endl;

    LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << 
        "Exiting HolisticFeatureExtractor::preprocess2"  << endl;

	return SUCCESS;
	
}

/**********************************************************************************
* AUTHOR		: Mehul Patel
* DATE			: 25-APR-2005
* NAME			: preprocess3
* DESCRIPTION	: preprocess the input trace without size normalization
* ARGUMENTS		: inTraceGroup - the tracegroup which has to be preprocessed
*				  outTraceGroup - output preprocessed tracegroup 
* RETURNS		: SUCCESS on successful preprocessing
* NOTES			:
* CHANGE HISTROY
* Author			Date				Description of change
*************************************************************************************/

int HolisticFeatureExtractor::preprocess3(const LTKTraceGroup& inTraceGroup, LTKTraceGroup& outTraceGroup, const LTKCaptureDevice& captureDevice, const LTKScreenContext& screenContext, LTKPreprocessorInterface *ltkShapeRecPtr)
{

	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << 
        "Entered HolisticFeatureExtractor::preprocess3"  << endl;
	
	LTKTraceGroup thinnedTraceGroup;

    LTKTraceGroup dehookedTraceGroup;
	
	LTKTraceGroup smoothenedTraceGroup;

	// dehooking

	ltkShapeRecPtr->removeDuplicatePoints(inTraceGroup, thinnedTraceGroup);
    
    ltkShapeRecPtr->dehookTraces(thinnedTraceGroup, dehookedTraceGroup);

	//dehookedTraceGroup = inTraceGroup;
	ltkShapeRecPtr->setFilterLength(m_smoothFilterLength2);
	ltkShapeRecPtr->smoothenTraceGroup(dehookedTraceGroup, smoothenedTraceGroup);

	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << 
        "TraceGroup Smoothened"  << endl;

	// resampling

	resampleTraceGroup(smoothenedTraceGroup, m_numResamplePoints2, outTraceGroup, ltkShapeRecPtr); 

    LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << 
        "Traces resampled"  << endl;

    LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << 
        "Exiting HolisticFeatureExtractor::preprocess3"  << endl;

	return SUCCESS;
	
}


/**********************************************************************************
* AUTHOR        : Mehul Patel
* DATE          : 05-APR-2005
* NAME          : resampleTraceGroup
* DESCRIPTION   : calls the resampleTrace function on each of the trace
*                 and resamples all the traces uniformly to numPoints
* ARGUMENTS     : inTraceGroup - trace group to be resampled
*				  numPoints - number of points to which the tracegroup is resampled
				  outTraceGroup - resampled inTraceGroup
* RETURNS       : SUCCESS on successful resampling operation
* NOTES         :
* CHANGE HISTROY
* Author            Date                Description of change
*************************************************************************************/

int HolisticFeatureExtractor::resampleTraceGroup(LTKTraceGroup& inTraceGroup, int numPoints, LTKTraceGroup& outTraceGroup, LTKPreprocessorInterface *ltkShapeRecPtr)
{
    LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << 
        "Entered HolisticFeatureExtractor::resampleTraceGroup"  << endl;

    //LTKPreprocessor preProcessorObj;
	
	int traceIndex;                     //  variable to loop over the traces of the trace group     

    int numberOfTraces;                 //  number of traces in the trace group

    //  initializing the variables

    numberOfTraces = inTraceGroup.getNumTraces();

	for(traceIndex = 0; traceIndex < numberOfTraces; ++traceIndex)
    {
        LTKTrace newTrace;

        const LTKTrace& trace = inTraceGroup.getTraceAt(traceIndex);

		// resampling all the traces of the traceGroup to numPoints

        //ltkShapeRecPtr->resampleTrace(trace, numPoints, newTrace);
		resampleTrace(trace, numPoints, newTrace);
            
        //outTraceGroup.addTrace(newTrace);
		outTraceGroup.m_traceVector.push_back(newTrace);
	   // LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << "Number of points after resampling trace " + traceIndex + " is " + resamplePointsVec[traceIndex]  << endl;

    }

    LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << 
        "Exiting HolisticFeatureExtractor::resampleTraceGroup"  << endl;

    return SUCCESS;
}

/**********************************************************************************
* AUTHOR		: Balaji R.
* DATE			: 23-DEC-2004
* NAME			: resampleTrace
* DESCRIPTION	: spatially resamples the given trace to a given number of points
* ARGUMENTS		: inTrace - trace to be resampled
*				  resamplePoints - number of points required after resampling
*				  outTrace - resampled trace
* RETURNS		: SUCCESS on successful resample operation
* NOTES			:
* CHANGE HISTROY
* Author			Date				Description of change
*************************************************************************************/

int HolisticFeatureExtractor::resampleTrace(const LTKTrace& inTrace,int resamplePoints, LTKTrace& outTrace)
{
	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << 
        "Entered PCAShapeRecognizer::resampleTrace"  << endl;


    int pointIndex;							//	a variable to loop over the points of a trace				

	int currentPointIndex =0;				//	index of the current point

	float xSum = 0.0f;								//	sum of the x coordinate values 
	
	float ySum = 0.0f;								//	sum of the y coordinate values

	int numTracePoints;						//	number of points in a trace

	int ptIndex = 0;						//	index of point in a trace

	float x;								//	an x coord value
	
	float y;								//	an y coord value
	
	float xDiff;							//	differenc in x direction
	
	float yDiff;							//	diference in y direction
	
	float xTemp;							//	temp x storage 
	
	float yTemp;							//	temp y storage

	float unitLength = 0;					//	estimated length of each segment after resampling
	
	float pointDistance;					
	
	float balanceDistance = 0;				//	distance between the last resampled point and 
											//	the next raw sample point
	
	float measuredDistance;					
	
	float m1,m2;

	floatVector xVec;
	
	floatVector resampledXVec;

	floatVector yVec;
	
	floatVector resampledYVec;

	floatVector distanceVec;
	
	numTracePoints = inTrace.getNumberOfPoints();

	if(numTracePoints == 0)
	{
		LOG( LTKLogger::LTK_LOGLEVEL_ERR) << string(getError(EEMPTY_TRACE)) << endl;
		LTKReturnError(EEMPTY_TRACE);		
	}

	//xVec = inTrace.getChannelValues(xChannelstr);
	xVec = inTrace.getChannelValues("X");
	//yVec = inTrace.getChannelValues(yChannelstr);
	yVec = inTrace.getChannelValues("Y");

	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << 
        "resamplePoints = " , resamplePoints << endl;


	

	if(resamplePoints < 1)
	{
		resamplePoints = 1;
	}

	if(resamplePoints == 1)
	{
    	xSum=accumulate(xVec.begin(),xVec.end(),0.0f); 
		ySum=accumulate(yVec.begin(),yVec.end(),0.0f); 

		x = xSum/ numTracePoints; //As only 1 point is required, this ratio is computed.

		y = ySum/ numTracePoints;

		LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << "x mean = " << x << endl;

		LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << "y mean = " << y << endl;

		resampledXVec.push_back(x);

		resampledYVec.push_back(y);

		outTrace.addChannel(resampledXVec, xChannelstr);
		outTrace.addChannel(resampledYVec, yChannelstr);
		
	}
	else if(numTracePoints <= 1)
	{
		x = xVec.at(0);

		y = yVec.at(0);

		for(pointIndex = 0; pointIndex < resamplePoints; ++pointIndex)
		{
			resampledXVec.push_back(x);

			resampledYVec.push_back(y);

			LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) <<
                "resampled point " << x << y  << endl;
		}

		outTrace.addChannel(resampledXVec, xChannelstr);
		

		outTrace.addChannel(resampledYVec, yChannelstr);
		
	}
	else
	{
		LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << "Point distances"  << endl;

		for( pointIndex = 0; pointIndex < (numTracePoints-1); ++pointIndex)
		{
			xDiff = xVec.at(pointIndex) - xVec.at(pointIndex+1);

			yDiff = yVec.at(pointIndex) - yVec.at(pointIndex+1);

			pointDistance = (float) (sqrt(xDiff*xDiff + yDiff*yDiff)); //distance between 2 points.

			unitLength += pointDistance; // finding the length of trace.

			distanceVec.push_back(pointDistance); //storing distances between every 2 consecutive points.

			LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << "point distance: " <<
                pointDistance << endl;
		}

		unitLength /= (resamplePoints -1);

		LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) <<
            "unitLength = " << unitLength  << endl;

		x = xVec.at(0); // adding x of first point;

		y = yVec.at(0); // adding y of first point;

		resampledXVec.push_back(x);

		resampledYVec.push_back(y);

		LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << "resampled point " << x << y  << endl;

		for(pointIndex = 1; pointIndex < (resamplePoints-1); ++pointIndex)
		{
			measuredDistance = balanceDistance;

			while(measuredDistance < unitLength)
			{
				measuredDistance += distanceVec.at(ptIndex++);

				if(ptIndex == 1)
				{
					currentPointIndex = 1;
				}
				else
				{
					currentPointIndex++;
				}
			}

			if(ptIndex < 1) ptIndex = 1;

			m2 = measuredDistance - unitLength;

			m1 = distanceVec.at(ptIndex -1) - m2;

			if( fabs(m1+m2) > EPS)
			{
				xTemp =  (m1* xVec.at(currentPointIndex) + m2* xVec.at(currentPointIndex -1))/(m1+m2);

				yTemp =  (m1* yVec.at(currentPointIndex) + m2* yVec.at(currentPointIndex -1))/(m1+m2);
			}
			else
			{
				xTemp = xVec.at(currentPointIndex);

				yTemp = yVec.at(currentPointIndex);
			}

			resampledXVec.push_back(xTemp);

			resampledYVec.push_back(yTemp);

			LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << "resampled point " <<
                xTemp << yTemp  << endl;

			balanceDistance = m2;
		}

		x = xVec.at(xVec.size() - 1); // adding x of last point;

		y = yVec.at(yVec.size() - 1); // adding y of last point;

		resampledXVec.push_back(x);

		resampledYVec.push_back(y);

		LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) <<
            "resampled point " << x << y << endl;

		outTrace.addChannel(resampledXVec, xChannelstr);

		outTrace.addChannel(resampledYVec, yChannelstr);
	}

	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << 
        "Exiting PCAShapeRecognizer::resampleTrace"  << endl;
	
	return SUCCESS;
}

/**********************************************************************************
* AUTHOR		: Mehul Patel
* DATE			: 29-MAR-2005
* NAME			: ~HolisticFeatureExtractor
* DESCRIPTION	: destructor
* ARGUMENTS		: 
* RETURNS		: 
* NOTES			:
* CHANGE HISTROY
* Author			Date				Description of change
*************************************************************************************/

HolisticFeatureExtractor::~HolisticFeatureExtractor()
{
	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << 
        "Entered destructor of HolisticFeatureExtractor"  << endl;

	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << 
        "Exiting destructor of HolisticFeatureExtractor"  << endl;
}

