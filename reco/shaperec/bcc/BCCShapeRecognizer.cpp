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
 * FILE DESCR: Implementation for BCC Shape Recognition module
 *
 * CONTENTS:
 *  loadTrainingData
 *  recognize
 *  learn
 *  train
 *  jacobi
 *  getPointAllocStats
 *  resampleTraceGroup
 *  preprocess
 *
 * AUTHOR:     Balaji R.
 *
 * DATE:       December 23, 2004
 * CHANGE HISTORY:
 * Author       Date            Description
 ************************************************************************/

#include "LTKTypes.h"

#include "LTKShapeRecognizer.h"

#include "BCCShapeRecognizer.h"

#include "LTKConfigFileReader.h"

#include "LTKStringUtil.h"

#include "LTKInkFileReader.h"

#include "LTKLoggerUtil.h"

#include "LTKException.h"

#include "LTKErrorsList.h"

#include "LTKPreprocDefaults.h"

#include "lipiengine.h"
/**********************************************************************************
* AUTHOR        : Balaji R.
* DATE          : 23-DEC-2004
* NAME          : BCCShapeRecognizer
* DESCRIPTION   : Default Constructor
* ARGUMENTS     : 
* RETURNS       : 
* NOTES         :
* CHANGE HISTROY
* Author            Date                Description
*************************************************************************************/

BCCShapeRecognizer::BCCShapeRecognizer()
{
    LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << "Entered default constructor of BCCShapeRecognizer" << endl;
	m_hPCA = NULL;
	m_hHolistic = NULL;
	m_pPCAObj = NULL;
	m_pHolisticObj = NULL;
	shapeRecognizerProperties = NULL;

    LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << "Exiting default constructor of BCCShapeRecognizer" << endl;
}

/**********************************************************************************
* AUTHOR        : Balaji R.
* DATE          : 23-DEC-2004
* NAME          : BCCShapeRecognizer
* DESCRIPTION   : Initialization Constructor
* ARGUMENTS     : 
* RETURNS       : 
* NOTES         :
* CHANGE HISTROY
* Author            Date                Description
*************************************************************************************/

/*BCCShapeRecognizer::BCCShapeRecognizer(LTKShapeRecoConfig& config) : LTKShapeRecognizer(BCC), m_pcaShapeRecognizer(config), m_holisticShapeRecognizer(config)
{
    LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << "Entered default constructor of BCCShapeRecognizer"  << endl;

    //  initializing number of shapes in the shape set

    m_numShapes = config.getNumShapes();

    //  composing the bcc configuration file name

    this->m_bccCfgFile = config.getLipiRoot() + SEPARATOR + config.getShapeSet() + SEPARATOR + 
             config.getProfile() + SEPARATOR + m_shapeRecognizerName + CONFIGFILEEXT;

    LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << "BCC Configuration File is: "<< m_bccCfgFile  << endl;

    LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << "Exiting default constructor of BCCShapeRecognizer"  << endl;
}*/

/*int BCCShapeRecognizer::initializeShapeRecognizer()
{

    LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << "Entered BCCShapeRecognizer::InitializeBCCShapeRecognizer"  << endl;

    stringStringMap shapeRecognizerProperties;  //  contains details for configuration of the shape recognizer

    //  reading bcc configuration file

    shapeRecognizerProperties = LTKConfigFileReader::getMap(m_bccCfgFile);

    //  initializing the variables
    
    m_numChoices = PREPROC_DEF_NUM_CHOICES;

	m_combinationScheme = PREPROC_DEF_COMBINATION;

    //  getting the preprocessing steps required

    if(shapeRecognizerProperties[NUMCHOICES] != "")
    {
        m_numChoices = atoi((shapeRecognizerProperties[NUMCHOICES]).c_str());

        LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << "Number of Choices = " << m_numChoices  << endl;
    }

    if(shapeRecognizerProperties[COMBINATIONSCHEME] != "")
    {
        m_combinationScheme = shapeRecognizerProperties[COMBINATIONSCHEME];

        LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << "Combination Scheme = " << m_combinationScheme  << endl;
    }

	// initializing the shape recognizers

	m_pcaShapeRecognizer.initializeShapeRecognizer();

	m_holisticShapeRecognizer.initializeShapeRecognizer();

    LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << "Exiting BCCShapeRecognizer::InitializeBCCShapeRecognizer"  << endl;

    return SUCCESS;

}*/

