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
 * FILE DESCR: Definitions for PCA Shape Recognition module 
 *
 * CONTENTS:   
 *
 * AUTHOR:     Balaji R.
 *
 * DATE:       December 23, 2004
 * CHANGE HISTORY:
 * Author		Date			Description of change
 ************************************************************************/

#ifndef __PCAShapeRecognizer_H
#define __PCAShapeRecognizer_H

#include "LTKInc.h"

#include <sys/stat.h>

#include <sstream>

#include <time.h>

#ifndef _WIN32
#include <dlfcn.h>
#endif

#include "LTKMacros.h"

#include "LTKCheckSumGenerate.h"

#include "LTKPreprocessorInterface.h"

#include "PCAShapeModel.h"

#include "LTKScreenContext.h"

#include "LTKCaptureDevice.h"

#define SUPPORTED_MIN_VERSION	"1.9.0"

class PCAShapeRecognizer;
class LTKTraceGroup;
class LTKShapeRecoConfig;
class LTKShapeRecoUtil;
class LTKShapeFeatureExtractor;
class LTKShapeFeature;
class LTKShapeSample;

extern void *ghDeviceInfo;

extern int getversion(int *iMajor, int *iMinor, int *iBugFix);



typedef int (PCAShapeRecognizer::*FN_PTR_PREPROCESSOR_FUNC_PCA)(const LTKTraceGroup& inTraceGroup, LTKTraceGroup& outTraceGroup);

typedef int (*PREPROC_FUN_PTR)(const LTKTraceGroup& inTraceGroup, LTKTraceGroup& outTraceGroup);

typedef LTKPreprocessorInterface* (*FN_PTR_CREATELTKLIPIPREPROCESSOR)(LTKControlInfo controlInfo);
typedef void (*FN_PTR_DELETELTKLIPIPREPROCESSOR)(LTKPreprocessorInterface* p);

struct MapModFunc
{
	string moduleName;
	string funcName;		
};

/**
 * @class PCAShapeRecognizer
 * <p> This class is derived from the abstract class 
 * LTKShapeRecognizer. This class implements the shape 
 * recognition alogirithm using subspace methods. The 
 * training involves calculation of class-wise eigen 
 * subspaces.  Recognition involves minimum distance 
 * classification using distance of sample from the 
 * subspaces[Deepu V. and Sriganesh M. 2004] <p> 
 */

class PCAShapeRecognizer : public LTKShapeRecognizer
{
private:
	void *m_libHandler;
	FN_PTR_DELETELTKLIPIPREPROCESSOR deleteLTKLipiPreProcessor;
										//Function pointer for deleteLTKLipiPreProcessor

	string m_preProcSeq;
	
	LTKShapeRecoUtil *m_ptrShapeRecUtil;		// object used to call the getAbsolute() function

	bool m_reverseFeatureVectors;			//	flag to set if feature vector reversal is required or not during training

	
	// Pointer to an instacnce of preprocessor module
	LTKPreprocessorInterface *m_ltkPreprocPtr;
	
	stringStringMap m_headerInfo;

	LTKCheckSumGenerate cheSumGen;				// Holds the checksum info

	unsigned short m_numShapes;				//	number of shapes in the recognition problem

	unsigned short m_traceDimension;		//	number of points required from a trace group

	unsigned short m_numEigenVectors;		//	number of eigen vectors used to model a shape
	
	bool m_projectTypeDynamic;						//  String variable to store the type of project

	string m_referenceModelFile;			//	file containing the eigen vectors of each of the shapes


	map<int,PCAShapeModel> m_referenceModels; //shape ID to class model (eigen vectors) correspondence

	vector<PREPROC_FUN_PTR> preproc;		//	pointer to an proprocessor function 

	string m_pcaCfgFile;					//	full path of pca configuration file

	string m_confMapFile;					//	File that contains the config mapping

	string m_distanceMeasure;

	// map functions and its address.
	map<string, FN_PTR_PREPROCESSOR_FUNC_PCA> PreprocMap;

	vector< MapModFunc > mapSequence;		// Holds the sequence of prepeocess function calls

	LTKCaptureDevice m_captureDevice;

	LTKScreenContext m_screenContext;	

	string xChannelstr, yChannelstr;

	string m_featureExtractor;
	LTKShapeFeatureExtractor *m_ptrFeatureExtractor;
	string m_lipiRootPath;
	string m_currentVersion;

public:

	/** @name Constructors and Destructor */
	//@{

	/**
	 * Initialization Constructor.
	 */

	PCAShapeRecognizer(LTKControlInfo controlInfo);

   /**
	* Destructor
	*/

	virtual ~PCAShapeRecognizer();

	/**
	 * @name Methods
	 */

	//@{

	/**
	* This function addes the maping elements ie function name and its address.
	*/
	void setPreprocMap();

	/**
	* This method loads the Training Data of the PCA shape recognizer.
	*/
	int loadModelData();

