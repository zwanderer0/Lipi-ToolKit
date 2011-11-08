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
 * FILE DESCR: Implementation for Holistic Recognition module
 *
 * CONTENTS:
 *	loadTrainingData
 *	recognize
 *	learn
 *	train
 *	getPointAllocStats
 *	resampleTraceGroup
 *	preprocess
 *
 * AUTHOR:     Mehul Patel
 *
 * DATE:       March 09, 2005
 * CHANGE HISTORY:
 * Author		Date			Description of change
 ************************************************************************/
#ifdef _WIN32
#include <windows.h>
#endif

#include "HolisticRecognizer.h"

#include "HolisticFeatureExtractor.h"

#include "LTKInkFileReader.h"

#include "LTKConfigFileReader.h"

#include "LTKLoggerUtil.h"

#include "LTKException.h"

#include "LTKPreprocDefaults.h"

#include "lipiengine.h"

#include "LTKErrorsList.h"

#include "LTKShapeRecoUtil.h"

extern void *m_libHandler;


/**********************************************************************************
* AUTHOR		: Mehul Patel
* DATE			: 09-MAR-2005
* NAME			: HolisticRecognizer
* DESCRIPTION	: Default Constructor
* ARGUMENTS		: 
* RETURNS		: 
* NOTES			:
* CHANGE HISTROY
* Author			Date				Description of change
*************************************************************************************/

HolisticRecognizer::HolisticRecognizer()
{
	m_projectTypeDynamic = false;
	shaperecUtilInst = new LTKShapeRecoUtil();	

}

/**********************************************************************************
* AUTHOR		: Mehul Patel
* DATE			: 09-MAR-2005
* NAME			: HolisticRecognizer
* DESCRIPTION	: Initialization Constructor
* ARGUMENTS		: 
* RETURNS		: 
* NOTES			:
* CHANGE HISTROY
* Author			Date				Description of change
*************************************************************************************/

HolisticRecognizer::HolisticRecognizer(LTKShapeRecoConfig& config) : LTKShapeRecognizer(HOLISTIC)
{
	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << "Entered default constructor of HolisticRecognizer"  << endl;

	//	initializing number of shapes in the shape set

	m_numShapes = config.getNumShapes();

	//	composing the Holistic configuration file name

	this->m_holisticCfgFile = config.getLipiRoot() + SEPARATOR + config.getShapeSet() + SEPARATOR + 
		     config.getProfile() + SEPARATOR + m_shapeRecognizerName + CONFIGFILEEXT;

	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) <<  
        "Holistic Configuration File is: " << m_holisticCfgFile << endl;

	//	composing the name of the reference model file

	this->m_referenceModelFile = config.getLipiRoot() + SEPARATOR + config.getShapeSet() + 
						SEPARATOR + config.getProfile() + SEPARATOR + m_shapeRecognizerName + DATFILEEXT;

	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << 
        "Reference Model File is: " << m_referenceModelFile << endl;

    //  composing the name of the confidence mapping file

   // m_cfgMapFile = config.getLipiRoot() + SEPARATOR + config.getShapeSet() + SEPARATOR +
     //         config.getProfile() + SEPARATOR ;

    m_confMapFile = "";
	xChannelstr = "X";
	yChannelstr = "Y";
	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << 
        "Holistic recognizer instantiated" << endl;

	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) <<
        "Exiting default constructor of HolisticRecognizer" << endl;
}

/**********************************************************************************
* AUTHOR		: Mehul Patel
* DATE			: 09-MAR-2005
* NAME			: initializeShapeRecognizer
* DESCRIPTION	: Initializes the members
* ARGUMENTS		: 
* RETURNS		: SUCCESS on successful initialization
* NOTES			:
* CHANGE HISTROY
* Author			Date				Description of change
*************************************************************************************/

