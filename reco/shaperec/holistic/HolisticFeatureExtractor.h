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
 * FILE DESCR: Definitions of Holistic feature extraction module
 *
 * CONTENTS:   
 *
 * AUTHOR:     Mehul Patel
 *
 * DATE:       March 29, 2005
 * CHANGE HISTORY:
 * Author		Date			Description of change
 ************************************************************************/

#ifndef __HOLISTICFEATUREEXTRACTOR_H
#define __HOLISTICFEATUREEXTRACTOR_H

#include "LTKTraceGroup.h"

#include "LTKPreprocessor.h"

#include "LTKCaptureDevice.h"

#include "LTKScreenContext.h"

#include "LTKPreprocessorInterface.h"

/**
 * @class HolisticFeatureExtractor
 * <p> This class extracts the zonal features of the preprocessed shape.<p> 
 */

class HolisticFeatureExtractor
{
private:

	int m_numResamplePoints1;		// number of points after resampling

	int m_numResamplePoints2;		// number of points after resampling

	int m_smoothFilterLength1;		// filter length used for smoothening

	int m_smoothFilterLength2;		// filter length used for smoothening

	float m_swAngNormFactor;		// normalization factor for swept angle

	int m_swAngHookThresh;			// hook threshold in swept angle calculation

	int m_horizStrokeAngleThresh;	// angle threshold for horizonal stroke

public:

	string xChannelstr, yChannelstr;
	/** @name Constructors and Destructor */
	//@{

	/**
	 * Default Constructor.
	 */

	HolisticFeatureExtractor(int numPoints1=60, int numPoints2=20, int filterLength1=3, 
		int filterLength2=5, float swAngNormFactor=1.25, int swAngHookThresh=120,
		int horizStrokeAngleThresh=10);

	/**
	* Destructor
	*/

	~HolisticFeatureExtractor();

	// @}

	/**
	 * @name Methods
	 */

	// @{

	/**
	 * This method does the feature extraction from the preprocessed
	 * shape traceGroupObj
	 * @param traceGroupObj The input shape
	 * @param featureVector The feature vector extracted
	 */

	 int extractFeatures(const LTKTraceGroup& traceGroup, const LTKCaptureDevice& captureDevice, const LTKScreenContext& screenContext, LTKPreprocessorInterface *ltkShapeRecPtr, float2DVector& featureVector);

	/**
	 * The function returns the m_numResamplePoints1 used by the feature Extractor
	 * @param void
	 *
	 * @return m_numResamplePoints1
	 */

	 int getNumResamplePoints1() const;

	 /**
	 * The function returns the m_numResamplePoints2 used by the feature Extractor
	 * @param void
	 *
	 * @return m_numResamplePoints2
	 */

	 int getNumResamplePoints2() const;

	/**
	 * The function returns the m_smoothFilterLength1 used by the feature Extractor
	 * @param void
	 *
	 * @return m_smoothFilterLength1
	 */

	 int getSmoothFilterLength1() const;

	 /**
	 * The function returns the m_smoothFilterLength2 used by the feature Extractor
	 * @param void
	 *
	 * @return m_smoothFilterLength2
	 */

	 int getSmoothFilterLength2() const;

	 /**
	 * The function returns the m_swAngNormFactor used by the feature Extractor
	 * @param void
	 *
	 * @return m_swAngNormFactor
	 */

	 int getSwAngNormFactor() const;

	 /**
	 * The function returns the m_swAngHookThresh used by the feature Extractor
	 * @param void
	 *
	 * @return m_swAngHookThresh
	 */

	 int getSwAngHookThresh() const;

	 /**
	 * The function returns the m_smoothFilterLength2 used by the feature Extractor
	 * @param void
	 *
	 * @return m_smoothFilterLength2
	 */

	 int getHorizStrokeAngleThresh() const;

	/**
	 * The function sets the NumResamplePoints1
	 * @param void
	 *
	 * @return SUCCESS on successful set operation
	 */

	 int setNumResamplePoints1(int numPoints);

	/**
	 * The function sets the NumResamplePoints2
	 * @param void
	 *
	 * @return SUCCESS on successful set operation
	 */

	 int setNumResamplePoints2(int numPoints);

	 /**
	 * The function sets the SmoothFilterLength1 
	 * @param void
	 *
	 * @return SUCCESS on successful set operation
	 */

	 int setSmoothFilterLength1(int filterLength);

	 /**
	 * The function sets the SmoothFilterLength2 
	 * @param void
	 *
	 * @return SUCCESS on successful set operation
	 */

	 int setSmoothFilterLength2(int filterLength);

	 	 /**
	 * The function sets the SmoothFilterLength2 
	 * @param void
	 *
	 * @return SUCCESS on successful set operation
	 */

