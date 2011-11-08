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
 * $LastChangedDate: 2008-02-29 15:07:25 +0530 (Fri, 29 Feb 2008) $
 * $Revision: 437 $
 * $Author: sharmnid $
 *
 ************************************************************************/
/************************************************************************
 * FILE DESCR: Implementation for HMM Shape Recognition module
 *
 * CONTENTS:
 *
 * AUTHOR:     Saravanan R.
 *
 w
 * DATE:       January 23, 2004
 * CHANGE HISTORY:
 * Author       Date            Description of change
 ************************************************************************/

#include "LTKConfigFileReader.h"

#include "HMMShapeRecognizer.h"

#include "LTKClassifierDefaults.h"

#include "LTKPreprocessorInterface.h"

#include "LTKShapeFeatureExtractorFactory.h"

#include "LTKShapeFeatureExtractor.h"

#include "LTKShapeFeature.h"

#include "LTKVersionCompatibilityCheck.h"

#include "LTKLoggerUtil.h"




HMMShapeRecognizer::HMMShapeRecognizer(const LTKControlInfo& controlInfo):
m_OSUtilPtr(LTKOSUtilFactory::getInstance()),
m_libHandler(NULL),
m_libHandlerFE(NULL)
{
	LOG(LTKLogger::LTK_LOGLEVEL_DEBUG) << "Entering " <<
        "HMMShapeRecognizer::HMMShapeRecognizer()" << endl;

    LTKControlInfo tmpControlInfo=controlInfo;

    string strProjectName = "";
    string strProfileName = "";


    if( (tmpControlInfo.projectName).empty() )
    {
        throw LTKException(EINVALID_PROJECT_NAME);
    }
    if( (tmpControlInfo.lipiRoot).empty() )
    {
        throw LTKException(ELIPI_ROOT_PATH_NOT_SET);
    }

    if( (tmpControlInfo.profileName).empty() )
    {
        strProfileName = DEFAULT_PROFILE;
        tmpControlInfo.profileName = strProfileName;
    }

    if ( tmpControlInfo.toolkitVersion.empty() )
    {
        throw LTKException(ENO_TOOLKIT_VERSION);
    }



    assignDefaultValues();

    m_lipiRootPath = tmpControlInfo.lipiRoot;
    m_currentVersion = tmpControlInfo.toolkitVersion;
    strProjectName = tmpControlInfo.projectName;
    strProfileName = tmpControlInfo.profileName;


    //Holds the value of Number of Shapes in string format
    string strNumShapes = "";

    string strProfileDirectory = m_lipiRootPath + PROJECTS_PATH_STRING +
        strProjectName + PROFILE_PATH_STRING;

    //Holds the path of the Preproc.dll
    /*string preprocDllPath = m_lipiRootPath + SEPARATOR + "lib" + SEPARATOR +
        PREPROC + DLL_EXT;*/

    //Holds the path of the Project.cfg
    string projectCFGPath = strProfileDirectory + PROJECT_CFG_STRING;

    // Config file

    m_hmmCfgFilePath = m_lipiRootPath + PROJECTS_PATH_STRING +
        (tmpControlInfo.projectName) + PROFILE_PATH_STRING +
        (tmpControlInfo.profileName) + SEPARATOR +
        HMM + CONFIGFILEEXT;


	ifstream doesFileExist;

	//Set the path for hmm.mdt
    m_hmmMDTFilePath = strProfileDirectory + strProfileName + SEPARATOR + HMM + DATFILEEXT;


	doesFileExist.open(m_hmmMDTFilePath.c_str());

	if(!doesFileExist)
	{
		LOG(LTKLogger::LTK_LOGLEVEL_ERR) << "Error: " << EFILE_OPEN_ERROR << " " <<
            "HMMShapeRecognizer::HMMShapeRecognizer()" <<endl;
		LOG(LTKLogger::LTK_LOGLEVEL_ERR) << "Unable to open HMM MDT file:"<<m_hmmMDTFilePath<<endl;

        throw LTKException(EFILE_OPEN_ERROR);
	}

	doesFileExist.close();

	m_hmmModelsListFilePath = strProfileDirectory + strProfileName + SEPARATOR + MODELSLISTFILE;

	doesFileExist.open(m_hmmModelsListFilePath.c_str());

	if(!doesFileExist)
	{
		LOG(LTKLogger::LTK_LOGLEVEL_ERR) << "Error: " << EFILE_OPEN_ERROR << " " <<
            "HMMShapeRecognizer::HMMShapeRecognizer()" <<endl;
		LOG(LTKLogger::LTK_LOGLEVEL_ERR) << "Unable to open HMM models list file:"<<m_hmmModelsListFilePath<<endl;

        throw LTKException(EFILE_OPEN_ERROR);
	}

	doesFileExist.close();

	m_hmmDictionaryFilePath = strProfileDirectory + strProfileName + SEPARATOR + DICTIONARYFILE;

	doesFileExist.open(m_hmmDictionaryFilePath.c_str());

	if(!doesFileExist)
	{
		LOG(LTKLogger::LTK_LOGLEVEL_ERR) << "Error: " << EFILE_OPEN_ERROR << " " <<
            "HMMShapeRecognizer::HMMShapeRecognizer()" <<endl;
		LOG(LTKLogger::LTK_LOGLEVEL_ERR) << "Unable to open HMM dictionary file:"<<m_hmmDictionaryFilePath<<endl;

        throw LTKException(EFILE_OPEN_ERROR);
	}

	doesFileExist.close();

	m_hmmNetworkFilePath = strProfileDirectory + strProfileName + SEPARATOR + NETWORKFILE;

	doesFileExist.open(m_hmmNetworkFilePath.c_str());

	if(!doesFileExist)
	{
		LOG(LTKLogger::LTK_LOGLEVEL_ERR) << "Error: " << EFILE_OPEN_ERROR << " " <<
            "HMMShapeRecognizer::HMMShapeRecognizer()" <<endl;
		LOG(LTKLogger::LTK_LOGLEVEL_ERR) << "Unable to open HMM network file:"<<m_hmmNetworkFilePath<<endl;

        throw LTKException(EFILE_OPEN_ERROR);
	}

	doesFileExist.close();


    //To find whether the project was dynamic or not andto read read number of shapes from project.cfg
    // TODO: change accordingly when signature of isProjectDynamic changes
    int errorCode = m_shapeRecUtil.isProjectDynamic(projectCFGPath,
            m_numShapes, strNumShapes, m_projectTypeDynamic);

    if( errorCode != SUCCESS)
    {
        LOG(LTKLogger::LTK_LOGLEVEL_ERR) << "Error: " << errorCode << " " <<
            "HMMShapeRecognizer::HMMShapeRecognizer()" <<endl;
        throw LTKException(errorCode);
    }


    //Currently preproc cfg also present in HMM
    tmpControlInfo.cfgFileName = HMM;
    errorCode = initializePreprocessor(tmpControlInfo,
            &m_ptrPreproc);

    if( errorCode != SUCCESS)
    {
        LOG(LTKLogger::LTK_LOGLEVEL_ERR) << "Error: " << errorCode << " " <<
            "HMMShapeRecognizer::HMMShapeRecognizer()" <<endl;
        throw LTKException(errorCode);
    }

    //Reading HMM configuration file
    errorCode = readClassifierConfig();

    if( errorCode != SUCCESS)
    {
		cout<<endl<<"Encountered error in readClassifierConfig"<<endl;
        LOG(LTKLogger::LTK_LOGLEVEL_ERR) << "Error: " << errorCode << " " <<
            "HMMShapeRecognizer::HMMShapeRecognizer()" <<endl;
        throw LTKException(errorCode);
    }


    errorCode = initializeFeatureExtractorInstance(tmpControlInfo);

    if( errorCode != SUCCESS)
    {
        LOG(LTKLogger::LTK_LOGLEVEL_ERR) << "Error: " << errorCode << " " <<
            "HMMShapeRecognizer::HMMShapeRecognizer()" <<endl;
        throw LTKException(errorCode);
    }
    LOG(LTKLogger::LTK_LOGLEVEL_DEBUG) << "Exiting " <<
        "HMMShapeRecognizer::HMMShapeRecognizer()" << endl;

}