int HolisticRecognizer::initialize(string& strProjectName, string &strProfileName)
{
	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << 
        "Entered HolisticShapeRecognizer::Initialize" << endl;

	string LipiRootPath = string(getLipiPath());	
	string tempStr = "";
    string preprocDllPath = "";
	headerInfo[PROJNAME] = strProjectName;
	int returnStatus=0;
	string strNumShapes = "";

	ltkShapeRecPtr = NULL;	

	//path for holistic.cfg
	m_holisticCfgFile = LipiRootPath + PROJECTS_PATH_STRING + strProjectName + PROFILE_PATH_STRING + strProfileName + SEPARATOR + HOLISTIC + CONFIGFILEEXT;

	//path for holistic.mdt
	m_referenceModelFile = LipiRootPath + PROJECTS_PATH_STRING + strProjectName + PROFILE_PATH_STRING + strProfileName + SEPARATOR + HOLISTIC + DATFILEEXT;

	m_confMapFile = LipiRootPath + PROJECTS_PATH_STRING + strProjectName + PROFILE_PATH_STRING + strProfileName + SEPARATOR;
                
	// Read number of shapes from project.cfg
	tempStr = LipiRootPath + PROJECTS_PATH_STRING + strProjectName + PROFILE_PATH_STRING + PROJECT_CFG_STRING;
	try
	{
		//To find whether the project was dynamic or not
		//to read read number of shapes from project.cfg
		m_projectTypeDynamic = shaperecUtilInst->isProjectDynamic(tempStr, m_numShapes, returnStatus, strNumShapes);
		
		headerInfo[NUMSHAPES] = strNumShapes;
		
		preprocDllPath = LipiRootPath + SEPARATOR + "lib" + SEPARATOR + PREPROC + DLL_EXT;

		// initialize preprocessor
		returnStatus = 0;
		ltkShapeRecPtr = shaperecUtilInst->initializePreprocessor(preprocDllPath,
						returnStatus);

	}
	
	catch(LTKException e)
	{
		LOG( LTKLogger::LTK_LOGLEVEL_ERR) << e.getExceptionMessage() << endl;
		LTKReturnError(returnStatus);
	}
	
	//setPreprocMap(); // To add function name and the function address into the mapping variable.

	//Assigning Default Values
	assignDefaultValues();

	//Reading pca configuration file
	returnStatus = readClassifierConfig();

	return returnStatus;
}

/**********************************************************************************
* AUTHOR		: Mehul Patel
* DATE			: 09-MAR-2005
* NAME			: loadTrainingData
* DESCRIPTION	: loads the reference model file into memory
* ARGUMENTS		: 
* RETURNS		: SUCCESS on successful loading of the reference model file
* NOTES			:
* CHANGE HISTROY
* Author			Date				Description of change
*************************************************************************************/

int HolisticRecognizer::loadModelData(void)
{
	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << "Entered HolisticRecognizer::loadTrainingData"  << endl;

	int numShapes;					//	number of shapes in the problem set

	int shapeId;					//	temp variable for storing the shape ID of incoming tracegroup

	int numFeatures;				//	total number of features used

	int numStrokes;					//	number of strokes in the character

	HolisticShapeModel shapeModel;	//	a shape model variable

	string algoVersion;					//Algorithm version.

	char versionInfo[VERSION_STR_LEN];
	string version;
	int iMajor, iMinor, iBugfix;

	stringStringMap headerSequence;

	float2DVector::iterator strokeIter; //	Iterator to loop over number of strokes
	float2DVector::iterator strokeIterEnd;

	floatVector::iterator loopIter;		// Iterator to loop over all the features
	floatVector::iterator loopIterEnd;

	// load the training file for the mapping
	if(m_confMapFile != "")
	{
		m_confMapper.readConfigMapping(m_confMapFile);
	}
	else
	{
		LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << "Warning: Mapping file is not set in the config" << endl;
	}

	
	if((errorCode = cheSumGen.readMDTHeader(m_referenceModelFile,headerSequence)) != 0)
	{
		return errorCode;
		//throw LTKException("ModelData File has been corrupted" + m_referenceModelFile);
	}
	
	algoVersion = headerSequence[RECVERSION].c_str();

	getCurrentVersion(&iMajor, &iMinor, &iBugfix);

	sprintf(versionInfo, "%c", algoVersion[0]);

	int ver = atoi(versionInfo);
	if(ver != iMajor)

	{
		LOG( LTKLogger::LTK_LOGLEVEL_ERR) << "Incompatible algorithm version : " + version  << endl;

		LTKReturnError(EINCOMPATIBLE_VERSION);
	}

	// open the reference model file for loading the training data
	ifstream infile(m_referenceModelFile.c_str(),ios::in);

	if(!infile)
    {
		LOG( LTKLogger::LTK_LOGLEVEL_ERR) << "Unable to open model data file : " +  m_referenceModelFile  << endl;

		LTKReturnError(EMODEL_DATA_FILE_OPEN);
	}


	infile.seekg(atoi(headerSequence[HEADERLEN].c_str()),ios::beg);  
	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << "Reference model file read"  << endl;

	//	reading reference model file header

	infile >> numShapes;

	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << "number of shapes read from reference model file = " << numShapes << endl;

	if(m_numShapes != numShapes)
    {
		LOG( LTKLogger::LTK_LOGLEVEL_ERR) << "Invalid value for number of shapes"  << endl;
		LOG( LTKLogger::LTK_LOGLEVEL_ERR) << "NumShapes from cofiguration file ( or default value ) : " << m_numShapes << endl;
		LOG( LTKLogger::LTK_LOGLEVEL_ERR) << "NumShapes from model data file : " << numShapes << endl;

		LTKReturnError(EINVALID_NUM_OF_SHAPES);
    }

	infile >> numFeatures;

	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << "number of features read from reference model file = " << numFeatures << endl;
	
	floatVector tempFloatVector(numFeatures,0);	// temporary float vector

	// reading the prototype models

	while(infile)
	{
		infile >> shapeId;

		infile >> numStrokes;

		float2DVector featureVector(numStrokes,tempFloatVector);

		// reading from the dat file and populating the featureVector 

		strokeIterEnd = featureVector.end();
		
		for(strokeIter = featureVector.begin(); strokeIter != strokeIterEnd; ++strokeIter)
		{
			loopIterEnd = (*strokeIter).end();

			for(loopIter = (*strokeIter).begin(); loopIter != loopIterEnd; ++loopIter)
			{
				infile >> (*loopIter);
			}
		}

		shapeModel.setShapeId(shapeId);

		shapeModel.setModel(featureVector);

		m_referenceModels.push_back(shapeModel);	// populating the reference models

	}

	cout << "Total number of reference models read - " << m_referenceModels.size() << endl;

	infile.close();

	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << "Exiting HolisticRecognizer::loadTrainingData"  << endl;

	return SUCCESS;

}