/**********************************************************************************
* NAME          : initialize
* DESCRIPTION   : initialize using project name and profile name
* ARGUMENTS     : strProjectName - project name string
*				  strProfileName - profile name string
* RETURNS       : 
* NOTES         :
* CHANGE HISTROY
* Author            Date                Description
* 
*************************************************************************************/
int BCCShapeRecognizer::initialize(string& strProjectName, string &strProfileName)
{
    LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << "Entered BCCShapeRecognizer::initialize" << endl;

	int iResult;
	string temp;
	char *envpath;
	string tempStr;
	string strLipiRootPath;
	LTKConfigFileReader *projectCfgAttrs = NULL;  //  contains details for configuration of the shape recognizer
	string valueFromCFG = "";
	int tempIntegerVar;
	float tempFloatVar;
	headerInfo[PROJNAME] = strProjectName;

	// Read the LIPI_ROOT environment variable
	envpath = string(getLipiPath());
	
	if(envpath == NULL)
	{
		LOG( LTKLogger::LTK_LOGLEVEL_ERR) << getError(ELIPI_ROOT_PATH_NOT_SET) << endl;
		LTKReturnError(ELIPI_ROOT_PATH_NOT_SET);	// PATH not set
	}

	strLipiRootPath = envpath;

	m_bccCfgFile = strLipiRootPath + PROJECTS_PATH_STRING + strProjectName + PROFILE_PATH_STRING + strProfileName + SEPARATOR + BCC + CONFIGFILEEXT;

	// Read number of shapes from project.cfg
	tempStr = strLipiRootPath + PROJECTS_PATH_STRING + strProjectName + PROFILE_PATH_STRING + PROJECT_CFG_STRING;
	

	try
	{
		projectCfgAttrs = new LTKConfigFileReader(tempStr.c_str());
	}
	catch(LTKException e)
	{
		delete projectCfgAttrs;
		LOG( LTKLogger::LTK_LOGLEVEL_ERR) << e.getExceptionMessage() << endl;

		LTKReturnError(ECONFIG_FILE_OPEN);
	}

	valueFromCFG = projectCfgAttrs->getConfigValue(PROJECT_CFG_ATTR_NUMSHAPES_STR);
	if(valueFromCFG != "")
	{
		m_numShapes = atoi(valueFromCFG.c_str());
		headerInfo[NUMSHAPES] = valueFromCFG;
	}
	else
	{
		m_numShapes = 0;	
	}

	if(m_numShapes <= 0)
	{
		LOG( LTKLogger::LTK_LOGLEVEL_ERR) << 
            "Invalid value for number of shapes : " << m_numShapes << endl;

		LTKReturnError(EINVALID_NUM_OF_SHAPES);
	}


	// Now load PCA.DLL
	temp = strLipiRootPath + SEPARATOR + "lib" + SEPARATOR + "PCA.DLL";
	m_hPCA = LTKLoadDLL(temp.c_str());
	if(m_hPCA == NULL)
	{
	//	errorCode = ELOAD_SHAPEREC_DLL_ERR;
		errorCode = ELOAD_SHAPEREC_DLL;
		LOG( LTKLogger::LTK_LOGLEVEL_ERR) << 
            getError(errorCode) << ":" << temp.c_str()  << endl;
		return -1; // Unable to load dll
	}

	// Now load DTW.DLL
	temp = strLipiRootPath + SEPARATOR + "lib" + SEPARATOR + "holistic.DLL";
	m_hHolistic = LTKLoadDLL(temp.c_str());
	if(m_hHolistic == NULL)
	{
		LTKUnloadDLL(m_hPCA);
		//errorCode = ELOAD_SHAPEREC_DLL_ERR;
			errorCode = ELOAD_SHAPEREC_DLL;
		LOG( LTKLogger::LTK_LOGLEVEL_ERR) << 
            getError(errorCode) << ":" << temp.c_str() << endl;
		return -1; // Unable to load dll
	}

	// Map PCA & DTW DLL functions...
	if(mapPCAAndHolisticModuleFunctions() != 0)
	{
		LTKUnloadDLL(m_hPCA);
		LTKUnloadDLL(m_hHolistic);
		return -1; // Unable to map the functions
	}

	// Create PCAShapeRecognizer object
	m_pPCAObj = module_createShapeRecognizerPCA();
	if(m_pPCAObj == NULL)
	{
		//errorCode = ECREATE_SHAPEREC_ERR;
		errorCode = ECREATE_SHAPEREC;

		LTKUnloadDLL(m_hPCA);
		LTKUnloadDLL(m_hHolistic);
		LOG( LTKLogger::LTK_LOGLEVEL_ERR) << getError(errorCode) << endl;
		return -1; /* Error, unable to create shape recognizer instance */
	}

	// Create DTWShapeRecognizer object
	m_pHolisticObj = module_createShapeRecognizerHolistic();
	if(m_pHolisticObj == NULL)
	{
		module_deleteShapeRecognizerPCA(m_pPCAObj);
		//errorCode = ECREATE_SHAPEREC_ERR;
		errorCode =ECREATE_SHAPEREC;
		LTKUnloadDLL(m_hPCA);
		LTKUnloadDLL(m_hHolistic);
		LOG( LTKLogger::LTK_LOGLEVEL_ERR) << getError(errorCode) << endl;
		return -1; /* Error, unable to create shape recognizer instance */
	}


	try
	{
		shapeRecognizerProperties = new LTKConfigFileReader(m_bccCfgFile);
	}
	catch(LTKException e)
	{
		/*
		We use the default values in this case
		*/
	}

	m_numChoices = PREPROC_DEF_NUM_CHOICES;

	m_combinationScheme = PREPROC_DEF_COMBINATION;

    //  getting the preprocessing steps required
	valueFromCFG = "";
	valueFromCFG = shapeRecognizerProperties->getConfigValue(NUMCHOICES);
    if(valueFromCFG != "")
    {
        m_numChoices = atoi(valueFromCFG.c_str());

        LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << 
            "Number of Choices = "  << valueFromCFG << endl;
    }

    if(shapeRecognizerProperties[COMBINATIONSCHEME] != "")
    {
        m_combinationScheme = shapeRecognizerProperties[COMBINATIONSCHEME];

        LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << 
            "Combination Scheme = " << m_combinationScheme << endl;
    }


	// Now initialize the recognizers...
	iResult = m_pPCAObj->initialize(strProjectName, strProfileName);
	if(iResult != 0)
	{
		module_deleteShapeRecognizerPCA(m_pPCAObj);
		module_deleteShapeRecognizerHolistic(m_pHolisticObj);
		//errorCode = ECREATE_SHAPEREC_ERR;
		errorCode =ECREATE_SHAPEREC;
		LTKUnloadDLL(m_hPCA);
		LTKUnloadDLL(m_hHolistic);
		LOG( LTKLogger::LTK_LOGLEVEL_ERR) << getError(errorCode) << endl;
		return -1; /* Error, unable to create shape recognizer instance */
	}

	iResult = m_pHolisticObj->initialize(strProjectName, strProfileName);
	cout << "strProfileName = " << strProjectName <<" = "  << strProfileName<<endl;

	if(iResult != 0)
	{
		module_deleteShapeRecognizerPCA(m_pPCAObj);
		module_deleteShapeRecognizerHolistic(m_pHolisticObj);
		//errorCode = ECREATE_SHAPEREC_ERR;
		errorCode =ECREATE_SHAPEREC;
		LTKUnloadDLL(m_hPCA);
		LTKUnloadDLL(m_hHolistic);
		LOG( LTKLogger::LTK_LOGLEVEL_ERR) << getError(errorCode) << endl;
		return -1; /* Error, unable to create shape recognizer instance */
	}

    LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << "Exiting ABCShapeRecognizer::initialize" << endl;

	delete projectCfgAttrs;
    return SUCCESS;
}