	/**
	* This method calls the recognize method of the PCA shape recognizer.
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
		vector<int>& shapeSubSet, float confThreshold, 
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
		const LTKScreenContext& screenContext,
		const vector<int>& shapeIds, vector<float>& shapeRecoConfidences);


	/**
	* This method calls the learn method of the PCA shape recognizer.
	* @param prototype The sample to be used for adaptation
	* @param classID The class label of the added prototype
	* @param uiParams Contains information about the input field like whether it is boxed input
	*                 or continuous writing
	* @param deviceParams Contains information about the device like its sampling rate
	*/
	
	int learn(const LTKTraceGroup& prototype, int classID, const LTKScreenContext& screenContext, const LTKCaptureDevice& captureDevice);

	/**
	* This method calls the train method of the PCA shape recognizer.
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


private:



	/**
	 * <p>This method does the necessary pre processing steps for LIPITk.
	 * It refers to the pre-processing parameters and calls the appropriate functions. </p>
	 *
	 * @param inObj The input object of type LTKTraceGroup
	 *
	 * @return an integer to indicate if the preprocessing was successful
	 */

	int preprocess (const LTKTraceGroup& inTraceGroup, LTKTraceGroup& outTraceGroup);
	
   /**
	* This method computes the eigenvectors and eigenvalues of a given covariance matrix
	*
	* @param covarianceMatrix covariance matrix whose eigenvectors are sought for
	* @param n rank of the covariance matrix
	* @param eigenValues eigen values of the given covariance matrix
	* @param eigenVectorMatrix eigen vectors of the given covariance matrix
	* @param nrot number of iterations required for the algorithm to converge
	* 
	* @return SUCCESS on successful eigen vector computation operation
	*/

	int computeEigenVectors(vector< vector<float> > &covarianceMatrix, int n,vector<float> &eigenValues, 
				vector< vector<float> > &eigenVectorMatrix, int& nrot);

   /**
	* This method reverses the x, y features of a given feature vector
	*
	* @param featureVector feature vector whose features are to be reverse
	* @param revFeatureVector the given feature vector after its x and y coordinate features are 
	*		 reversed
	*
	* @return SUCCESS on successful feature reverse operation
	*/

	int reverseFeatureVector(floatVector& featureVector, floatVector& revFeatureVector);

	/**
	* This method maps preprocessing functions from the dll
	*
	* @return SUCCESS on successful completion of mappings
	*/
	int mapPreprocFunctions();

	/**
	* This method normalizes the confidences
	* 
	* @param norm       Norm of the feature vector. Used  for normal distance measure
	* @param normFact   normalization factor for weighted distance measure
	*
	* @return SUCCESS on successful completion of mappings
	*/
	//int normalizeConf(float norm, float normFact, vector<LTKShapeRecoResult>& results);
	float PCAShapeRecognizer::normalizeConf(float normFact, float distance);

	void initializeFeatureExtractorInstance(LTKControlInfo controlInfo);

	int mapFeatureExtractor();

	int convertLTKShapeFeToFloatVec(vector<LTKShapeFeature*>& shapeFeature,floatVector featureVector);

	void unloadFeatureExtractorInstance();

	int trainFromFeatureFile(const string& featureFilePath);

	int trainFromListFile(const string& trainingList);

	int getShapeSampleFromLine(string& line, LTKShapeSample& shapeSampleFeatures);

	void updateHeaderWithAlgoInfo();


private :

	/** Reads the NN.cfg and initializes the data members of the class
	*/
	int readClassifierConfig();

	
	/** This method is used to initialize the PreProcessor
	*
	* Semantics
	*
	* 	- Load the preprocessor DLL using LTKLoadDLL().
	*
	* 	- Get the proc address for creating and deleting the preprocessor instance.
	*
	* 	- Create preprocessor instance.
	*
	* 	- Start the logging for the preprocessor module.
	*	
	* @param  preprocDLLPath : string : Holds the Path of the Preprocessor DLL, 
	* @param  errorStatus    : int    : Holds SUCCESS or Error Values, if occurs
	* @return preprocessor instance
	*
	* @exception	ELOAD_PREPROC_DLL			Could not load preprocessor DLL
	* @exception	EDLL_FUNC_ADDRESS_CREATE	Could not map createPreprocInst
	* @exception	EDLL_FUNC_ADDRESS_DELETE	Could not map destroyPreprocInst
	*/
	LTKPreprocessorInterface* initializePreprocessor(const string preprocDLLPath, LTKControlInfo controlInfo);

	/** This method is used to deletes the PreProcessor instance
	*
	* Semantics
	*
	* 	- Call deleteLTKPreprocInst from the preproc.dll.
	*
	* 	- Unload the preprocessor DLL.
	*
	* @param  ptrPreprocInstance : Holds the pointer to the LTKPreprocessorInterface 
	* @return none
	* @exception none
	*/
	void deletePreprocessor(LTKPreprocessorInterface *ptrPreprocInstance);

	/** This method is used to Unloads the preprocessor DLL.
	*
	* Semantics
	*
	* 	- If m_libHandler != NULL, unload the DLL
	*		LTKUnloadDLL(m_libHandler);
	*		m_libHandler = NULL;
	*	
	* @param none
	* @return none
	* @exception none
	*/
	void unloadPreprocessorDLL();
	 
	
};

#endif

//#ifndef __PCAShapeRecognizer_H
//#define __PCAShapeRecognizer_H