/**********************************************************************************
* AUTHOR		: Mehul Patel
* DATE			: 09-MAR-2005
* NAME			: recognize
* DESCRIPTION	: recognizes the incoming tracegroup
* ARGUMENTS		: traceGroup - trace group to be recognized
*				  uiParams - screen context
*				  deviceParams - device information
*				  shapeSubSet - possible output space of the recognition result
*				  confThreshold - shapes with confidence below this threshold are rejected
*				  numChoices - maximum number of choices to be returned
*				  resultVec - result of recognition
* RETURNS		: SUCCESS on successful running of the code
* NOTES			:
* CHANGE HISTROY
* Author			Date				Description of change
*************************************************************************************/

int HolisticRecognizer::recognize(const LTKTraceGroup& traceGroup, const LTKScreenContext& screenContext, 
		         const vector<bool>& shapeSubSet, float confThreshold, 
    int numChoices, vector<LTKShapeRecoResult>& resultVec)
{
	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << "Entered HolisticRecognizer::recognize"  << endl;

	float2DVector testFeatureVector;

	intVector kNNindices;					// indices of the min distance models

	floatVector kNNdist;					// kNN distances corresponding to the indices

	LTKShapeRecoResult outResult;			// result

	int shapeIndex;							// index used for looping over all the shapes

	int loopIndex;							// index used for looping

	vector<HolisticShapeModel> subSetOfReferenceModels;	// subset on which the comparisons have to be made
	
	intVector labelVec;                     //  list of current top choices                 

    floatVector distanceVec;                //  list of current top holistic distances
	
	intVector recognitionCount(m_numShapes,0);	// count of recognized classes

	float confidence;							// the confidence of the recognition
	
	// extracting the features from the incoming tracegroup

	m_featureExtractorObj.extractFeatures(traceGroup, m_captureDevice, screenContext, ltkShapeRecPtr, testFeatureVector);	

	// if a particular class is not present in the shapeSubSet then it has to be rejected
	
	if (shapeSubSet.empty()==true)
	{
		subSetOfReferenceModels = m_referenceModels;
	}
	else
	{
		for(loopIndex = 0; loopIndex < m_referenceModels.size(); ++loopIndex)
		{
			int shapeId;

			shapeId = (m_referenceModels.at(loopIndex)).getShapeId();
			
			if (shapeSubSet.at(shapeId) == true)
			{
				subSetOfReferenceModels.push_back(m_referenceModels.at(loopIndex));
			}
		}
	}

	findkNN(testFeatureVector, subSetOfReferenceModels, holisticDistance, m_numNNs, m_worstCaseValue, kNNindices, kNNdist);

	// counting the number of correct recognitions corresponding to each shapeId
	
	for(loopIndex = 0; loopIndex < m_numNNs; ++loopIndex)
	{
		int shapeId;

		shapeId = (subSetOfReferenceModels.at(kNNindices.at(loopIndex))).getShapeId();

		++(recognitionCount.at(shapeId));
		
		
	}



	// getting the top N counts and populating the resultVec
	
	for(loopIndex = 0; loopIndex < numChoices; ++loopIndex)
	{
		int maxCount = 0;

		int maxCountIndex = -1;

		// finding the class for which recognitionCount is the maximum

		for(shapeIndex = 0; shapeIndex < m_numShapes; ++shapeIndex)
		{
			if (maxCount < recognitionCount.at(shapeIndex))
			{
				maxCount = recognitionCount.at(shapeIndex);

				maxCountIndex = shapeIndex;
			}
		}

		// to find the class with next maximum recognitionCount, the
		
		outResult.setShapeId(maxCountIndex);

		if(maxCountIndex != -1)
		{
			confidence = (float)recognitionCount.at(maxCountIndex)/(float)m_numNNs;

			outResult.setConfidence(m_confMapper.mapDistanceToConfidence(-(confidence)));
		}
		else
		{
			outResult.setConfidence(0);
		}

		resultVec.push_back(outResult);

		// present class recognitionCount is set to zero

		if (maxCountIndex != -1)
		{
			recognitionCount.at(maxCountIndex) = 0;
		}

		
	}

	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << "Holistic Recognize Called"  << endl;

	return SUCCESS;

}
    