/**********************************************************************************
* AUTHOR        : Dinesh M
* DATE          : 19-DEC-2005
* NAME          : mapPCAAndHolisticModuleFunctions
* DESCRIPTION   : 
* ARGUMENTS     : 
* RETURNS       : 
* NOTES         :
* CHANGE HISTROY
* Author            Date                Description
*************************************************************************************/

int BCCShapeRecognizer::mapPCAAndHolisticModuleFunctions()
{
	module_createShapeRecognizerPCA = NULL;
	module_createShapeRecognizerPCA = (FN_PTR_CREATESHAPERECOGNIZER) LTKGetDLLFunc(m_hPCA, CREATESHAPERECOGNIZER_FUNC_NAME);
	if(module_createShapeRecognizerPCA == NULL)
	{
		LOG( LTKLogger::LTK_LOGLEVEL_ERR) << 
            "Exported function not found in module : createShapeRecognizer"  << endl;

		//LTKReturnError(EDLL_FUNC_ADDRESS_ERR);
		LTKReturnError(EDLL_FUNC_ADDRESS);
	}

	module_deleteShapeRecognizerPCA = (FN_PTR_DELETESHAPERECOGNIZER) LTKGetDLLFunc(m_hPCA, DELETESHAPERECOGNIZER_FUNC_NAME);
	if(module_deleteShapeRecognizerPCA == NULL)
	{
		LOG( LTKLogger::LTK_LOGLEVEL_ERR) << "Exported function not found in module: deleteShapeRecognizer" << endl;

	//LTKReturnError(EDLL_FUNC_ADDRESS_ERR);
		LTKReturnError(EDLL_FUNC_ADDRESS);
		// ERROR: Unable to link with deleteShapeRecognizer function in module */

	}

	module_createShapeRecognizerHolistic = NULL;
	module_createShapeRecognizerHolistic = (FN_PTR_CREATESHAPERECOGNIZER) LTKGetDLLFunc(m_hHolistic, CREATESHAPERECOGNIZER_FUNC_NAME);
	if(module_createShapeRecognizerHolistic == NULL)
	{
		LOG( LTKLogger::LTK_LOGLEVEL_ERR) << 
            "Exported function not found in module : createShapeRecognizer"  << endl;

		//LTKReturnError(EDLL_FUNC_ADDRESS_ERR);
		LTKReturnError(EDLL_FUNC_ADDRESS);
	}

	module_deleteShapeRecognizerHolistic = (FN_PTR_DELETESHAPERECOGNIZER) LTKGetDLLFunc(m_hHolistic, DELETESHAPERECOGNIZER_FUNC_NAME);
	if(module_deleteShapeRecognizerHolistic == NULL)
	{
		LOG( LTKLogger::LTK_LOGLEVEL_ERR) << 
            "Exported function not found in module : deleteShapeRecognizer"  << endl;

		//LTKReturnError(EDLL_FUNC_ADDRESS_ERR);
		LTKReturnError(EDLL_FUNC_ADDRESS);
		// ERROR: Unable to link with deleteShapeRecognizer function in module */

	}

	return SUCCESS;
}