HMMShapeRecognizer::~HMMShapeRecognizer()
{

	LOG(LTKLogger::LTK_LOGLEVEL_DEBUG) << "Entering " <<
        "HMMShapeRecognizer::~HMMShapeRecognizer()" << endl;

	int returnStatus = deletePreprocessor();
    if(returnStatus != SUCCESS)
    {
        LOG(LTKLogger::LTK_LOGLEVEL_ERR) << "Error: " << returnStatus << " " <<
            " HMMShapeRecognizer::~HMMShapeRecognizer()" << endl;
        throw LTKException(returnStatus);
    }

    //Unloading the feature Extractor instance
    returnStatus = deleteFeatureExtractorInstance();
    if(returnStatus != SUCCESS)
    {
        LOG(LTKLogger::LTK_LOGLEVEL_ERR) << "Error: " <<  returnStatus << " " <<
            " HMMShapeRecognizer::~HMMShapeRecognizer()" << endl;
        throw LTKException(returnStatus);
    }


	delete m_OSUtilPtr;

    LOG(LTKLogger::LTK_LOGLEVEL_DEBUG) << "Exiting " <<
        "HMMShapeRecognizer::~HMMShapeRecognizer()" << endl;

}


int HMMShapeRecognizer::train(const string& trainingInputFilePath,
        const string& mdtHeaderFilePath,
        const string &comment,const string &dataset,
        const string &trainFileType)
{
	LOG(LTKLogger::LTK_LOGLEVEL_DEBUG) << "Entering " <<
        "HMMShapeRecognizer::train()" << endl;

	LOG(LTKLogger::LTK_LOGLEVEL_ERR) << "Train not implemented for HMM Recognizer"<< endl;


	LOG(LTKLogger::LTK_LOGLEVEL_DEBUG) << "Exiting " <<
        "HMMShapeRecognizer::train()" << endl;


    return FAILURE;
}