/**********************************************************************************
* AUTHOR		: Mehul Patel
* DATE			: 09-MAR-2005
* NAME			: learn
* DESCRIPTION	: on the fly adaptation
* ARGUMENTS		: prototype - prototype to be learned
*				  shapeId - label of the prototype to be learned
*				  uiParams - screen context
*				  deviceParams - device information
* RETURNS		: SUCCESS on successful learning
* NOTES			:
* CHANGE HISTROY
* Author			Date				Description of change
*************************************************************************************/

int HolisticRecognizer::learn(const LTKTraceGroup& prototype, int shapeId, const LTKScreenContext& screenContext,
		              const LTKCaptureDevice& captureDevice)
{
	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << "Entered HolisticRecognizer::learn"  << endl;

	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << "Exiting HolisticRecognizer::learn"  << endl;

	return SUCCESS;
}

/**********************************************************************************
* AUTHOR		: Mehul Patel
* DATE			: 09-MAR-2005
* NAME			: 

* DESCRIPTION	: offline training module
* ARGUMENTS		: trainingList - name of the file containing the listing of the files to be used for
*				  training each of the shapes
* RETURNS		: SUCCESS on successful training
* NOTES			:
* CHANGE HISTROY
* Author			Date				Description of change
*************************************************************************************/

int HolisticRecognizer::train(const string& trainingList, string& strModelDataHeaderInfoFile, string &comment, string &dataset, const string &inFileType)
{
	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << "Entered HolisticRecognizer::train"  << endl;

	string path;							//	file path of a ink file

	string shapeId;							//	shape id of the ink file

	int shapeCount = 0;						//	count of the no. of shape samples read so far

	int prevShape = 0;						//	shape id of the previous shape

	int numStrokes = 0;						//	number of strokes in a particular character

	int numFeatures = 0;					//	number of features being used

	int	traceIndex;							//	variable to loop over all traces of the trace group

	bool firstFileFlag = true;				//	flag is used for writing the number of features in the dat file

	LTKCaptureDevice captureDevice;

	LTKScreenContext screenContext;

	int iMajor, iMinor, iBugFix;			// Version identifiers

	char versionInfo[VERSION_STR_LEN];					// Holds the version information.

    string version;	///, pca="PCA";


	//	opening the file containing the training list of each of the shapes	
	
	ifstream in(trainingList.c_str());

	//	throwing error if unable to open the training list file

	if(!in)
	{
		throw LTKException(ETRAINLIST_FILE_OPEN);
	}

	//	opening the output reference model file

	ofstream out(m_referenceModelFile.c_str(),ios::out);

	// throwing error if unable to open the reference model file

	if(!out)
	{
		throw LTKException(EMODEL_DATA_FILE_OPEN);
	}

	//	writing the header information into the reference model file

	out << m_numShapes << endl;

	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << "m_numShapes = " << m_numShapes << endl;

	//	reading the ink file paths from the training list file
	
	while(in)
	{
		
		getline(in, path, ' ');

		getline(in, shapeId);

		if(atoi(shapeId.c_str()) != prevShape)
		{
			++shapeCount;

			prevShape = atoi(shapeId.c_str()); 
		}

		// checking for end of training data

		if(shapeCount == m_numShapes)
		{
			break;
		}

		LTKTraceGroup traceGroup;

		float2DVector featureVector;

		cout << path ;

		// reading the ink files

		LTKInkFileReader::readUnipenInkFile(path, traceGroup, captureDevice, screenContext);

		numStrokes = traceGroup.getNumTraces();
		
		// extracting features from the traceGroup

		m_featureExtractorObj.extractFeatures(traceGroup, captureDevice, screenContext, ltkShapeRecPtr, featureVector);

		// writing the number of features in the dat file only the first time

		if (firstFileFlag==true)
		{
			numFeatures = featureVector.at(0).size();			

			out << numFeatures << endl;
		}

		firstFileFlag = false;

		// writing the shapeId and the corresponding features in the dat file

		out << shapeId << " ";

		out << numStrokes << " ";
		
		for(traceIndex = 0; traceIndex < numStrokes; ++traceIndex)
		{
			for(int loopIndex = 0; loopIndex < numFeatures; ++loopIndex)
			{
				out << featureVector.at(traceIndex).at(loopIndex) << " ";
			}
		}

		out << endl;
		
	}//while(in)

	//	closing the training list file and returning

	in.close();

	out.close();

	getCurrentVersion(&iMajor,&iMinor,&iBugFix);

	sprintf(versionInfo, "%d.%d.%d",iMajor, iMinor, iBugFix);

	version = versionInfo;

	headerInfo[RECVERSION] = version;
	string algoName = "holistic";
	headerInfo[RECNAME] = algoName;
	headerInfo[COMMENT]=comment;
	headerInfo[DATASET]=dataset;
	if(errorCode = cheSumGen.addHeaderInfo(strModelDataHeaderInfoFile, m_referenceModelFile, headerInfo))
	{
		return errorCode;
	}

	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << "Holistic Engine Trained"  << endl;

	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << "Exiting HolisticRecognizer::train"  << endl;

	return SUCCESS;
}