/**********************************************************************************
* AUTHOR        : Balaji R.
* DATE          : 23-DEC-2004
* NAME          : loadTrainingData
* DESCRIPTION   : loads the reference model file into memory
* ARGUMENTS     : 
* RETURNS       : SUCCESS on successful loading of the reference model file
* NOTES         :
* CHANGE HISTROY
* Author            Date                Description
*************************************************************************************/

int BCCShapeRecognizer::loadModelData()
{
	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << 
        "Entered BCCShapeRecognizer::loadModelData" << endl;

	int iLoadDataResult = -1;

	iLoadDataResult = m_pPCAObj->loadModelData();
	if(iLoadDataResult != SUCCESS)
	{
		return -1;
	}

	iLoadDataResult = m_pHolisticObj->loadModelData();
	if(iLoadDataResult != SUCCESS)
	{
		return -1;
	}

    LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << 
        "Exiting BCCShapeRecognizer::loadModelData" << endl;

    return SUCCESS;
}

/**********************************************************************************
* AUTHOR        : Balaji R.
* DATE          : 23-DEC-2004
* NAME          : recognize
* DESCRIPTION   : recognizes the incoming tracegroup
* ARGUMENTS     : traceGroup - trace group to be recognized
*                 uiParams - screen context
*                 deviceParams - device information
*                 shapeSubSet - possible output space of the recognition result
*                 confThreshold - shapes with confidence below this threshold are rejected
*                 numChoices - maximum number of choices to be returned
*                 resultVec - result of recognition
* RETURNS       : SUCCESS on successful running of the code
* NOTES         :
* CHANGE HISTROY
* Author            Date                Description
*************************************************************************************/

