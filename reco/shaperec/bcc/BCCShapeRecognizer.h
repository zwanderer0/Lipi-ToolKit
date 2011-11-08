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
 * FILE DESCR: Definitions for BCC Shape Recognition module 
 *
 * CONTENTS:   
 *
 * AUTHOR:     Balaji R.
 *
 * DATE:       December 23, 2004
 * CHANGE HISTORY:
 * Author		Date			Description
 ************************************************************************/

#ifndef __BCCShapeRecognizer_H
#define __BCCShapeRecognizer_H

#define SUPPORTED_MIN_VERSION	"1.9.0"

#include <sys/stat.h>
#include <sstream>
#include <time.h>

#ifndef _WIN32
#include <dlfcn.h>
#else
#include <windows.h>
#endif
#include "LTKInc.h"

#include "LTKPreprocessor.h"

#include "LTKTraceGroup.h"

#include "LTKShapeRecoResult.h"

#include "LTKScreenContext.h"

#include "LTKCaptureDevice.h"

#include "LTKShapeRecoConfig.h"

#include "LTKShapeRecoEngine.h"

//#include "PCAShapeRecognizer.h"

//#include "HolisticRecognizer.h"

#include "LTKMacros.h"
#include "LTKErrorsList.h"
#include "LTKErrors.h"	

/**
 * @class BCCShapeRecognizer
 * <p> This class is derived from the abstract class LTKShapeRecognizer.<p> 
 */

class BCCShapeRecognizer : public LTKShapeRecognizer
{
public: 
	public:
    LTKConfigFileReader *shapeRecognizerProperties;  //  contains details for configuration of the shape recognizer

	void *m_hPCA, *m_hHolistic;
	typedef LTKShapeRecognizer* (*FN_PTR_CREATESHAPERECOGNIZER)(void);
	typedef int (*FN_PTR_DELETESHAPERECOGNIZER)(LTKShapeRecognizer*);

	FN_PTR_CREATESHAPERECOGNIZER module_createShapeRecognizerPCA;
	FN_PTR_DELETESHAPERECOGNIZER module_deleteShapeRecognizerPCA;

	FN_PTR_CREATESHAPERECOGNIZER module_createShapeRecognizerHolistic;
	FN_PTR_DELETESHAPERECOGNIZER module_deleteShapeRecognizerHolistic;

	int mapPCAAndDTWModuleFunctions();

	LTKShapeRecognizer *m_pPCAObj;
	LTKShapeRecognizer *m_pHolisticObj;

private:	
	
	//LTKPreprocessor m_preProcessor;			//	preprocessor object of the shape recognizer

	unsigned short m_numShapes;				//	number of shapes in the recognition problem

	int m_numChoices;

	string m_bccCfgFile;					//	full path of bcc configuration file

	string m_combinationScheme;				//	"sum" rule or "bcc"<borda count combination> 

	//PCAShapeRecognizer m_pcaShapeRecognizer;

	//HolisticRecognizer m_holisticShapeRecognizer;
	stringStringMap headerInfo;

	LTKCaptureDevice m_captureDevice;

public:

	/** @name Constructors and Destructor */
	//@{


	/**
	 * Default Constructor.
	 */

	BCCShapeRecognizer();

	/**
	 * Initialization Constructor.
	 */

	//BCCShapeRecognizer(LTKShapeRecoConfig& config);

   /**
	* Destructor
	*/

	~BCCShapeRecognizer();

	/**
	 * @name Methods
	 */

	//@{

	/**
	* This method initializes the BCC shape recognizer
	*/

	int initialize(string& strProjectName, string &strProfileName);

	/**
	* This method loads the Training Data of the BCC shape recognizer.
	*/

	int loadModelData();

	/**
	* This method calls the recognize method of the BCC shape recognizer.
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
		 const vector<bool>& shapeSubSet, float confThreshold, 
		int numChoices, vector<LTKShapeRecoResult>& results);

		
	/**
	* This method calls the recognize method of the derived shape recognizer.
	*
	* @param traceGroup The co-ordinates of the shape which is to be recognized
	* @param screenContext Contains information about the input field like whether it is boxed input
	*                 or continuous writing
	* @param captureDevice Contains information about the device like its sampling rate
	* @param shapeIds A subset of the entire shape space for which shape recongizer confidences
	*                 need to be computed
	* @param shapeRecoConfidences the confidence values calculated by the recognizer
	*/

	int getShapeRecoConfidences(const LTKTraceGroup& traceGroup, 
		const LTKScreenContext& screenContext,const LTKCaptureDevice& captureDevice, 
		const vector<int>& shapeIds, vector<float>& shapeRecoConfidences);


	/**
	* This method calls the learn method of the BCC shape recognizer.
	* @param prototype The sample to be used for adaptation
	* @param classID The class label of the added prototype
	* @param uiParams Contains information about the input field like whether it is boxed input
	*                 or continuous writing
	* @param deviceParams Contains information about the device like its sampling rate
	*/
	
	int learn(const LTKTraceGroup& prototype, int classID, const LTKScreenContext& screenContext, const LTKCaptureDevice& captureDevice);

	/**
	* This method calls the train method of the BCC shape recognizer.
	*
	* @param trainingList The name of the file containing the listing of files to be used for
	*                    training each of the classes.
	*/

	int train(const string& trainingList, string& strModelDataHeaderInfoFile, string &comment, string &dataset, const string &inFileType=INK_FILE);	
	/**
	* This method unloads all the training data
	*/
	int unloadModelData();

		/**
	* This method sets the device context for the recognition
	*
	* @param deviceInfo The parameter to be set
	*/
	int setDeviceContext(LTKCaptureDevice& deviceinfo);

	/**
	* This method returns the last error occured in PCA Module.
	*/
	int getLastError();

	int mapPCAAndHolisticModuleFunctions();
};

#endif

//#ifndef __BCCShapeRecognizer_H
//#define __BCCShapeRecognizer_H