/**********************************************************************************
* AUTHOR		: Mehul Patel
* DATE			: 07-APR-2005
* NAME			: holisticDistance
* DESCRIPTION	: Finds the distance between the test and the train feature vectors
* ARGUMENTS		: testVec - The feature vector of the test sample
				  trainVec - The feature vector of the train sample with which the 
							 test has to be compared
				  bestSoFar - This is the best so far distance so that the function 
							  can break in case the distance goes beyond this value
* RETURNS		: distance between testVec and trainVec
* NOTES			:
* CHANGE HISTORY
* Author			Date				Description of change
*************************************************************************************/

float HolisticRecognizer::holisticDistance(const float2DVector& test,const HolisticShapeModel& train, float bestSoFar)
{
	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << "Entered HolisticRecognizer::holisticDistance"  << endl;

	float distance=0.0f;		// distance between testVec and floatVec

	int loopIndex;				// Index used for looping
	
	// check and validate the test vector size

	if (test.size()!=1)
	{
		throw LTKException(ECOMPUTE_DISTANCE_ERROR);
	}

	floatVector testVec = test.at(0);

	// check and validate the train vector size
	
	if ((train.getModel()).size()!=1)
	{
		throw LTKException(ECOMPARISON_ERROR);
	}
	
	floatVector trainVec = (train.getModel()).at(0);

	int vecSize = testVec.size();

	// check and validate the train & test vector sizes

	if (vecSize!=trainVec.size())
	{
		throw LTKException(ETRAIN_TEST_VECTOR_SIZE_MISMATCH);
	}

	// calculating the euclidean distance 

	for(loopIndex = 0; loopIndex < vecSize; ++loopIndex)
	{
		distance += (testVec.at(loopIndex)-trainVec.at(loopIndex))*(testVec.at(loopIndex)-trainVec.at(loopIndex));
	}

	distance = sqrt(distance);

	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << "Exiting HolisticRecognizer::holisticDistance"  << endl;

	return distance;
}

/**********************************************************************************
* AUTHOR		: Mehul Patel
* DATE			: 09-MAR-2005
* NAME			: ~HolisticRecognizer
* DESCRIPTION	: destructor
* ARGUMENTS		: 
* RETURNS		: 
* NOTES			:
* CHANGE HISTROY
* Author			Date				Description of change
*************************************************************************************/

HolisticRecognizer::~HolisticRecognizer()
{
	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << "Entered destructor of HolisticRecognizer"  << endl;

	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << "Exiting destructor of HolisticRecognizer"  << endl;
}

/**********************************************************************************
* AUTHOR		: Dinesh M.
* DATE			: 14-DEC-2005
* NAME			: setDeviceContext
* DESCRIPTION	: New Function - Not yet added
* ARGUMENTS		: 
* RETURNS		: 
* NOTES			:
* CHANGE HISTROY
* Author			Date				Description of change
*************************************************************************************/
int HolisticRecognizer::setDeviceContext(LTKCaptureDevice& deviceinfo)
{
	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << "Entered HolisticRecognizer : setDeviceContext"  << endl;

	m_captureDevice = deviceinfo; 

	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << "Exiting HolisticRecognizer : setDeviceContext"  << endl;

	return SUCCESS;
}

/**********************************************************************************
* AUTHOR		: Dinesh M.
* DATE			: 14-DEC-2005
* NAME          : getLastError
* DESCRIPTION   : returns last occured error description.
* ARGUMENTS     : - NO -
* RETURNS       : pointer to an error description 
* NOTES         :
* CHANGE HISTROY
* Author            Date                Description of change
*************************************************************************************/
int HolisticRecognizer::getLastError()
{
	return errorCode;
}