/**
 * This method loads the Training Data of the HMM classifier.
 * @param
 * @return LTKInc::SUCCESS : if the model data was loaded successfully
 * @exception
 */
int HMMShapeRecognizer::loadModelData()
{

	m_ptrRecoNetwork =  new FSN();

	m_ptrViterbiDecoder = new Decoder();

	m_ptrRecoNetwork->initializeFromNetworkFile(m_hmmDictionaryFilePath,m_hmmNetworkFilePath,m_hmmModelsListFilePath,m_hmmMDTFilePath);

	m_ptrViterbiDecoder->initialize(m_ptrRecoNetwork,0,false);


	return SUCCESS;
}

int HMMShapeRecognizer::unloadModelData()
{
	delete m_ptrRecoNetwork;

	m_ptrRecoNetwork = NULL;

	delete m_ptrViterbiDecoder;

	m_ptrViterbiDecoder = NULL;

	return SUCCESS;
}


/**
 * This method sets the device context for the recognition
 *
 * @param deviceInfo The parameter to be set
 * @return
 * @exception
 */
int HMMShapeRecognizer::setDeviceContext(const LTKCaptureDevice& deviceInfo)
{
 LOG(LTKLogger::LTK_LOGLEVEL_DEBUG) << "Entering " <<
        "HMMShapeRecognizer::setDeviceContext()" << endl;

    m_captureDevice = deviceInfo;

    LOG(LTKLogger::LTK_LOGLEVEL_DEBUG) << "Exiting " <<
        "HMMShapeRecognizer::setDeviceContext()" << endl;

    return SUCCESS;
}


int HMMShapeRecognizer::recognize(const LTKTraceGroup& traceGroup,
        const LTKScreenContext& screenContext,
        const vector<int>& subSetOfClasses,
        float confThreshold,
        int  numChoices,
        vector<LTKShapeRecoResult>& outResultVector)
{


	if(m_ptrViterbiDecoder == NULL)
	{
		LOG(LTKLogger::LTK_LOGLEVEL_ERR)<<"Error: "<<ENULL_POINTER << " " <<
            " Decoder pointer is NULL"<<
            " HMMShapeRecognizer::recognize()" << endl;
        LTKReturnError(ENULL_POINTER);
	}


	  //Contains TraceGroup after Preprocessing is done
    LTKTraceGroup preprocessedTraceGroup;


    // Setting the capture device and screen context to be used in preprocessing

    if(m_ptrPreproc != NULL)
    {
        m_ptrPreproc->setCaptureDevice(m_captureDevice);
        m_ptrPreproc->setScreenContext(screenContext);
    }
    else
    {
        LOG(LTKLogger::LTK_LOGLEVEL_ERR)<<"Error: "<<ENULL_POINTER << " " <<
            " Preproc pointer is NULL"<<
            " HMMShapeRecognizer::recognize()" << endl;
        LTKReturnError(ENULL_POINTER);
    }


     LOG(LTKLogger::LTK_LOGLEVEL_DEBUG)<< "Before preprocess..."<< endl;
   //Preprocess the traceGroup
    if( preprocess(traceGroup, preprocessedTraceGroup) != SUCCESS)
    {
        LOG(LTKLogger::LTK_LOGLEVEL_ERR)<<"Error: "<< errorCode << " " <<
            getErrorMessage(errorCode)<<
            " HMMShapeRecognizer::recognize()" << endl;
        LTKReturnError(errorCode);
    }
     LOG(LTKLogger::LTK_LOGLEVEL_DEBUG)<< "After preprocess..."<< endl;

    //Extract the shapeSample from preprocessedTraceGroup
    if(!m_ptrFeatureExtractor)
    {
        LOG(LTKLogger::LTK_LOGLEVEL_ERR)<<"Error: "<< ENULL_POINTER << " " <<
            " m_ptrFeatureExtractor is NULL"<<
            " HMMShapeRecognizer::recognize()" << endl;
        LTKReturnError(ENULL_POINTER);
    }

	LOG(LTKLogger::LTK_LOGLEVEL_DEBUG)<< "Before extractFeatures..."<< endl;

    vector<LTKShapeFeaturePtr> tempFeatureVec;
    int errorCode = m_ptrFeatureExtractor->extractFeatures(preprocessedTraceGroup,
            tempFeatureVec);
	LOG(LTKLogger::LTK_LOGLEVEL_DEBUG)<< "After extractFeatures..."<< endl;

    if (errorCode != SUCCESS)
    {
        LOG(LTKLogger::LTK_LOGLEVEL_ERR)<<"Error: "<< errorCode << " " <<
            " HMMShapeRecognizer::recognize()" << endl;

        LTKReturnError(errorCode);
    }

	errorCode = recognize(tempFeatureVec,subSetOfClasses,confThreshold,numChoices,outResultVector);

	if (errorCode != SUCCESS)
    {
        LOG(LTKLogger::LTK_LOGLEVEL_ERR)<<"Error: "<< errorCode << " " <<
            " HMMShapeRecognizer::recognize()" << endl;

        LTKReturnError(errorCode);
    }

	return SUCCESS;
}