int BCCShapeRecognizer::recognize(const LTKTraceGroup& traceGroup, const LTKScreenContext& screenContext, 
    const vector<bool>& shapeSubSet, float confThreshold, 
    int  numChoices, vector<LTKShapeRecoResult>& resultVec)
{
    LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << 
        "Entered BCCShapeRecognizer::recognize"  << endl;

	vector<LTKShapeRecoResult> pcaResults;

	floatVector bordaCounts(m_numShapes, 0.0f);

	int resultIndex;

	vector<LTKShapeRecoResult> holisticResults;

	//resultVec.clear();

	pcaResults.reserve(numChoices +1);
	holisticResults.reserve(numChoices +1);

	int iRecognizeResult = -1;

	iRecognizeResult = m_pPCAObj->recognize(traceGroup, screenContext, shapeSubSet, confThreshold, numChoices, pcaResults);
	if(iRecognizeResult != SUCCESS)
	{
		LOG( LTKLogger::LTK_LOGLEVEL_ERR) << 
            "Error while recognizing using PCA" << endl;
		return -1;
	}

	iRecognizeResult = m_pHolisticObj->recognize(traceGroup, screenContext, shapeSubSet, confThreshold, numChoices, holisticResults);
	if(iRecognizeResult != SUCCESS)
	{
		LOG( LTKLogger::LTK_LOGLEVEL_ERR) << 
            "Error while recognizing using Holisitc" << endl;
		return -1;
	}

	LOG( LTKLogger::LTK_LOGLEVEL_ERR) << 
        "Got results from both PCA and Holistic" << endl;
	
	if(m_combinationScheme == "sum") //sum rule
	{
		for(resultIndex = 0; resultIndex < pcaResults.size(); ++resultIndex)
		{
			if(pcaResults[resultIndex].getShapeId() >=0 && pcaResults[resultIndex].getShapeId() < m_numShapes)
			{
				bordaCounts[pcaResults[resultIndex].getShapeId()] += pcaResults[resultIndex].getConfidence();
			}
		}


		for(resultIndex = 0; resultIndex < holisticResults.size(); ++resultIndex)
		{
			if(holisticResults[resultIndex].getShapeId() >=0 && holisticResults[resultIndex].getShapeId() < m_numShapes)
			{
				bordaCounts[holisticResults[resultIndex].getShapeId()] += holisticResults[resultIndex].getConfidence();
			}
		}

	}
	else//default is borda count combination
	{
		for(resultIndex = 0; resultIndex < pcaResults.size(); ++resultIndex)
		{
			//cout << pcaResults[resultIndex].getShapeId() << endl;

			if(pcaResults[resultIndex].getShapeId() >=0 && pcaResults[resultIndex].getShapeId() < m_numShapes)
			{
				bordaCounts[pcaResults[resultIndex].getShapeId()] += (m_numShapes - resultIndex);
			}
		}


		for(resultIndex = 0; resultIndex < holisticResults.size(); ++resultIndex)
		{
			//cout << holisticResults[resultIndex].getShapeId() << endl;

			if(holisticResults[resultIndex].getShapeId() >=0 && holisticResults[resultIndex].getShapeId() < m_numShapes)
			{
				bordaCounts[holisticResults[resultIndex].getShapeId()] += (m_numShapes - resultIndex);
			}
		}
	}

	int maxInd;

	float maxValue;

	int classIndex, newOrderIndex;

	for(newOrderIndex = 0; newOrderIndex < numChoices ; ++newOrderIndex)
	{
		maxValue = -1;

		maxInd = -1;

		for(classIndex = 0; classIndex < m_numShapes; ++classIndex)
		{
			if(bordaCounts.at(classIndex) > maxValue)
			{
				maxValue = bordaCounts.at(classIndex);

				maxInd = classIndex;
			}

		}

		//cout << "reordered" << endl;

		if(shapeSubSet.empty() || shapeSubSet.at(maxInd) == true)
		{
			LTKShapeRecoResult newResult(maxInd, (float)maxValue);

			resultVec.push_back(newResult);
		}

		//cout << "result pushed" << endl;

		bordaCounts.at(maxInd) = -1;
	}

	return SUCCESS;
}

