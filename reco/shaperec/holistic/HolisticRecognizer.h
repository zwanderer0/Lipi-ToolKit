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
 * FILE DESCR: Definitions for Holistic Recognition module 
 *
 * CONTENTS:   
 *
 * AUTHOR:     Mehul Patel
 *
 * DATE:       March 08, 2005
 * CHANGE HISTORY:
 * Author		Date			Description of change
 ************************************************************************/

#ifndef __HOLISTICRECOGNIZER_H
#define __HOLISTICRECOGNIZER_H

#define SUPPORTED_MIN_VERSION	"1.9.0"

#include "LTKTypes.h"

#include "LTKErrors.h"

#include "LTKErrorsList.h"

#include "LTKShapeRecognizer.h"

#include "LTKPreprocessor.h"

#include "LTKTraceGroup.h"

#include "LTKShapeRecoResult.h"

#include "LTKScreenContext.h"

#include "LTKCaptureDevice.h"

#include "LTKShapeRecoConfig.h"

#include "LTKShapeRecoEngine.h"

#include "HolisticFeatureExtractor.h"

#include "HolisticShapeModel.h"

#include "kNearestNeighbor.h"

#include "LTKDistToConfMapper.h"

#ifndef _WIN32
#include <dlfcn.h>
#endif

#include "LTKMacros.h"

#include "LTKCheckSumGenerate.h"

#include "LTKPreprocessorInterface.h"

class HolisticRecognizer;
class LTKShapeRecoUtil;

extern void *ghDeviceInfo;

extern int getversion(int *iMajor, int *iMinor, int *iBugFix);

typedef LTKPreprocessorInterface* (*FN_PTR_CREATELTKLIPIPREPROCESSOR)();
typedef void (*FN_PTR_DELETELTKLIPIPREPROCESSOR)(LTKPreprocessorInterface* p);

typedef int (HolisticRecognizer::*FN_PTR_PREPROCESSOR_FUNC_PCA)(const LTKTraceGroup& inTraceGroup, LTKTraceGroup& outTraceGroup);

typedef int (*PREPROC_FUN_PTR)(const LTKTraceGroup& inTraceGroup, LTKTraceGroup& outTraceGroup);

//required when there is a map for preprocessing sequence
/*struct MapModFunc
{
	string moduleName;
	string funcName;		
};*/
/**
 * @class HolisticRecognizer
 * <p> This class is derived from the abstract class LTKShapeRecognizer.<p> 
 */

class HolisticRecognizer : public LTKShapeRecognizer
{

private:

	bool m_reverseFeatureVectors;			//	flag to set if feature vector reversal is required or not during training

	
	// Pointer to an instacnce of preprocessor module
	LTKPreprocessorInterface *ltkShapeRecPtr;
	
	stringStringMap headerInfo;

	LTKCheckSumGenerate cheSumGen;				// Holds the checksum info

	FN_PTR_CREATELTKLIPIPREPROCESSOR  createLTKLipiPreProcessor;
	FN_PTR_DELETELTKLIPIPREPROCESSOR  deleteLTKLipiPreProcessor;

	unsigned short m_numShapes;		// total number of shapes

	int m_numNNs;			// number of nearest neighbors

	float m_worstCaseValue;	// worst case value of the distance

	HolisticFeatureExtractor m_featureExtractorObj;	// featureExtractor object of the shape recognizer

	//LTKPreprocessor m_preProcessor;			// preprocessor object of the shape recognizer

	string m_holisticCfgFile;				// Config file name containing the configuration parameters

	string m_referenceModelFile;			// File containing the prototypes for each class

	string m_confMapFile;					//	File that contains the config mapping

	LTKDistToConfMapper m_confMapper;       //	Maps distances to confidences
	
	vector<HolisticShapeModel> m_referenceModels;	// vector of prototypes for each class

	/*//required when there is a map for preprocessing sequence
	// map functions and its address.
	map<string, FN_PTR_PREPROCESSOR_FUNC_PCA> PreprocMap;*/
	
	//vector< MapModFunc > mapSequence;		// Holds the sequence of prepeocess function calls

	LTKCaptureDevice m_captureDevice;

	LTKScreenContext m_screenContext;	

	string xChannelstr, yChannelstr;

	bool m_projectTypeDynamic;						//  String variable to store the type of project

	LTKShapeRecoUtil *shaperecUtilInst;
public:

	/** @name Constructors and Destructor */
	//@{