int HolisticRecognizer::unloadModelData()
{
	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << "Entered HolisticRecognizer : unloadModelData"  << endl;

	m_referenceModels.clear();
	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << "Exiting HolisticRecognizer : unloadModelData"  << endl;

	return SUCCESS ;
}

/**********************************************************************************
* AUTHOR		: Dinesh M.
* DATE			: 14-DEC-2005
* NAME			: setPreprocMap
* DESCRIPTION	: This function addes the function adderss to the preprecssor maping 
				  variable.
* ARGUMENTS		: 
* RETURNS		: 
* NOTES			:
* CHANGE HISTROY
* Author			Date				Description of change
*************************************************************************************/

/*void HolisticRecognizer::setPreprocMap()
{
	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << "Entered PCAShapeRecognizer : setPreprocMap"  << endl;

	string resample(RESAMPLE_TRACE_GROUP_FUNC);
	string preprocess(PREPROCESS_FUNC);

	PreprocMap[resample] = &HolisticRecognizer::resampleTraceGroup;

	//PreprocMap[preprocess] = &HolisticRecognizer::preprocess;

	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << "Exiting HolisticShapeRecognizer : setPreprocMap"  << endl;
}*/

/**********************************************************************************
* AUTHOR		: Dinesh M.
* DATE			: 14-DEC-2005
* NAME			: getShapeRecoConfidences
* DESCRIPTION	: calls the corresponding getShapeRecoConfidences method of the derived class
*				  this abstract class is pointing to
* ARGUMENTS		: traceGroup - trace group to be recognized
*				  screenContext - screen context
*				  captureDevice - device information
*				  shapeIds - A subset of the entire shape space for which shape recongizer 
*                            confidences need to be computed
*				  shapeRecoConfidences - the confidence values calculated by the recognizer
* RETURNS		: SUCCESS on successfully evaluating the confidences
* NOTES			:
* CHANGE HISTROY
* Author			Date				Description of change
*************************************************************************************/
int HolisticRecognizer::getShapeRecoConfidences(const LTKTraceGroup& traceGroup, 
		const LTKScreenContext& screenContext,
		const vector<int>& shapeIds, vector<float>& shapeRecoConfidences)
{
	return FAILURE;
}
/**********************************************************************************
* AUTHOR        : Vijayakumara M 
* DATE          : 20-Sept-2005
* NAME          : mapPreprocFunctions
* DESCRIPTION   : Maps the module name and its function names in the preprocessing 
				  sequence.
* ARGUMENTS     : - NO -
* RETURNS       : SUCCESS on successful, errorNumbers on Failure.
* NOTES         :
* CHANGE HISTROY
* Author            Date                Description of change
*************************************************************************************/
/*int HolisticRecognizer::mapPreprocFunctions()
{
	stringStringMap preProcSequence;

	MapModFunc tmpMap;

	string preProcSeq;
	vector<string> moduleFuncNames;
	vector<string> modFuncs;
	vector<string> funcNameTokens;	
	
	string module, funName, sequence;
	string::size_type indx;

	LTKTraceGroup local_inTraceGroup;

	preProcSeq = shapeRecognizerProperties[PREPROCSEQUENCE];

	if(preProcSeq == "")
	{
		preProcSeq = PREPROC_DEF_SEQ_PCA;
	}

	LTKStringUtil::tokenizeString(preProcSeq,  DELEMITER_SEQUENCE,  funcNameTokens);

	if(funcNameTokens.size() == 0)
	{
		preProcSeq = PREPROC_DEF_SEQ_PCA;
		LTKStringUtil::tokenizeString(preProcSeq,  DELEMITER_SEQUENCE,  funcNameTokens);
	}

	for (indx = 0; indx < funcNameTokens.size(); indx++)
	{
		moduleFuncNames.push_back(funcNameTokens.at(indx));
	}

	for(indx=0; indx < moduleFuncNames.size(); indx++)
	{
		sequence = moduleFuncNames.at(indx);

		LTKStringUtil::tokenizeString(sequence,  DELEMITER_FUNC,  modFuncs);

		if(modFuncs.size() >= 2)
		{
			module = modFuncs.at(0);
			
			funName =  modFuncs.at(1);
			
			if(!module.compare("PCA"))
			{
				if(PreprocMap[funName] != NULL)
				{
					tmpMap.moduleName = module;
					tmpMap.funcName = funName;
					mapSequence.push_back(tmpMap);				
				}
				else
				{
					LTKReturnError(EINVALID_PREPROC_SEQUENCE);	
				}
			}
			else
			{
				if(!module.compare("CommonPreProc"))
				{
					FN_PTR_PREPROCESSOR pPreprocFunc = NULL;
					pPreprocFunc = ltkShapeRecPtr->getPreprocptr(funName);
					if(pPreprocFunc!= NULL)
					{
						tmpMap.moduleName = module;
						tmpMap.funcName = funName;
						mapSequence.push_back(tmpMap);
					}
					else
					{
						LTKReturnError(EINVALID_PREPROC_SEQUENCE);	
					}
				}
				else
				{
					LTKReturnError(EINVALID_PREPROC_SEQUENCE);	
				}
			}
		}
		else
		{
			LTKReturnError(EINVALID_PREPROC_SEQUENCE);	
		}
	}
	

    return SUCCESS;
}*/