/**********************************************************************************
* AUTHOR		: Deepu V.
* DATE			: 11-APR-2005
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
* Author			Date				Description
*************************************************************************************/

int BCCShapeRecognizer::getShapeRecoConfidences(const LTKTraceGroup& traceGroup, 
		const LTKScreenContext& screenContext, const LTKCaptureDevice& captureDevice,
		const vector<int>& shapeIds, vector<float>& shapeRecoConfidences)
{
	
    LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << 
        "Entered BCCShapeRecognizer::getShapeRecoConfidences" << endl;

    LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << 
        "Exiting BCCShapeRecognizer::getShapeRecoConfidences" << endl;

	return SUCCESS;
}
    
/**********************************************************************************
* AUTHOR        : Balaji R.
* DATE          : 23-DEC-2004
* NAME          : learn
* DESCRIPTION   : on the fly adaptation
* ARGUMENTS     : prototype - prototype to be learned
*                 shapeId - label of the prototype to be learned
*                 uiParams - screen context
*                 deviceParams - device information
* RETURNS       : SUCCESS on successful learning
* NOTES         :
* CHANGE HISTROY
* Author            Date                Description
*************************************************************************************/

int BCCShapeRecognizer::learn(const LTKTraceGroup& prototype, int shapeId, const LTKScreenContext& uiParams, 
                         const LTKCaptureDevice& deviceParams)
{
    LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << 
        "Entered BCCShapeRecognizer::learn" << endl;

    LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << 
        "Exiting BCCShapeRecognizer::learn" << endl;

    return SUCCESS;
}

/**********************************************************************************
* AUTHOR        : Balaji R.
* DATE          : 23-DEC-2004
* NAME          : train
* DESCRIPTION   : offline training module
* ARGUMENTS     : trainingList - name of the file containing the listing of the files to be used for
*                 training each of the shapes
* RETURNS       : SUCCESS on successful training
* NOTES         :
* CHANGE HISTROY
* Author            Date                Description
*************************************************************************************/