int HMMShapeRecognizer::recognize(const vector<LTKShapeFeaturePtr>& shapeFeatureVector,
			const vector<int>& subSetOfClasses,
			float confThreshold,
			int  numChoices,
			vector<LTKShapeRecoResult>& outResultVector)
{
	if(m_ptrViterbiDecoder == NULL)
	{
		LOG(LTKLogger::LTK_LOGLEVEL_ERR)<<"Error: "<<ENULL_POINTER << " " <<
            " Decoder pointer is NULL"<<
            " HMMShapeRecognizer::recognize()" << endl;
        LTKReturnError(ENULL_POINTER);
	}

	vector<vector<float> > featuresInVecForm;

	for(int f=0;f<shapeFeatureVector.size();++f)
	{
		vector<float> eachFeature;
		errorCode = shapeFeatureVector[f]->toFloatVector(eachFeature);

		if(errorCode != SUCCESS)
		{
			LOG(LTKLogger::LTK_LOGLEVEL_ERR)<<"Error: "<< errorCode << " " <<
            " HMMShapeRecognizer::recognize()" << endl;

			LTKReturnError(errorCode);
		}

		eachFeature.pop_back(); //removing the pen-up/down bit

		featuresInVecForm.push_back(eachFeature);
	}

	LOG(LTKLogger::LTK_LOGLEVEL_DEBUG) <<"Calling decode.."<<endl;

	//m_ptrViterbiDecoder->decode(featuresInVecForm);

	vector<string> recoResults;
	vector<float> scores;

	m_ptrViterbiDecoder->decodeNBestChars(featuresInVecForm,numChoices,recoResults,scores);

	//m_ptrViterbiDecoder->getNBest(numChoices, recoResults, scores);

	LOG(LTKLogger::LTK_LOGLEVEL_DEBUG) <<"Decode called."<<endl;

	for(int r=0;r<recoResults.size();++r)
	{
		LTKShapeRecoResult shapeRecoResult;
		//shapeRecoResult.setConfidence(scores[r]);
		shapeRecoResult.setConfidence(0.5f); //dummy number since loglikelihoods are negative and LTKShapeRecoResult mandates the confidence lies between 0 and 1
		shapeRecoResult.setShapeId(atoi(recoResults[r].c_str()));
		outResultVector.push_back(shapeRecoResult);
	}

	return SUCCESS;
}

int HMMShapeRecognizer::mapPreprocFunctions()
{
 LOG(LTKLogger::LTK_LOGLEVEL_DEBUG) << "Entering " <<
        "HMMShapeRecognizer::mapPreprocFunctions()" << endl;

    stringStringMap preProcSequence;

    stringStringPair tmpPair;

    stringVector moduleFuncNames;
    stringVector modFuncs;
    stringVector funcNameTokens;

    string module = "", funName = "", sequence = "";
    string::size_type indx;

    LTKTraceGroup local_inTraceGroup;

    LTKStringUtil::tokenizeString(m_preProcSeqn,  DELEMITER_SEQUENCE,  funcNameTokens);

    int numFunctions = funcNameTokens.size();

    if(numFunctions == 0)
    {
        LOG( LTKLogger::LTK_LOGLEVEL_ERR)<<"Error: "<<
            "Wrong preprocessor sequence in cfg file : " + m_preProcSeqn <<
            " HMMShapeRecognizer::mapPreprocFunctions()"<<endl;

        LTKReturnError(EINVALID_PREPROC_SEQUENCE);
    }

    for (indx = 0; indx < numFunctions ; indx++)
    {
        moduleFuncNames.push_back(funcNameTokens[indx]);
    }

    int numModuleFunctions = moduleFuncNames.size();

    for(indx=0; indx < numModuleFunctions ; indx++)
    {
        sequence = moduleFuncNames[indx];

        LTKStringUtil::tokenizeString(sequence,  DELEMITER_FUNC,  modFuncs);

        if(modFuncs.size() >= 2)
        {
            module = modFuncs.at(0);

            funName =  modFuncs.at(1);

            if(!module.compare("CommonPreProc"))
            {
                FN_PTR_PREPROCESSOR pPreprocFunc = NULL;
                pPreprocFunc = m_ptrPreproc->getPreprocptr(funName);
                if(pPreprocFunc!= NULL)
                {
                    tmpPair.first = module;
                    tmpPair.second = funName;
                    m_preprocSequence.push_back(tmpPair);
                }
                else
                {
                    LOG(LTKLogger::LTK_LOGLEVEL_ERR)<<"Error: "<< EINVALID_PREPROC_SEQUENCE << " " <<
                        "Wrong preprocessor sequence entry in cfg file : " <<funName<<
                        " HMMShapeRecognizer::mapPreprocFunctions()"<<endl;
                    LTKReturnError(EINVALID_PREPROC_SEQUENCE);
                }
            }
            else
            {
                LOG(LTKLogger::LTK_LOGLEVEL_ERR)<<"Error: "<< EINVALID_PREPROC_SEQUENCE << " " <<
                    "Wrong preprocessor sequence entry in cfg file  : " << module<<
                    " HMMShapeRecognizer::mapPreprocFunctions()"<<endl;
                LTKReturnError(EINVALID_PREPROC_SEQUENCE);
            }
        }
        else
        {
            LOG(LTKLogger::LTK_LOGLEVEL_ERR)<<"Error: "<< EINVALID_PREPROC_SEQUENCE << " " <<
                "Wrong preprocessor sequence entry in cfg file  : "<<module<<
                " HMMShapeRecognizer::mapPreprocFunctions()"<<endl;
            LTKReturnError(EINVALID_PREPROC_SEQUENCE);
        }
    }

    LOG(LTKLogger::LTK_LOGLEVEL_DEBUG) << "Exiting " <<
        "HMMShapeRecognizer::mapPreprocFunctions()" << endl;

    return SUCCESS;
}