void HolisticRecognizer::assignDefaultValues()
{
	//Assigning all factory defaults...
	
	m_numNNs = PREPROC_DEF_NUMNNS;
	m_worstCaseValue = PREPROC_DEF_WORSTCASE_VALUE;
	m_featureExtractorObj.setNumResamplePoints1(PREPROC_DEF_NUM_RESAMPLE_POINTS1);
	m_featureExtractorObj.setNumResamplePoints2(PREPROC_DEF_NUM_RESAMPLE_POINTS2);
	m_featureExtractorObj.setSmoothFilterLength1(PREPROC_DEF_SMOOTHFILTER_LENGTH1);
	m_featureExtractorObj.setSmoothFilterLength2(PREPROC_DEF_SMOOTHFILTER_LENGTH2);
	m_featureExtractorObj.setSwAngNormFactor(PREPROC_DEF_SWANG_NORM_FACTOR);
	m_featureExtractorObj.setSwAngHookThresh(PREPROC_DEF_SWANG_HOOK_THRESHOLD);
	m_featureExtractorObj.setHorizStrokeAngleThresh(PREPROC_DEF_HOR_STROKE_ANGLE_THRESHOLD);
	m_featureExtractorObj.xChannelstr = "X";
	m_featureExtractorObj.yChannelstr = "Y";
	ltkShapeRecPtr->setNormalizedSize(PREPROC_DEF_NORMALIZEDSIZE);
	ltkShapeRecPtr->setHookLengthThreshold1(PREPROC_DEF_HOOKLENGTH_THRESHOLD1);
	ltkShapeRecPtr->setHookLengthThreshold2(PREPROC_DEF_HOOKLENGTH_THRESHOLD2);
	ltkShapeRecPtr->setHookAngleThreshold(PREPROC_DEF_HOOKANGLE_THRESHOLD);
}

