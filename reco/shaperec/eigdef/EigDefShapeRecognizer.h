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
 * FILE DESCR: Definitions for EigDef Shape Recognition module 
 *
 * CONTENTS:   
 *
 * AUTHOR:     Balaji R.
 *
 * DATE:       December 23, 2004
 * CHANGE HISTORY:
 * Author		Date			Description
 ************************************************************************/

#ifndef __EigDefShapeRecognizer_H
#define __EigDefShapeRecognizer_H

#define SUPPORTED_MIN_VERSION	"1.9.0"

#include "LTKInc.h"

#include "LTKErrors.h"

#include "LTKErrorsList.h"

#include "LTKPreprocessor.h"

#include "LTKTraceGroup.h"

#include "LTKShapeRecoResult.h"

#include "LTKScreenContext.h"

#include "LTKCaptureDevice.h"

#include "LTKShapeRecoConfig.h"

#include "LTKShapeRecoEngine.h"

#include "EigDefShapeModel.h"

#include <sys/stat.h>

#include <sstream>

#include <time.h>

#ifndef _WIN32
#include <dlfcn.h>
#endif

#include "LTKMacros.h"

#include "LTKCheckSumGenerate.h"

#include "LTKPreprocessorInterface.h"

class EigDefShapeRecognizer;
class LTKShapeRecoUtil;

extern void *ghDeviceInfo;

extern int getversion(int *iMajor, int *iMinor, int *iBugFix);

typedef LTKPreprocessorInterface* (*FN_PTR_CREATELTKLIPIPREPROCESSOR)();
typedef void (*FN_PTR_DELETELTKLIPIPREPROCESSOR)(LTKPreprocessorInterface* p);

typedef int (EigDefShapeRecognizer::*FN_PTR_PREPROCESSOR_FUNC_EigDef)(const LTKTraceGroup& inTraceGroup, LTKTraceGroup& outTraceGroup);

typedef int (*PREPROC_FUN_PTR)(const LTKTraceGroup& inTraceGroup, LTKTraceGroup& outTraceGroup);

struct MapModFunc
{
	string moduleName;
	string funcName;		
};


struct myPair
{
	int classId;
	float distance;
};

bool mycmp(struct myPair x, struct myPair y);

 /**
 * @class EigDefShapeRecognizer
 * <p> This class is derived from the abstract class LTKShapeRecognizer.<p> 
 */

class EigDefShapeRecognizer : public LTKShapeRecognizer
{

public:
    stringStringMap shapeRecognizerProperties;  //  contains details for configuration of the shape recognizer

private:
	
	bool m_reverseFeatureVectors;			//	flag to set if feature vector reversal is required or not during training

	// Pointer to an instacnce of preprocessor module
	LTKPreprocessorInterface *ltkShapeRecPtr;
	
	stringStringMap headerInfo;

	//pointer to an preprocessor function.
	FN_PTR_CREATELTKLIPIPREPROCESSOR  createLTKLipiPreProcessor;
	FN_PTR_DELETELTKLIPIPREPROCESSOR  deleteLTKLipiPreProcessor;

	LTKCheckSumGenerate cheSumGen;				// Holds the checksum info
	//float2DVector m_pointAllocStats;		//	point allocation statistics data

	//LTKPreprocessor m_preProcessor;			//	preprocessor object of the shape recognizer

	unsigned short m_numShapes;				//	number of shapes in the recognition problem

	unsigned short m_traceDimension;		//	number of points required from a trace group

	unsigned short m_numEigenVectors;		//	number of eigen vectors used to model a shape

	unsigned short m_numClusters;			// number of clusters required, if not specified it calculates automatically.

	//unsigned short m_smoothFilterLength;    //  length of smoothening filter

	int m_quantizationStep;					//	quantization step for resampling

	string m_referenceModelFile;			//	file containing the eigen vectors of each of the shapes