	/**
	 * Default Constructor.
	 */

	HolisticRecognizer();

	/**
	 * Initialization Constructor.
	 */

	HolisticRecognizer(LTKShapeRecoConfig& config);

   /**
	* Destructor
	*/

	~HolisticRecognizer();

	/**
	 * @name Methods
	 */

	//@{

	/**
	* This method initializes Holistic recognizer
	*/

	int initialize(string& strProjectName, string &strProfileName);

	/**
	* This method loads the Training Data of the Holistic recognizer.
	*/

	int loadModelData();

	/**
	* This method calls the recognize method of the Holistic recognizer.
	* @param traceGroupObj The co-ordinates of the shape which is to be recognized
	* @param uiParams Contains information about the input field like whether it is boxed input
	*                 or continuous writing
	* @param deviceParams Contains information about the device like its sampling rate
	* @param subSetOfClasses A subset of the entire class space which is to be used for 
	*                        recognizing the input shape.
	* @param confThreshold A threshold on confidence value of the recognized class. This is
	*                      used as Rejection Criteria
	* @param numOfChoices Number of top choices to be returned in the result structure
	* @param results The result of recognition
	*/

	int recognize(const LTKTraceGroup& traceGroupObj, const LTKScreenContext& screenContext, 
		        const vector<bool>& shapeSubSet,
				float confThreshold, int numChoices, vector<LTKShapeRecoResult>& results) ;



//	int recognize(const LTKTraceGroup& traceGroupObj, const LTKScreenContext& screenContext, 
//		const LTKCaptureDevice& captureDevice, const vector<bool>& shapeSubSet, float confThreshold, 
//		int numChoices, vector<LTKShapeRecoResult>& results);


	/**
	* This method calls the learn method of the Holistic recognizer.
	* @param prototype The sample to be used for adaptation
	* @param classID The class label of the added prototype
	* @param uiParams Contains information about the input field like whether it is boxed input
	*                 or continuous writing
	* @param deviceParams Contains information about the device like its sampling rate
	*/
	
	int learn(const LTKTraceGroup& prototype, int shapeId, const LTKScreenContext& screenContext,
		              const LTKCaptureDevice& captureDevice) ;

	/**
	* This method calls the train method of the Holistic recognizer.
	*
	* @param trainingList The name of the file containing the listing of files to be used for
	*                    training each of the classes.
	*/

	int train(const string& trainingList, string& strModelDataHeaderInfoFile, string &comment, string &dataset, const string &inFileType=INK_FILE);

	/**
	* This method finds the distance between the test and the train feature vectors
	*
	* @param testVec The feature vector of the test sample
	* @param trainVec The feature vector of the train sample with which the test has to be compared
	* @param bestSoFar This is the best so far distance so that the function can break 
	*				   in case the distance goes beyond this value
	*/

	static float holisticDistance(const float2DVector& test,const HolisticShapeModel& train, float bestSoFar);

	int setDeviceContext(LTKCaptureDevice& deviceinfo);

	int getShapeRecoConfidences(const LTKTraceGroup& traceGroup, 
		const LTKScreenContext& screenContext,
		const vector<int>& shapeIds, vector<float>& shapeRecoConfidences);

	/**
	* This method returns the last error occured in PCA Module.
	*/
	int getLastError();
	int unloadModelData();

	//void setPreprocMap();

	

private:

	//int mapPreprocFunctions();

	//int resampleTraceGroup(const LTKTraceGroup& inTraceGroup, LTKTraceGroup& outTraceGroup);

	//int resampleTrace(const LTKTrace& inTrace,int resamplePoints, LTKTrace& outTrace);

	/**
	 * <p>This method does the necessary pre processing steps for LIPITk.
	 * It refers to the pre-processing parameters and calls the appropriate functions. </p>
	 *
	 * @param inObj The input object of type LTKTraceGroup
	 *
	 * @return an integer to indicate if the preprocessing was successful
	 */

	//int preprocess (const LTKTraceGroup& inTraceGroup, LTKPreprocessorInterface *ltkShapeRecPtr);

	/** Reads the NN.cfg and initializes the data members of the class
	*/
	int readClassifierConfig();
	
	/**
	* This method will assign default values to the members
	*
	* @return none
	*/
	void assignDefaultValues();
	
};

#endif

//#ifndef __HOLISTICRECOGNIZER_H
//#define __HOLISTICRECOGNIZER_H