void HMMShapeRecognizer::assignDefaultValues()
{
	LOG(LTKLogger::LTK_LOGLEVEL_DEBUG) << "Entering " <<
        "HMMShapeRecognizer::assignDefaultValues()" << endl;

    m_numShapes = 0;
    m_hmmCfgFilePath = "";
    m_hmmMDTFilePath = "";
    m_ptrPreproc = NULL;
    m_projectTypeDynamic=false;
    m_traceDimension=PREPROC_DEF_TRACE_DIMENSION;
    m_preProcSeqn=HMM_DEF_PREPROC_SEQ;
    m_ptrFeatureExtractor=NULL;
    m_featureExtractorName=HMM_DEF_FEATURE_EXTRACTOR;
    m_rejectThreshold=HMM_DEF_REJECT_THRESHOLD;
    m_deleteLTKLipiPreProcessor=NULL;

    LOG(LTKLogger::LTK_LOGLEVEL_DEBUG) << "Exiting " <<
        "HMMShapeRecognizer::assignDefaultValues()" << endl;
}


int HMMShapeRecognizer::readClassifierConfig()
{
	LOG(LTKLogger::LTK_LOGLEVEL_DEBUG) << "Entering " <<
        "HMMShapeRecognizer::readClassifierConfig()" << endl;
    string tempStringVar = "";
    int tempIntegerVar = 0;
    float tempFloatVar = 0.0;
    LTKConfigFileReader *shapeRecognizerProperties = NULL;
    int errorCode = FAILURE;

    try
    {
        shapeRecognizerProperties = new LTKConfigFileReader(m_hmmCfgFilePath);
    }
    catch(LTKException e)
    {
        LOG(LTKLogger::LTK_LOGLEVEL_INFO)<< "Info: " <<
            "Config file not found, using default values of the parameters" <<
            "HMMShapeRecognizer::readClassifierConfig()"<<endl;

        delete shapeRecognizerProperties;

		return FAILURE;
    }

    //Pre-processing sequence
    errorCode = shapeRecognizerProperties->getConfigValue(PREPROCSEQUENCE, m_preProcSeqn);

    if(errorCode != SUCCESS)
    {
        LOG(LTKLogger::LTK_LOGLEVEL_INFO) << "Info: " <<
            "Using default value of prerocessing sequence: "<< m_preProcSeqn <<
            " HMMShapeRecognizer::readClassifierConfig()"<<endl;

        m_preProcSeqn = HMM_DEF_PREPROC_SEQ;
    }



    if((errorCode = mapPreprocFunctions()) != SUCCESS)
    {
        LOG(LTKLogger::LTK_LOGLEVEL_ERR)<<" Error: " << errorCode << " " <<
            "HMMShapeRecognizer::readClassifierConfig()"<<endl;

        LTKReturnError(errorCode);
    }

    // reading the trace dimension
    errorCode = shapeRecognizerProperties->getConfigValue(TRACEDIMENSION,
                                                          tempStringVar);
    if(errorCode == SUCCESS )
    {
        if (LTKStringUtil::isInteger(tempStringVar))
        {
            tempIntegerVar = atoi((tempStringVar).c_str());

            if(tempIntegerVar >= 0)
            {
                m_traceDimension = tempIntegerVar;

                LOG(LTKLogger::LTK_LOGLEVEL_DEBUG)<<
                    TRACEDIMENSION " = " << m_traceDimension<< endl;
            }
            else
            {
                LOG(LTKLogger::LTK_LOGLEVEL_ERR) <<
                    "Error: " << ECONFIG_FILE_RANGE <<  " " << TRACEDIMENSION
                    " is out of permitted range" <<
                    " HMMShapeRecognizer::readClassifierConfig()"<<endl;
                LTKReturnError(ECONFIG_FILE_RANGE);
            }
        }
        else
        {
            LOG(LTKLogger::LTK_LOGLEVEL_ERR) <<
                    "Error: " << ECONFIG_FILE_RANGE <<  " " << TRACEDIMENSION
                    " is out of permitted range" <<
                    " HMMShapeRecognizer::readClassifierConfig()" << endl;

            LTKReturnError(ECONFIG_FILE_RANGE);
        }
    }
    else
    {
	    LOG(LTKLogger::LTK_LOGLEVEL_DEBUG) <<
            "Using default value for " << TRACEDIMENSION << ": " <<
            m_traceDimension << " HMMShapeRecognizer::readClassifierConfig()"<<endl;
    }



    tempStringVar = "";
    errorCode = shapeRecognizerProperties->getConfigValue(REJECT_THRESHOLD,
                                                          tempStringVar);
    if(errorCode == SUCCESS)
    {
        if ( LTKStringUtil::isFloat(tempStringVar) )
        {
            tempFloatVar = atof((tempStringVar).c_str());

            if(tempFloatVar  > 0 && tempFloatVar < 1)
            {
                m_rejectThreshold = tempFloatVar;

                LOG(LTKLogger::LTK_LOGLEVEL_DEBUG)<<
                    REJECT_THRESHOLD << " = " <<tempStringVar <<endl;
            }
            else
            {
                LOG(LTKLogger::LTK_LOGLEVEL_ERR)<<
                    "Error: " << ECONFIG_FILE_RANGE << REJECT_THRESHOLD <<
                    " should be in the range (0-1)" <<
                    " HMMShapeRecognizer::readClassifierConfig()"<<endl;

                LTKReturnError(ECONFIG_FILE_RANGE);
            }
        }
        else
        {
            LOG(LTKLogger::LTK_LOGLEVEL_ERR)<<
                "Error: " << ECONFIG_FILE_RANGE << REJECT_THRESHOLD <<
                " should be in the range (0-1)" <<
                " HMMShapeRecognizer::readClassifierConfig()"<<endl;

                LTKReturnError(ECONFIG_FILE_RANGE);
        }
    }
    else
    {
        LOG(LTKLogger::LTK_LOGLEVEL_DEBUG)<<
            "Using default value for " << REJECT_THRESHOLD <<
            " : " << m_rejectThreshold << endl;
    }


    tempStringVar = "";
    errorCode = shapeRecognizerProperties->getConfigValue(FEATUREEXTRACTOR,
                                                          tempStringVar);
    if(errorCode == SUCCESS)
    {
        m_featureExtractorName = tempStringVar;
        LOG(LTKLogger::LTK_LOGLEVEL_DEBUG)<<
            FEATUREEXTRACTOR << " = "<<tempStringVar<<endl;
    }
    else
    {
        LOG(LTKLogger::LTK_LOGLEVEL_DEBUG)<<
            "Using default value for " << FEATUREEXTRACTOR << " : " <<
            m_featureExtractorName << endl;
    }


    delete shapeRecognizerProperties;

    LOG(LTKLogger::LTK_LOGLEVEL_DEBUG) << "Exiting " <<
        "HMMShapeRecognizer::readClassifierConfig()" << endl;

    return SUCCESS;
}