int BCCShapeRecognizer::train(const string& trainingList, string& strModelDataHeaderInfoFile, string &comment, string &dataset, const string &inFileType)
{
    int iTrainResult = -1;

	iTrainResult = m_pPCAObj->train(trainingList, strModelDataHeaderInfoFile, comment, dataset);
	if(iTrainResult != SUCCESS)
	{
		LOG( LTKLogger::LTK_LOGLEVEL_ERR) << 
            "Error while training using PCA"  << endl;
		return -1;
	}

	iTrainResult = m_pHolisticObj->train(trainingList, strModelDataHeaderInfoFile, comment, dataset);
	if(iTrainResult != SUCCESS)
	{
		LOG( LTKLogger::LTK_LOGLEVEL_ERR) << 
            "Error while training using Holistic" << endl;
		return -1;
	}

	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << 
        "Exiting BCCShapeRecognizer::train"  << endl;

    return SUCCESS;
}

/**********************************************************************************
* AUTHOR        : Balaji R.
* DATE          : 23-DEC-2004
* NAME          : ~BCCShapeRecognizer
* DESCRIPTION   : destructor
* ARGUMENTS     : 
* RETURNS       : 
* NOTES         :
* CHANGE HISTROY
* Author            Date                Description
*************************************************************************************/

BCCShapeRecognizer::~BCCShapeRecognizer()
{
     LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << 
        "Entered default destructor of BCCShapeRecognizer" << endl;

	if(m_pPCAObj != NULL)
		module_deleteShapeRecognizerPCA(m_pPCAObj);

	if(m_pHolisticObj != NULL)
		module_deleteShapeRecognizerPCA(m_pHolisticObj);

	if(m_hPCA != NULL)
		LTKUnloadDLL(m_hPCA);

	if(m_hHolistic != NULL)
		LTKUnloadDLL(m_hHolistic);


    LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << 
        "Exiting default destructor of BCCShapeRecognizer" << endl;
}

/**********************************************************************************
* NAME			: unloadModelData
* DESCRIPTION	: to unload the model reference data file which is in memory
* ARGUMENTS		: 
* RETURNS		: SUCCESS
* NOTES			:
* CHANGE HISTROY
* Author			Date				Description
* 
*************************************************************************************/
int BCCShapeRecognizer::unloadModelData()
{
	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << 
        "Entered BCCShapeRecognizer::unloadModelData"  << endl;

	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << 
        "Exiting BCCShapeRecognizer::unloadModelData"  << endl;
	
	return SUCCESS;
}

/**********************************************************************************
* NAME			: setDeviceContext
* DESCRIPTION	: Stores the device parameters in the class 
* ARGUMENTS		: deviceinfo - device parameters
* RETURNS		: SUCCESS
* NOTES			:
* CHANGE HISTROY
* Author			Date				Description
* 
*************************************************************************************/
int BCCShapeRecognizer::setDeviceContext(LTKCaptureDevice& deviceinfo)
{
	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << 
        "Entered ABCShapeRecognizer::setDeviceContext"  << endl;

	m_pPCAObj->setDeviceContext(deviceinfo);
	m_pHolisticObj->setDeviceContext(deviceinfo);
	
	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << 
        "Exiting ABCShapeRecognizer::setDeviceContext"  << endl;
	
	return SUCCESS;
}

/**********************************************************************************
* NAME			: getLastError
* DESCRIPTION	: 
* ARGUMENTS		: 
* RETURNS		: 
* NOTES			:
* CHANGE HISTROY
* Author			Date				Description
*************************************************************************************/
int BCCShapeRecognizer::getLastError()
{
	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << 
        "Entered ABCShapeRecognizer::getLastError"  << endl;

	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << 
        "Exiting ABCShapeRecognizer::getLastError"  << endl;
	
	return errorCode;
}