	 int setSwAngNormFactor(float swAngNormFactor);

	 	 /**
	 * The function sets the SmoothFilterLength2 
	 * @param void
	 *
	 * @return SUCCESS on successful set operation
	 */

	 int setSwAngHookThresh(int swAngHookThresh);

	 	 /**
	 * The function sets the SmoothFilterLength2 
	 * @param void
	 *
	 * @return SUCCESS on successful set operation
	 */

	 int setHorizStrokeAngleThresh(int horizStrokeAngleThresh);

	// @}

private:

	/**
	 * This is a private method which computes the End-to-end Ratio
	 * shape traceGroupObj
	 * @param float2DVector containing the preprocessed ink
	 */

	float computeEER(const LTKTrace& inTrace);

	/**
	 * This is a private method which computes the Orientation
	 * shape traceGroupObj
	 * @param float2DVector containing the preprocessed ink
	 */

	float computeOrientation(const LTKTrace& inTrace);

	/**
	 * This is a private method which computes the Total Curve Length
	 * shape traceGroupObj
	 * @param float2DVector containing the preprocessed ink
	 */

	float computeTCL(const LTKTrace& inTrace);

	/**
	 * This is a private method which computes the Position Information Index
	 * shape traceGroupObj
	 * @param float2DVector containing the preprocessed ink
	 */

	float computePII(const LTKTrace& inTrace, const LTKScreenContext screenContext);

	/**
	 * This is a private method which computes the Swept Angle
	 * shape traceGroupObj
	 * @param float2DVector containing the preprocessed ink
	 */

	float computeSweptAngle(const LTKTrace& inTrace);

	/**
	 * <p>This method does the necessary pre processing steps
	 * It refers to the pre-processing parameters and calls the appropriate functions. </p>
	 *
	 * @param inObj The input object of type LTKTraceGroup
	 *
	 * @return an integer to indicate if the preprocessing was successful
	 */

	int preprocess(const LTKTraceGroup& inTraceGroup, LTKTraceGroup& outTraceGroup, const LTKCaptureDevice& captureDevice, const LTKScreenContext& screenContext, LTKPreprocessorInterface *ltkShapeRecPtr);

	/**
	 * <p>This method does the necessary pre processing steps
	 * It refers to the pre-processing parameters and calls the appropriate functions. </p>
	 *
	 * @param inObj The input object of type LTKTraceGroup
	 *
	 * @return an integer to indicate if the preprocessing was successful
	 */

	int preprocess2(const LTKTraceGroup& inTraceGroup, LTKTraceGroup& outTraceGroup, const LTKCaptureDevice& captureDevice, const LTKScreenContext& screenContext, LTKPreprocessorInterface *ltkShapeRecPtr);

	/**
	 * <p>This method does the necessary pre processing steps
	 * It refers to the pre-processing parameters and calls the appropriate functions. </p>
	 *
	 * @param inObj The input object of type LTKTraceGroup
	 *
	 * @return an integer to indicate if the preprocessing was successful
	 */

	int preprocess3(const LTKTraceGroup& inTraceGroup, LTKTraceGroup& outTraceGroup, const LTKCaptureDevice& captureDevice, const LTKScreenContext& screenContext, LTKPreprocessorInterface *ltkShapeRecPtr);

	/**
	 * This method does uniform resampling of the traces to m_numPoints
	 *
	 * @param inTraceGroup trace group to be resampled
	 * @param outTraceGroup resampled version of the given trace group
	 *
	 *@ return SUCCESS on successful resampling operation
	 */
	
	int resampleTraceGroup(LTKTraceGroup& inTraceGroup, int numPoints, LTKTraceGroup& outTraceGroup, LTKPreprocessorInterface *ltkShapeRecPtr);

	/**
	 * This function calculates Euclidian distance between 2 points.
	 *
	 * @param x1,x2,y1,y2 Coordinates of two points.
	 *
	 * @return Euclidian Distance between two points.
	 */

	int HolisticFeatureExtractor::resampleTrace(const LTKTrace& inTrace,int resamplePoints, LTKTrace& outTrace);

	float calculateEuclidDist(float x1,float y1,float x2,float y2);

	/**
	 * This function calculates slope of 2 points.
	 *
	 * @param x1,x2,y1,y2 coordinates of 2 points
	 *
	 * @return Slope of two points.
	 */
	
	float calculateSlope(float x1,float y1,float x2,float y2);

	/**
	 * This function calculates the perimeter of the bounding box
	 *
	 * @param screenContext
	 *
	 * @return perimeter
	 */
	
	float calculateBBoxPerimeter(LTKScreenContext screenContext);
};

#endif

//#ifndef __HOLISTICFEATUREEXTRACTOR_H
//#define __HOLISTICFEATUREEXTRACTOR_H