int HMMShapeRecognizer::initializeFeatureExtractorInstance(const LTKControlInfo& controlInfo)
{
	LOG(LTKLogger::LTK_LOGLEVEL_DEBUG) << "Entering " <<
		"HMMShapeRecognizer::initializeFeatureExtractorInstance()" << endl;


	LTKShapeFeatureExtractorFactory factory;
	int errorCode = factory.createFeatureExtractor(m_featureExtractorName,
			m_lipiRootPath,
			&m_libHandlerFE,
			controlInfo,
			&m_ptrFeatureExtractor);

	if(errorCode != SUCCESS)
	{
		LOG(LTKLogger::LTK_LOGLEVEL_ERR) << "Error: "<< EFTR_EXTR_NOT_EXIST << " " <<
			" HMMShapeRecognizer::initializeFeatureExtractorInstance()" << endl;
		LTKReturnError(errorCode);
	}
	LOG(LTKLogger::LTK_LOGLEVEL_DEBUG) << "Exiting " <<
		"HMMShapeRecognizer::initializeFeatureExtractorInstance()" << endl;

	return SUCCESS;
}


int HMMShapeRecognizer::preprocess (const LTKTraceGroup& inTraceGroup, LTKTraceGroup& outPreprocessedTraceGroup)
{
	LOG(LTKLogger::LTK_LOGLEVEL_DEBUG) << "Entering " <<
        "HMMShapeRecognizer::preprocess()" << endl;

    int indx = 0;

    string module = "";
    string funName = "" ;

    LTKTraceGroup local_inTraceGroup;

    local_inTraceGroup = inTraceGroup;

    if(m_preprocSequence.size() != 0)
    {
        while(indx < m_preprocSequence.size())
        {
            module = m_preprocSequence.at(indx).first;
            funName =  m_preprocSequence.at(indx).second;

            FN_PTR_PREPROCESSOR pPreprocFunc = NULL;
            pPreprocFunc = m_ptrPreproc->getPreprocptr(funName);

            if(pPreprocFunc!= NULL)
            {
                outPreprocessedTraceGroup.emptyAllTraces();


                if((errorCode = (m_ptrPreproc->*(pPreprocFunc))
                            (local_inTraceGroup,outPreprocessedTraceGroup)) != SUCCESS)
                {
                    LOG(LTKLogger::LTK_LOGLEVEL_ERR) <<"Error: "<<  errorCode << " " <<
                        " HMMShapeRecognizer::preprocess()" << endl;
                    LTKReturnError(errorCode);
                }
                local_inTraceGroup = outPreprocessedTraceGroup;
            }
            indx++;
        }
    }
    LOG(LTKLogger::LTK_LOGLEVEL_DEBUG)<<"Exiting HMMShapeRecognizer::preprocess()"<<endl;
    return SUCCESS;
}