int HolisticRecognizer::readClassifierConfig()
{
	int tempIntegerVar = 0;
	float tempFloatVar = 0;
	string tempStringVar = "";

	//	reading Holistic configuration file
	LTKConfigFileReader* shapeRecognizerProperties = NULL;

	try
	{
		shapeRecognizerProperties = new LTKConfigFileReader(m_holisticCfgFile);
	}
	catch(LTKException e)
	{
		delete shapeRecognizerProperties;
		/*	We use the default values in this case	*/
		return SUCCESS;
	}

	/*if((errorCode = mapPreprocFunctions()) != SUCCESS)
	{
		if(ltkShapeRecPtr != NULL)
		{
			deleteLTKLipiPreProcessor(ltkShapeRecPtr);
			ltkShapeRecPtr = NULL;
		}
		if(m_libHandler != NULL)
		{
			LTKUnloadDLL(m_libHandler);
			m_libHandler = NULL;
		}
		return errorCode;
	}*/ 
	
	//getting the Holistic shape recognizer configuration
	tempStringVar = shapeRecognizerProperties->getConfigValue(NUMNNS);
	if(tempStringVar != "")
	{
		tempIntegerVar = atoi(tempStringVar.c_str());
		if(tempIntegerVar > 0)
		{
			m_numNNs = tempIntegerVar;
			LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << "m_numNNs = " << m_numNNs << endl;
		}
		else
		{
			LOG( LTKLogger::LTK_LOGLEVEL_ERR) << " TRACEDIMENSION is out of permitted range"  << endl;
			delete shapeRecognizerProperties;
			LTKReturnError(ENUM_NNS);	
		}

	}

	tempStringVar = "";
	tempStringVar = shapeRecognizerProperties->getConfigValue(WORSTCASEVALUE);
	if(tempStringVar != "")
	{
		m_worstCaseValue = atof(tempStringVar.c_str());

		LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << "m_worstCaseValue = " << m_worstCaseValue << endl;
	}
	
	//	initializing the feature extractor object

	tempStringVar = "";
	tempStringVar = shapeRecognizerProperties->getConfigValue(NUMRESAMPLEPOINTS1);
	if(tempStringVar != "")
	{
		m_featureExtractorObj.setNumResamplePoints1(atoi(tempStringVar.c_str()));

		LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << "numResamplePoints1 = " << m_featureExtractorObj.getNumResamplePoints1() << endl;
	}

	tempStringVar = "";
	tempStringVar = shapeRecognizerProperties->getConfigValue(NUMRESAMPLEPOINTS2);
	if(tempStringVar != "")
	{
		m_featureExtractorObj.setNumResamplePoints2(atoi(tempStringVar.c_str()));

		LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << "numResamplePoints2 = " << m_featureExtractorObj.getNumResamplePoints2() << endl;
	}

	tempStringVar = "";
	tempStringVar = shapeRecognizerProperties->getConfigValue(SMOOTHFILTERLENGTH1);
	if(tempStringVar != "")
	{
		m_featureExtractorObj.setSmoothFilterLength1(atoi(tempStringVar.c_str()));

		LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << "SmoothFilterLength1 = " << m_featureExtractorObj.getSmoothFilterLength1() << endl;
	}

	tempStringVar = "";
	tempStringVar = shapeRecognizerProperties->getConfigValue(SMOOTHFILTERLENGTH2);
	if(tempStringVar != "")
	{
		m_featureExtractorObj.setSmoothFilterLength2(atoi(tempStringVar.c_str()));

		LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << "SmoothFilterLength2 = " << m_featureExtractorObj.getSmoothFilterLength2() << endl;
	}

	tempStringVar = "";
	tempStringVar = shapeRecognizerProperties->getConfigValue(SWANGNORMFACTOR);
	if(tempStringVar != "")
	{
		m_featureExtractorObj.setSwAngNormFactor(atof(tempStringVar.c_str()));

		LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << "SwAngNormFactor = " << m_featureExtractorObj.getSwAngNormFactor() << endl;
	}

	tempStringVar = "";
	tempStringVar = shapeRecognizerProperties->getConfigValue(SWANGHOOKTHRESH);
	if(tempStringVar != "")
	{
		m_featureExtractorObj.setSwAngHookThresh(atoi(tempStringVar.c_str()));

		LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << "SwAngHookThresh = " << m_featureExtractorObj.getSwAngHookThresh() << endl;
	}

	tempStringVar = "";
	tempStringVar = shapeRecognizerProperties->getConfigValue(HORIZSTROKEANGLETHRESH);
	if(tempStringVar != "")
	{
		m_featureExtractorObj.setHorizStrokeAngleThresh(atoi(tempStringVar.c_str()));

		LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << "HorizStrokeAngleThresh = " << m_featureExtractorObj.getHorizStrokeAngleThresh() << endl;
	}
	
	// initializing the preProcessor object

	tempStringVar = "";
	tempStringVar = shapeRecognizerProperties->getConfigValue(NORMALIZEDSIZE);
	if(tempStringVar != "")
    {
        ltkShapeRecPtr->setNormalizedSize(atof(tempStringVar.c_str()));

        LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << "NormalizedSize = " + tempStringVar << endl;
    }

	tempStringVar = "";
	tempStringVar = shapeRecognizerProperties->getConfigValue(HOOKLENGTHTHRESHOLD1);
    if(tempStringVar != "")
    {
        ltkShapeRecPtr->setHookLengthThreshold1(atof(tempStringVar.c_str()));

        LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << "Hook length threshold 1 = " + tempStringVar << endl;
    }

	tempStringVar = "";
	tempStringVar = shapeRecognizerProperties->getConfigValue(HOOKLENGTHTHRESHOLD2);
    if(tempStringVar != "")
    {
        ltkShapeRecPtr->setHookLengthThreshold2(atof(tempStringVar.c_str()));

        LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << "Hook length threshold 2 = " + tempStringVar << endl;
    }

	tempStringVar = "";
	tempStringVar = shapeRecognizerProperties->getConfigValue(HOOKANGLETHRESHOLD);
    if(tempStringVar != "")
    {
        ltkShapeRecPtr->setHookAngleThreshold(atof(tempStringVar.c_str()));

        LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << "Hook angle threshold = " + tempStringVar << endl;
    }
	
	tempStringVar = "";
	tempStringVar = shapeRecognizerProperties->getConfigValue(CONFIDENCEMAPPINGFILE);
	if(tempStringVar != "")
	{
		m_confMapFile += tempStringVar;
		LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << " confidence mapping file" +m_confMapFile << endl;
	}
	else
	{
		m_confMapFile = "";
	}

	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << "Exiting HolisticRecognizer::InitializeShapeRecognizer"  << endl;

	delete shapeRecognizerProperties;
	return SUCCESS;

}