	vector<EigDefShapeModel> m_referenceModels;//	shape to eigen vectors correspondence objects

	vector<PREPROC_FUN_PTR> preproc;		//	pointer to an proprocessor function 

	string m_EigDefCfgFile;					//	full path of EigDef configuration file

	//string m_pointAllocStatsFile;			//	full path of point allocation statistics file

	string m_distanceMeasure;
	
	//DynamicTimeWarping<vector<float>,float,distPointer,reduceFunctionPointer> m_dtw;
	// map functions and its address.
	map<string, FN_PTR_PREPROCESSOR_FUNC_EigDef> PreprocMap;

	vector< MapModFunc > mapSequence;		// Holds the sequence of prepeocess function calls

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

	EigDefShapeRecognizer();

	/**
	 * Initialization Constructor.
	 */

	EigDefShapeRecognizer(LTKShapeRecoConfig& config);

   /**
	* Destructor
	*/

	~EigDefShapeRecognizer();

	/**
	 * @name Methods
	 */

	//@{

	/**
	* This method initializes the EigDef shape recognizer
	*/

	int initialize(string& strProjectName, string &strProfileName);

	/**
	* This function addes the maping elements ie function name and its address.
	*/
	void setPreprocMap();
	
	/**
	* This method loads the Training Data of the EigDef shape recognizer.
	*/

	int loadModelData();

	/**
	* This method calls the recognize method of the EigDef shape recognizer.
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
		const LTKScreenContext& screenContext,
		const vector<int>& shapeIds, vector<float>& shapeRecoConfidences);


	/**
	* This method calls the learn method of the EigDef shape recognizer.
	* @param prototype The sample to be used for adaptation
	* @param classID The class label of the added prototype
	* @param uiParams Contains information about the input field like whether it is boxed input
	*                 or continuous writing
	* @param deviceParams Contains information about the device like its sampling rate
	*/
	
	int learn(const LTKTraceGroup& prototype, int classID, const LTKScreenContext& screenContext, const LTKCaptureDevice& captureDevice);

	/**
	* This method calls the train method of the EigDef shape recognizer.
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
	* This method reads the point allocation statistics for a multi-trace shape
	*
	* @param statsFilePath Full path of the point allocation statistics file
	* 
	* @return SUCCESS on successful read operation
	*/

	//int readPointAllocFile(string statsFilePath);

   /**
	* This method does equidistant resampling of the given trace group to get 'traceDimension' 
	* number of points from the entire shape
	*
	* @param inTraceGroup trace group to be resampled
	* @param outTraceGroup resampled version of the given trace group
	*
	*@ return SUCCESS on successful resampling operation
	*/

	int resampleTraceGroup(const LTKTraceGroup& inTraceGroup, LTKTraceGroup& outTraceGroup);

	int resampleTrace(const LTKTrace& inTrace,int resamplePoints, LTKTrace& outTrace);


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
	int normalizeConf(float norm, float normFact, vector<LTKShapeRecoResult>& results);

	static double computeDTWDistance(Character& train,Character& test);

	static float computeEuclideanDistance(const Features& f1,const Features& f2);	
	
	static float computeDeformDistance(floatVector , floatVector , float2DVector , floatVector );

	static int ComputeDTWDeformationVector(float2DVector trainCord,  float2DVector testCord, int trainSize, int testSize, floatVector& DeformVec, float& distanceDTW);
	static float euclidDist(float ,float ,float ,float );


	/**
	* This function converts the character from TraceGroup format to Feature Vector format
	* @param traceGroupObj This is an input parameter and corresponds to the traceGroup format input.
	* @param outChar This is an output parameter and corresponds to the Vector format output.
	*/
	int convertTraceGroupToCharacter(const LTKTraceGroup& traceGroupObj, Character& outChar);

	static int reduceByHalf(const Character& character, Character& outChar);

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

//#ifndef __EigDefShapeRecognizer_H
//#define __EigDefShapeRecognizer_H