int HMMShapeRecognizer::deleteFeatureExtractorInstance()
{

    LOG(LTKLogger::LTK_LOGLEVEL_DEBUG) << "Entering " <<
        "HMMShapeRecognizer::deleteFeatureExtractorInstance()" << endl;

    if (m_ptrFeatureExtractor != NULL)
    {
        typedef int (*FN_PTR_DELETE_SHAPE_FEATURE_EXTRACTOR)(LTKShapeFeatureExtractor *obj);
        FN_PTR_DELETE_SHAPE_FEATURE_EXTRACTOR deleteFeatureExtractor;
        void * functionHandle = NULL;

        // Map createpreprocessor and deletePreprocessor functions
        int returnVal = m_OSUtilPtr->getFunctionAddress(m_libHandlerFE,
                                                DELETE_SHAPE_FEATURE_EXTRACTOR,
                                                &functionHandle);
        // Could not map the createLipiPreprocessor function from the DLL
    	if(returnVal != SUCCESS)
    	{
    	    LOG(LTKLogger::LTK_LOGLEVEL_ERR) <<
                "Error: "<< EDLL_FUNC_ADDRESS_DELETE_FEATEXT << " " <<
                getErrorMessage(EDLL_FUNC_ADDRESS_DELETE_FEATEXT) <<
                " HMMShapeRecognizer::deleteFeatureExtractorInstance()" << endl;

            LTKReturnError(EDLL_FUNC_ADDRESS_DELETE_FEATEXT);
    	}

        deleteFeatureExtractor = (FN_PTR_DELETE_SHAPE_FEATURE_EXTRACTOR)functionHandle;

        deleteFeatureExtractor(m_ptrFeatureExtractor);

        m_ptrFeatureExtractor = NULL;

        // unload feature extractor dll
        if(m_libHandlerFE != NULL)
    	{
            //Unload the DLL
    		m_OSUtilPtr->unloadSharedLib(m_libHandlerFE);
    		m_libHandlerFE = NULL;

    	}
    }

    LOG(LTKLogger::LTK_LOGLEVEL_DEBUG) << "Exiting " <<
        "HMMShapeRecognizer::deleteFeatureExtractorInstance()" << endl;

    return SUCCESS;
}


int HMMShapeRecognizer::extractFeatVecFromTraceGroup(const LTKTraceGroup& inTraceGroup,
        vector<LTKShapeFeaturePtr>& featureVec)
{
	LOG(LTKLogger::LTK_LOGLEVEL_DEBUG) << "Entering " <<
        "HMMShapeRecognizer::extractFeatVecFromTraceGroup()" << endl;

    LTKTraceGroup preprocessedTraceGroup; 	//TraceGroup after Preprocessing

    //Check for empty traces in inTraceGroup
    if(inTraceGroup.containsAnyEmptyTrace())
    {
        LOG(LTKLogger::LTK_LOGLEVEL_ERR) << "Error: "<< EEMPTY_TRACE << " " <<
            getErrorMessage(EEMPTY_TRACE) <<
            " HMMShapeRecognizer::extractFeatVecFromTraceGroup()" << endl;
        LTKReturnError(EEMPTY_TRACE);
    }

    //Preprocess the inTraceGroup
    if(preprocess(inTraceGroup, preprocessedTraceGroup) != SUCCESS)
    {
        LOG(LTKLogger::LTK_LOGLEVEL_ERR) << "Error: "<< errorCode << " " <<
            " HMMShapeRecognizer::extractFeatVecFromTraceGroup()" << endl;
        LTKReturnError(errorCode);
    }

    int errorCode = m_ptrFeatureExtractor->extractFeatures(preprocessedTraceGroup,
            featureVec);

    if (errorCode != SUCCESS)
    {
        LOG(LTKLogger::LTK_LOGLEVEL_ERR) << "Error: "<< errorCode << " " <<
            " HMMShapeRecognizer::extractFeatVecFromTraceGroup()" << endl;
        LTKReturnError(errorCode);
    }

    LOG(LTKLogger::LTK_LOGLEVEL_DEBUG)
        <<"Exiting HMMShapeRecognizer::extractFeatVecFromTraceGroup()" << endl;
    return SUCCESS;
}




int HMMShapeRecognizer::initializePreprocessor(const LTKControlInfo& controlInfo,
        LTKPreprocessorInterface** preprocInstance)
{
LOG(LTKLogger::LTK_LOGLEVEL_DEBUG) << "Entering " <<
        "HMMShapeRecognizer::initializePreprocessor()" << endl;

    FN_PTR_CREATELTKLIPIPREPROCESSOR createLTKLipiPreProcessor = NULL;
    int errorCode;

    // Load the DLL with path=preprocDLLPath
    void* functionHandle = NULL;

    int returnVal = m_OSUtilPtr->loadSharedLib(controlInfo.lipiRoot, PREPROC, &m_libHandler);


	if(returnVal != SUCCESS)
    {
        LOG(LTKLogger::LTK_LOGLEVEL_ERR) << "Error: "<<  ELOAD_PREPROC_DLL << " " <<
            getErrorMessage(ELOAD_PREPROC_DLL) <<
            " HMMShapeRecognizer::initializePreprocessor()" << endl;
        LTKReturnError(ELOAD_PREPROC_DLL);
    }

    // Map createpreprocessor and deletePreprocessor functions
    returnVal = m_OSUtilPtr->getFunctionAddress(m_libHandler,
                                            CREATEPREPROCINST,
                                            &functionHandle);
    // Could not map the createLipiPreprocessor function from the DLL
	if(returnVal != SUCCESS)
    {
        //Unload the dll
        unloadPreprocessorDLL();
        LOG(LTKLogger::LTK_LOGLEVEL_ERR) << "Error: "<< EDLL_FUNC_ADDRESS_CREATE << " " <<
            getErrorMessage(EDLL_FUNC_ADDRESS_CREATE) <<
            " HMMShapeRecognizer::initializePreprocessor()" << endl;

        LTKReturnError(EDLL_FUNC_ADDRESS_CREATE);
    }

    createLTKLipiPreProcessor = (FN_PTR_CREATELTKLIPIPREPROCESSOR)functionHandle;

    functionHandle = NULL;

    // Map createpreprocessor and deletePreprocessor functions
    returnVal = m_OSUtilPtr->getFunctionAddress(m_libHandler,
                                            DESTROYPREPROCINST,
                                            &functionHandle);
    // Could not map the createLipiPreprocessor function from the DLL
	if(returnVal != SUCCESS)
    {
        //Unload the dll
        unloadPreprocessorDLL();

        LOG(LTKLogger::LTK_LOGLEVEL_ERR) << "Error: "<< EDLL_FUNC_ADDRESS_CREATE << " " <<
            getErrorMessage(EDLL_FUNC_ADDRESS_CREATE) <<
            " HMMShapeRecognizer::initializePreprocessor()" << endl;
        LTKReturnError(EDLL_FUNC_ADDRESS_CREATE);
	}

    m_deleteLTKLipiPreProcessor = (FN_PTR_DELETELTKLIPIPREPROCESSOR)functionHandle;

    // Create preprocessor instance
    errorCode = createLTKLipiPreProcessor(controlInfo, preprocInstance);

    if(errorCode!=SUCCESS)
    {
        LOG(LTKLogger::LTK_LOGLEVEL_ERR) << "Error: "<<  errorCode << " " <<
            " HMMShapeRecognizer::initializePreprocessor()" << endl;
        LTKReturnError(errorCode);
    }

    // Could not create a LTKLipiPreProcessor
    if(*preprocInstance == NULL)
    {
        // Unload the DLL
        unloadPreprocessorDLL();

        LOG(LTKLogger::LTK_LOGLEVEL_ERR) << "Error: "<< ECREATE_PREPROC << " " <<
            getErrorMessage(ECREATE_PREPROC) <<
            " HMMShapeRecognizer::initializePreprocessor()" << endl;
        LTKReturnError(ECREATE_PREPROC);
    }

    LOG(LTKLogger::LTK_LOGLEVEL_DEBUG) << "Exiting " <<
        "HMMShapeRecognizer::initializePreprocessor()" << endl;

    return SUCCESS;
}




int HMMShapeRecognizer::deletePreprocessor()
{
	LOG(LTKLogger::LTK_LOGLEVEL_DEBUG) << "Entering " <<
        "HMMShapeRecognizer::deletePreprocessor()" << endl;

    //deleting the preprocessor instance
    if(m_ptrPreproc != NULL)
    {
        m_deleteLTKLipiPreProcessor(m_ptrPreproc);
        m_ptrPreproc = NULL;
    }

    //Unload the dll
    int returnStatus = unloadPreprocessorDLL();
    if(returnStatus != SUCCESS)
    {
        LOG(LTKLogger::LTK_LOGLEVEL_DEBUG) << "Error: " <<
            getErrorMessage(returnStatus) <<
            " HMMShapeRecognizer::deletePreprocessor()" << endl;
        LTKReturnError(returnStatus);
    }

    LOG(LTKLogger::LTK_LOGLEVEL_DEBUG) << "Exiting " <<
        "HMMShapeRecognizer::deletePreprocessor()" << endl;

    return SUCCESS;
}


int HMMShapeRecognizer::unloadPreprocessorDLL()
{
    LOG(LTKLogger::LTK_LOGLEVEL_DEBUG) << "Entering " <<
        "HMMShapeRecognizer::unloadPreprocessorDLL()" << endl;


    //Check the preprocessor DLL was loaded already
    if(m_libHandler != NULL)
    {
        //Unload the DLL
		m_OSUtilPtr->unloadSharedLib(m_libHandler);
        m_libHandler = NULL;
    }

    LOG(LTKLogger::LTK_LOGLEVEL_DEBUG) << "Exiting " <<
        "HMMShapeRecognizer::unloadPreprocessorDLL()" << endl;


	return SUCCESS;
}