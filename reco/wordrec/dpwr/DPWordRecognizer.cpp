/****************************************************************************
* Copyright (c) HP Labs India. All rights reserved.
* This source code is a property of HP Labs India. and is copyright protected.
* UNATHORISED ACCESS, COPYING, REPRODUCTION OR DISTRIBUTION of this program
* is strictly PROHIBITED.
****************************************************************************/

/************************************************************************
 * SVN MACROS
 *
 * $LastChangedDate: 2008-07-18 15:24:36 +0530 (Fri, 18 Jul 2008) $
 * $Revision: 563 $
 * $Author: sharmnid $
 *
 ************************************************************************/

/************************************************************************
 * FILE DESCR: Implementation of DPWordRecognizer 
 * CONTENTS:   <TODO>
 *
 * AUTHOR:     Deepu V.
 *
 * DATE:       March 23, 2005
 * CHANGE HISTORY:
 * Author		Date			Description of 
 ************************************************************************/

#include "DPWordRecognizer.h"

extern void *m_hAlgoDLLHandle;
/**********************************************************************************
* AUTHOR		: Deepu V.
* DATE			: 23-MAR-2005
* NAME			: DPWordRecognizer
* DESCRIPTION	: Default Constructor
* ARGUMENTS		: 
* RETURNS		: 
* NOTES			:
* CHANGE HISTROY
* Author			Date				Description of 
*************************************************************************************/

DPWordRecognizer::DPWordRecognizer()
{
	LOG(LTKLogger::LTK_LOGLEVEL_DEBUG, "Entered default constructor of DPWordRecognizer" );

	LOG(LTKLogger::LTK_LOGLEVEL_DEBUG, "Exiting default constructor of DPWordRecognizer" );
}

/**********************************************************************************
* AUTHOR		: Deepu V.
* DATE			: 23-MAR-2005
* NAME			: DPWordRecognizer
* DESCRIPTION	: Initialization Constructor
* ARGUMENTS		: 
* RETURNS		: 
* NOTES			:
* CHANGE HISTROY
* Author			Date				Description of 
*************************************************************************************/


DPWordRecognizer::DPWordRecognizer(const LTKWordRecoConfig& config)
:LTKWordRecognizer(DP)
{
	
	LOG(LTKLogger::LTK_LOGLEVEL_DEBUG, "Entered default Initialization of DPWordRecognizer" );

	//lipiRoot
	m_lipiRoot = config.getLipiRoot();

	//loading script 
	m_script = config.getScript();

	LOG(LTKLogger::LTK_LOGLEVEL_DEBUG, "Script : "<< m_script);

	//Finding the classifier path
	m_dpConfigFile = config.getLipiRoot() + SEPARATOR + config.getProblemName() 
		+ SEPARATOR + config.getProfile() +  SEPARATOR + config.getClassifierName() ;

	//writing styles training file
	m_styleModelFile = m_dpConfigFile  + DATFILEEXT;

	LOG(LTKLogger::LTK_LOGLEVEL_DEBUG, "Classifer training file : "<< m_styleModelFile);

	//relative positional training file
	m_positionModelFile = m_dpConfigFile + POSFILEEXT;

	LOG(LTKLogger::LTK_LOGLEVEL_DEBUG, "Positional training file : "<< m_positionModelFile);

	//loading the path to dictionary model 
	m_dictionaryConfigFile = config.getLipiRoot() +SEPARATOR + config.getDictionaryPath(); 

	LOG(LTKLogger::LTK_LOGLEVEL_DEBUG, "Dictionary config file : "<< m_dictionaryConfigFile);

	//loading the path to grammar model
	m_grammarConfigFile = config.getLipiRoot() +SEPARATOR + config.getGrammarPath();


	LOG(LTKLogger::LTK_LOGLEVEL_DEBUG, "Grammar config file : "<< m_grammarConfigFile);
	

	//classifier config file name
	m_dpConfigFile += CONFIGFILEEXT;

    LOG(LTKLogger::LTK_LOGLEVEL_DEBUG, "Classifer configuration file : "<< m_dpConfigFile );

	LOG(LTKLogger::LTK_LOGLEVEL_DEBUG, "Exiting default Initialization of DPWordRecognizer" );
}

/**********************************************************************************
* AUTHOR		: Deepu V.
* DATE			: 23-MAR-2005
* NAME			: initializeWordRecognizer
* DESCRIPTION	: Initialization of DP word Recognizer. This function performs
*                      -Initialization of styles Training Data
*                      -Initialization of positional Training Data
*                      -Loading the Grammar Model
*                      -Loading the dictionary
*                      -Create & initialize shape recognizer
* ARGUMENTS		: 
* RETURNS		: 
* NOTES			:
* CHANGE HISTROY
* Author			Date				Description of 
*************************************************************************************/
int DPWordRecognizer::initialize(string& strProjectName,string& strProfileName)
{
	LTKConfigFileReader* dpConfigMap;

	string shapeRecoConfigFile;

	LOG(LTKLogger::LTK_LOGLEVEL_DEBUG, "Entered DPWordRecognizer::initialize" );


	//reading the lipiroot from environment
	m_lipiRoot = getenv(LIPIROOT_ENV_STRING);


	m_dpConfigFile=m_lipiRoot + PROJECTS_PATH_STRING + 
		strProjectName + PROFILE_PATH_STRING + strProfileName + 
		SEPARATOR + DP + CONFIGFILEEXT;

	try
	{
		dpConfigMap= new LTKConfigFileReader(m_dpConfigFile);
	}
	catch(LTKException e)
	{
		delete dpConfigMap;
		LOG(LTKLogger::LTK_LOGLEVEL_ERR, e.getExceptionMessage() );
		LTKReturnError(ECONFIG_FILE_OPEN);	// Error while reading project.cfg 		
	}


	m_script = dpConfigMap->getConfigValue("Script");

	LOG(LTKLogger::LTK_LOGLEVEL_DEBUG, "Script : "+ m_script);


	//writing styles training file
	m_styleModelFile = m_lipiRoot + PROJECTS_PATH_STRING + 
		strProjectName + PROFILE_PATH_STRING + strProfileName + 
		SEPARATOR + DP + DATFILEEXT;

	LOG(LTKLogger::LTK_LOGLEVEL_DEBUG, "Classifer training file : "+ m_styleModelFile);

	//relative positional training file
	m_positionModelFile = m_lipiRoot + PROJECTS_PATH_STRING + 
		strProjectName + PROFILE_PATH_STRING + strProfileName + 
		SEPARATOR + DP + POSFILEEXT;

	LOG(LTKLogger::LTK_LOGLEVEL_DEBUG, "Positional training file : "+ m_positionModelFile);


	if(dpConfigMap->getConfigValue("UseDictionary")=="true")
	{
		//loading the path to dictionary model 
		m_dictionaryConfigFile = dpConfigMap->getConfigValue("DictionaryConfig"); 
	}

	LOG(LTKLogger::LTK_LOGLEVEL_DEBUG, "Dictionary config file : "+ m_dictionaryConfigFile);

	if(dpConfigMap["UseGrammar"]=="true")
	{
		//loading the path to grammar model
		m_grammarConfigFile = dpConfigMap["GrammarConfig"];
		m_grammar.initialize(m_grammarConfigFile);

		cout<<"Grammar Initialized!"<<endl;
	}


	LOG(LTKLogger::LTK_LOGLEVEL_DEBUG, "Grammar config file : "+ m_grammarConfigFile);






	m_numSymbols = atoi(dpConfigMap->getConfigValue("NumSymbols").c_str());

	m_shapeRecoProject=dpConfigMap->getConfigValue(DPSHAPEPROJECT);

	m_shapeRecoProfile=dpConfigMap->getConfigValue(DPSHAPEPROFILE);

	m_numShapeRecoResults = atoi(dpConfigMap->getConfigValue("NumShapeChoices").c_str());

	m_shapeRecoMinConfidence = atoi(dpConfigMap->getConfigValue("MinShapeConfid").c_str());




		
	//load training data
	if(loadTrainingData() == FAILURE)
	{
		LOG(LTKLogger::LTK_LOGLEVEL_ERR, "Exiting DPWordRecognizer::initializeWordRecognizer (Cannot load training data)" );
		return FAILURE;
	}

	
	//load dictionary
    //<TODO>: load the dictionary

	//load grammar model
	//<TODO>: load the grammar model

	//stringStringMap grammarConfig=LTKConfigFileReader::getMap(m_grammarConfigFile);

	//string grammarDefinitionFilePath=grammarConfig["grammardefinitionfile"];

	//m_grammar.initialize(grammarDefinitionFilePath);




	stringStringMap dictionariesList;

	try
	{
		dictionariesList=LTKConfigFileReader::getMap(m_dictionaryConfigFile);
	}
	catch(LTKException e)
	{
		LOG(LTKLogger::LTK_LOGLEVEL_ERR, e.getExceptionMessage() );
		LTKReturnError(ECONFIG_FILE_OPEN);	// Error while reading dictionary config file 		
	}	


	for(map<string,string>::iterator dictionariesListIterator=dictionariesList.begin();dictionariesListIterator!=dictionariesList.end();++dictionariesListIterator)
	{
		LTKDictionary eachDictionary;
		eachDictionary.loadDictionary(dictionariesListIterator->second);
		m_dictNameContentsMap.insert(make_pair(dictionariesListIterator->first,eachDictionary));
	}
	m_currDictPointer=m_dictNameContentsMap.end();

	//Initializing the Shape recognizer

	//shapeRecoConfigFile = this->m_lipiRoot +SEPARATOR + dpConfigMap["shaperecocfg"];
  
	//Creating the shape recognizer object
	m_shapeRecognizer = createShapeRecognizer(m_shapeRecoProject, m_shapeRecoProfile);

	if(m_shapeRecognizer == NULL)
	{
		LOG(LTKLogger::LTK_LOGLEVEL_ERR, getError(ECREATE_SHAPEREC) );
		LTKReturnError(ECREATE_SHAPEREC);			
	}


	//loading the model data file
	if( (errorCode = (m_shapeRecognizer->loadModelData())) != SUCCESS )
	{
		module_deleteShapeRecognizer(m_shapeRecognizer);
		m_shapeRecognizer = NULL;
		return errorCode;
	}



	m_numTracesProcessed=0;	//initializing number of traces processed

	LOG(LTKLogger::LTK_LOGLEVEL_DEBUG, "Exiting DPWordRecognizer::initialize Normally" );

    return SUCCESS;
}

/**********************************************************************************
* AUTHOR		: Deepu V.
* DATE			: 31-MAR-2005
* NAME			: processInk
* DESCRIPTION	: This method is called from recognition context whenever new traces
*               : are added to it.  The Recognizer need to process the new traces 
*               : in this methods and updates the internal state.
* ARGUMENTS		: rc - The recognition context for the current recognition
* RETURNS		: SUCCESS/FAILURE
* NOTES			:
* CHANGE HISTROY
* Author			Date				Description of 
*************************************************************************************/
int DPWordRecognizer::processInk (LTKRecognitionContext& rc) 
{

	vector<LTKTrace> preprocessedTraces;  //pre processed trace group

	const vector<LTKTrace>&  allTraces = rc.getAllInk();   //Reference to Ink in the recognition context

	LOG(LTKLogger::LTK_LOGLEVEL_DEBUG, "Entered DPWordRecognizer::processInk" );
     
	//pre processing using the screen context
	LTKWordPreprocessor::normalizeWordSize(allTraces,rc.getScreenContext(),
		preprocessedTraces, m_numTracesProcessed);

	recognizeTraces(preprocessedTraces,rc);
	
	LOG(LTKLogger::LTK_LOGLEVEL_DEBUG, "Entered DPWordRecognizer::processInk" );
	
	return SUCCESS;
}

/**********************************************************************************
* AUTHOR		: Deepu V.
* DATE			: 31-MAR-2005
* NAME			: endRecoUnit
* DESCRIPTION	: This function notifies the recognizer that end of current ink is
*               : the end of a logic segment.  This information could be used in 
*               : constraining the recognizer choices
* ARGUMENTS		: 
* RETURNS		: SUCCESS/FAILURE	
* NOTES			:
* CHANGE HISTROY
* Author			Date				Description of 
*************************************************************************************/
int DPWordRecognizer::endRecoUnit () 
{
	//<TODO>: Implement this method
	return SUCCESS;
}
 
/**********************************************************************************
* AUTHOR		: Deepu V.
* DATE			: 31-MAR-2005
* NAME			: recognize
* DESCRIPTION	: This is the recognize call. In case of trace by trace recognition
*               : The results of the recognition is set on the Recognition context 
*               : object.  
* ARGUMENTS		: rc - The recognition context for the current recognition
* RETURNS		: SUCCESS/FAILURE	
* NOTES			:
* CHANGE HISTROY
* Author			Date				Description of 
*************************************************************************************/
int DPWordRecognizer::recognize (LTKRecognitionContext& rc) 
{
	vector<LTKTrace> preprocessedTraces;  //pre processed trace group
	vector<LTKTrace> nonShiroRekhaTraceVec;
	vector<int> shiroRekhaIndices;

	string strDictMode=DICTIONARY_MODE;
	string strRecMode=REC_MODE;

	LOG(LTKLogger::LTK_LOGLEVEL_DEBUG, "Entered DPWordRecognizer::recognize" );

	if(rc.getFlag(strDictMode)==DICT_DRIVEN || rc.getFlag(strDictMode)==DICT_ASSISTED)
	{
		m_currDictPointer=m_dictNameContentsMap.find(rc.getLanguageModel("dictionary"));

		if(m_currDictPointer==m_dictNameContentsMap.end())
		{
			LOG(LTKLogger::LTK_LOGLEVEL_ERR, "No dictionary found"<<endl;
		}

		m_dictionaryMode=rc.getFlag(strDictMode);

	}
	else if(rc.getFlag(strDictMode)!=-1 && rc.getFlag(strDictMode)!=DICT_OFF)
	{
		LOG(ERR)<<"Unsupported dictionary mode"<<endl;
		return FAILURE;
	}

	if(rc.getFlag(strRecMode) == REC_MODE_BATCH)
	{
		//clear all the recognizer state
		clearRecognizerState();


		DPPreProcessor dpPreprocessor;
		dpPreprocessor.detectShiroRekha(rc.getAllInk(),shiroRekhaIndices);

		for(int traceGroupIndex=0;traceGroupIndex<rc.getAllInk().size();++traceGroupIndex)
		{
			bool isSRIndex=false;
			for(int indicesIndex=0;indicesIndex<shiroRekhaIndices.size();++indicesIndex)
			{
				if(traceGroupIndex==shiroRekhaIndices[indicesIndex])
				{
					isSRIndex=true;
					break;
				}
			}
			if(!isSRIndex)
			{
				if(rc.getAllInk()[traceGroupIndex].getNumChannels()>0)
				{
					nonShiroRekhaTraceVec.push_back(rc.getAllInk()[traceGroupIndex]);
				}
				
			}
		}

		
		LTKWordPreprocessor::normalizeWordSize(nonShiroRekhaTraceVec,preprocessedTraces);


		recognizeTraces(preprocessedTraces,rc);


	}
	else if (rc.getFlag(strRecMode) == REC_MODE_STREAMING)
	{
		LTKWordPreprocessor::normalizeWordSize(rc.getAllInk(),rc.getScreenContext(),
		preprocessedTraces, m_numTracesProcessed);
		recognizeTraces(preprocessedTraces,rc);
	}
	else 
	{
		LOG(ERR)<<"Unsupported reccongnizer mode"<<endl;
		return FAILURE;
	}

	//cout<<"Printing the result lattice..."<<endl;

	//cout<<"size of result lattice:"<<m_resultLattice.size()<<endl;
	/*for(int resIndex=0;resIndex<m_resultLattice.size();++resIndex)
	{

		for(int eachResIndex=0;eachResIndex<m_resultLattice[resIndex].size();++eachResIndex)
		{
			
			//cout<<"symbol:"<<m_symbolIDs[m_resultLattice[resIndex][eachResIndex].m_styleID]<<"-"<<m_resultLattice[resIndex][eachResIndex].m_confidence<<"-->";

			
			for(int m=0;m<m_styleModels[m_resultLattice[resIndex][eachResIndex].m_styleID].size();++m)
			{
				cout<<m_styleModels[m_resultLattice[resIndex][eachResIndex].m_styleID][m]<<"-";
			}

			//cout<<endl;

		}
		
		
	}*/




	m_currDictPointer=m_dictNameContentsMap.end();
	m_dictionaryMode=DICT_OFF;

	//<TODO> set the recognition results in the recognition context

	LOG(LTKLogger::LTK_LOGLEVEL_DEBUG, "Exiting DPWordRecognizer::recognize" );

	return SUCCESS;
}

/**********************************************************************************
* AUTHOR		: Deepu V.
* DATE			: 31-MAR-2005
* NAME			: recognize
* DESCRIPTION	: This method reset the recognizer
* ARGUMENTS		: resetParam - This parameter could specify what to reset
* RETURNS		: SUCCESS/FAILURE	
* NOTES			:
* CHANGE HISTROY
* Author			Date				Description of 
*************************************************************************************/
 int DPWordRecognizer::reset (int resetParam)
{
	//<TODO>: Implement this method
	clearRecognizerState();
	return SUCCESS;
}


/**********************************************************************************
* AUTHOR		: Deepu V.
* DATE			: 23-MAR-2005
* NAME			: ~DPWordRecognizer
* DESCRIPTION	: destructor 
* ARGUMENTS		: 
* RETURNS		: 
* NOTES			:
* CHANGE HISTROY
* Author			Date				Description of 
*************************************************************************************/

DPWordRecognizer::~DPWordRecognizer()
{
	LOG(LTKLogger::LTK_LOGLEVEL_DEBUG, "Entered destructor of DPWordRecognizer" );


	LOG(LTKLogger::LTK_LOGLEVEL_DEBUG, "Exiting destructor of DPWordRecognizer" );
}


//PRIVATE FUNCTIONS

/**********************************************************************************
* AUTHOR		: Deepu V.
* DATE			: 23-MAR-2005
* NAME			: loadTrainingData
* DESCRIPTION	: Initialization of DP word Recognizer Training data
* ARGUMENTS		: 
* RETURNS		: 
* NOTES			:
* CHANGE HISTROY
* Author			Date				Description of 
*************************************************************************************/

int DPWordRecognizer::loadTrainingData()
{
	LOG(LTKLogger::LTK_LOGLEVEL_DEBUG, "Entered DPWordRecognizer::loadTrainingData" );

	int totalStyles;	//stores the total number of styles
	int styleElement;	//stores element of each style
	int symbol;			//stores symbol ID
	int startShapeID;	//stores shape ID for each style

	m_maxStyleLength=0;

	int numOfShapes=-1;

	ifstream input(m_styleModelFile.c_str(),ios::binary);	//opening input stream for reading style info

	if(!input)
	{
		LOG(ERR)<<"Cannot open file "<<m_styleModelFile<<endl;
		return FAILURE;
	}
	input.read((char*)&totalStyles,sizeof(int));			//reading total number of styles

	LOG(LTKLogger::LTK_LOGLEVEL_DEBUG, "Total number of styles read from style info:"<<totalStyles<<endl;

	cout<< "Total number of styles read from style info:"<<totalStyles<<" styles..."<<endl;

	//Reading each style
	for(int styleIndex=0;styleIndex<totalStyles;++styleIndex)
	{

		 int styleLength;	//stores the length of each style

		 
		 input.read((char*)&styleLength,sizeof(int));	//reading length of current style

		 if(styleLength>m_maxStyleLength)
		 {
			m_maxStyleLength=styleLength;
		 }

		 input.read((char*)&styleElement,sizeof(int));	//reading style element(cluster ID/shape ID)

		 cout<<styleElement<<"-";
		 
		 //m_styleModelStartIDs.push_back(styleElement);	//getting the first element of each style

		 startShapeID=styleElement;

		 if(styleElement>numOfShapes)
		 {
			numOfShapes=styleElement;
		 }

		 m_startShapeStyleMap.insert(make_pair(styleElement,styleIndex));	//holds mapping of starting element and the corresponding style ID

		 vector<int> eachStyleVec;	//vector which contains the elements of the current style except the first element

		 for(int eachStyleIndex=1;eachStyleIndex<styleLength;++eachStyleIndex)
		 {

			 input.read((char*)&styleElement,sizeof(int));	//reading style element
			 eachStyleVec.push_back(styleElement);			//each style is formed

			 if(styleElement>numOfShapes)
			 {
				numOfShapes=styleElement;
			 }

			 cout<<styleElement<<"-";

		 }

		 
		 m_styleModels.push_back(eachStyleVec);	//style model constructed with each style

		 
		 input.read((char*)&symbol,sizeof(int));//reading symbol ID

		 m_symbolIDs.push_back(symbol);//constructing vector of symbol IDs

		 cout<<"-"<<symbol<<endl;

		 
		 if(m_symbolStartShapesMap.find(symbol)==m_symbolStartShapesMap.end())
		 {
			set<int> startShapesSet;
			startShapesSet.insert(startShapeID);
			m_symbolStartShapesMap.insert(make_pair(symbol,startShapesSet));

		 }
		 else
		 {

			 set<int>& startShapesSet=m_symbolStartShapesMap[symbol];
			 startShapesSet.insert(startShapeID);
		 }


       
	}

	//getchar();


	++numOfShapes;	//since styleElement is zero-based, adding 1 gives the actual number of shapes

	LOG(LTKLogger::LTK_LOGLEVEL_DEBUG, "Total number of shapes:"<<numOfShapes<<endl;

	//m_styleModelStartSubset.resize(numOfShapes);	//creating m_styleModelStartSubset bool vector for total number of shapes

	m_candidateShapeIDs.resize(numOfShapes);	////creating m_candidateShapeIDs bool vector for total number of shapes


	//Indices of styleModelStartSubset corresponding to styleModelStartIDs are made true
	/*for(int startIDIndex=0;startIDIndex<m_styleModelStartIDs.size();++startIDIndex)
	{

		m_styleModelStartSubset[m_styleModelStartIDs[startIDIndex]]=true;	//start ID index made true in the bool vector

	}*/

	input.close(); //closing the input stream of dat file	
	
	input.open(m_positionModelFile.c_str(),ios::binary);	//opening input stream for relative position info file
	
	if(!input)
	{
		LOG(ERR)<<"Cannot open file "<<m_positionModelFile<<endl;
		return FAILURE;
	}

	input.read((char*)&totalStyles,sizeof(int));		//reading total number of styles

	
	LOG(LTKLogger::LTK_LOGLEVEL_DEBUG, "Total number of styles read from relative pos info:"<<totalStyles<<endl;

	int numOfFeatures;		//stores number of features to be read

	input.read((char*)&numOfFeatures,sizeof(int));	//reading number of features

	
	LOG(LTKLogger::LTK_LOGLEVEL_DEBUG, "Number of features in relative pos info:"<<numOfFeatures<<endl;
	
	//Reading relative position info for each style
	for(int posStyleIndex=0;posStyleIndex<totalStyles;posStyleIndex++)
	{
		int numOfRecords;	//stores number of records(mean plus variance vectors)

		input.read((char*)&numOfRecords,sizeof(int));	//reads number of records(mean plus variance vectors)

		float2DVector eachStylePosInfo;	//stores each style's position info

		//reading mean and variance vectors
		for(int i=0;i<numOfRecords;i++)
		{
			floatVector featureVector;	//stores the feature vector, could be mean or variance vector
		   
			//reading each feature
		   for(int ii=0;ii<numOfFeatures;ii++)
		   {
			 float value;	//stores feature value

			 input.read((char*)&value,sizeof(float));	//reading feature value

			 featureVector.push_back(value);	//constructing the feature vector        
		   }  

		   eachStylePosInfo.push_back(featureVector);	//constructing each style's position info vector
		}

		m_positionModels.push_back(eachStylePosInfo);	//constructing positional models for each style
		
		input.read((char*)&symbol,sizeof(int));		//reading symbol ID but it is not used
	   
	}


	input.close();	//closing the input stream of relative positional info file

	
	LOG(LTKLogger::LTK_LOGLEVEL_DEBUG, "Exiting DPWordRecognizer::loadTrainingData" );

	return SUCCESS;
}


/**********************************************************************************
* AUTHOR		: Deepu V.
* DATE			: 04-APR-2005
* NAME			: clearRecognizerState
* DESCRIPTION	: Erase the state information of the recognizer
* ARGUMENTS		: 
* RETURNS		: 
* NOTES			:
* CHANGE HISTROY
* Author			Date				Description of 
*************************************************************************************/

int DPWordRecognizer::clearRecognizerState()
{
	LOG(LTKLogger::LTK_LOGLEVEL_DEBUG, "Entered DPWordRecognizer::clearRecognizerState" );

	m_numTracesProcessed = 0;

	m_incompleteResults.clear();
	
	m_resultLattice.clear();

	m_decodedResults.clear();

	//<TODO> : Implement clear of partial top results

	LOG(LTKLogger::LTK_LOGLEVEL_DEBUG, "Exiting DPWordRecognizer::clearRecognizerState" );

	return SUCCESS;
}

/**********************************************************************************
* AUTHOR		: Deepu V.
* DATE			: 04-APR-2005
* NAME			: recognizeTraces
* DESCRIPTION	: performs the recognition task
*               : pre condition - traces are pre processed
*               :               - markers are present in this vector
*               :               - m_numTracesProcessed is set to proper value
* ARGUMENTS		: traces - the strokes to be recognizer
* RETURNS		: 
* NOTES			:
* CHANGE HISTROY
* Author			Date				Description of 
*************************************************************************************/
int DPWordRecognizer::recognizeTraces(const vector<LTKTrace>& traces, LTKRecognitionContext& rc )
{
	vector<LTKTrace>::const_iterator traceIter,traceEnd; //iterator for the traces
	int recUnit;  //unit for recognition (either symbol/char/word(?))
	LTKScreenContext normalizedScreenContext = rc.getScreenContext(); //retrieving the screen context 
	LTKCaptureDevice normalizedCaptureDevice = rc.getDeviceContext(); //retrieving the device context

	string strRecUnitInfo=REC_UNIT_INFO;
	string strRecMode=REC_MODE;

	LOG(LTKLogger::LTK_LOGLEVEL_DEBUG, "Entered DPWordRecognizer::recognizeTraces" );

	//<TODO>: normalize screen context
	//<TODO>: normalize device context

	traceEnd = traces.end();

	for(traceIter = traces.begin(); traceIter != traceEnd; ++traceIter)
	{
		vector<DPSymbolRecognitionResult> symbolRecognitionResult; //holds symbol recognition results of the current trace
	
		m_resultLattice.push_back(symbolRecognitionResult);

		//if it is marker stroke
		if((*traceIter).getNumChannels() == 0)
		{
			recUnit = rc.getFlag(strRecUnitInfo);
			switch(recUnit)
			{
			//* This case is not supported
			case REC_UNIT_WORD:
				break;
				//<TODO>: General case this could be 
				//        used to restart dictionary search
				//        and throw away all incomplete choices

			//* The segment is character
			case REC_UNIT_CHAR:
				//<TODO>: Restart the character level grammar
				//        discard incompleted characters

			//* segment is symbol.
			case REC_UNIT_SYMBOL:
				//discard incompleted symbols
				m_incompleteResults.clear();

			//* segment info is unknown. no effect
			case REC_UNIT_UNKNOWN: break;
			}

		}
		else
		{
				//update confids of hypotheses
				updateHypotheses(*traceIter,normalizedScreenContext, normalizedCaptureDevice);
				//form new Hypothesis
				generateNewHypotheses(*traceIter,normalizedScreenContext, normalizedCaptureDevice);

				//decodes best paths
				decodeRecognitionPaths(rc);

					/*cout<<"Recognized top choices:("<<m_numTracesProcessed<<")"<<endl;
					for(int resVecIndex=0;resVecIndex<m_decodedResults[0].size();++resVecIndex)
					{
						cout<<"Recognized(TOP"<<resVecIndex+1<<"):";
						for(int res=0;res<m_decodedResults[0][resVecIndex].m_symbolIDVec.size();++res)
						{
							cout<<m_decodedResults[0][resVecIndex].m_symbolIDVec[res]<<",";
						}
						//cout<<" confidence:"<<m_decodedResults[0][resVecIndex].m_confidence<<endl;
						printf("%s%f\n"," confidence:",m_decodedResults[0][resVecIndex].m_confidence);
					}

				getchar();*/
		}

		++m_numTracesProcessed;

	}


	if(m_decodedResults.size()>0)	//check necessary when the first stroke is invalid (scribbled) and recognize is called
	{
		if(rc.getFlag(strRecMode)==REC_MODE_BATCH && m_dictionaryMode==DICT_DRIVEN)
		{
			vector<DPDecodedRecognitionResult>& wordRecognitionResults=m_decodedResults[0];
			vector<DPDecodedRecognitionResult> finalRecognitionResults;
			for(vector<DPDecodedRecognitionResult>::iterator wordRecognitionResultsIter=wordRecognitionResults.begin();wordRecognitionResultsIter!=wordRecognitionResults.end();++wordRecognitionResultsIter)
			{
				vector<int> nextPossibleSymbols;
				(m_currDictPointer->second).getNextPossibleSymbols((*wordRecognitionResultsIter).m_symbolIDVec,nextPossibleSymbols);
				if(nextPossibleSymbols.size()==0)
				{
					finalRecognitionResults.push_back(*wordRecognitionResultsIter);
				}
			}

			m_decodedResults[0]=finalRecognitionResults;
			
		}

		for(int resVecIndex=0;resVecIndex<m_decodedResults[0].size();++resVecIndex)
		{
			cout<<"Recognized(TOP"<<resVecIndex+1<<"):";
			vector<unsigned short> eachRecognizedResult;
			for(int res=0;res<m_decodedResults[0][resVecIndex].m_symbolIDVec.size();++res)
			{
				cout<<m_decodedResults[0][resVecIndex].m_symbolIDVec[res]<<",";
				eachRecognizedResult.push_back(m_decodedResults[0][resVecIndex].m_symbolIDVec[res]);

			}
			LTKWordRecoResult ltkWordResult;
			ltkWordResult.setWordRecoResult(eachRecognizedResult,m_decodedResults[0][resVecIndex].m_confidence);
			rc.addRecognitionResult(ltkWordResult);
			
			//cout<<" confidence:"<<m_decodedResults[0][resVecIndex].m_confidence<<endl;
			printf("%s%f\n"," confidence:",m_decodedResults[0][resVecIndex].m_confidence);
		}
	}

	/*cout<<"Recognized top choices:"<<endl;

	for(int resVecIndex=0;resVecIndex<m_decodedResults[0].size();++resVecIndex)
	{
		cout<<"Recognized(TOP"<<resVecIndex+1<<"):";
		for(int res=0;res<m_decodedResults[0][resVecIndex].m_symbolIDVec.size();++res)
		{
			cout<<m_decodedResults[0][resVecIndex].m_symbolIDVec[res]<<",";
		}
		//cout<<" confidence:"<<m_decodedResults[0][resVecIndex].m_confidence<<endl;
		printf("%s%f\n"," confidence:",m_decodedResults[0][resVecIndex].m_confidence);
	}*/

	//<TODO> : do a DP search to find best N paths


	//Last : update the number of traces processed

	LOG(LTKLogger::LTK_LOGLEVEL_DEBUG, "Exiting DPWordRecognizer::recognizeTraces" );

	return SUCCESS;
}


/**********************************************************************************
* AUTHOR		: Deepu V.
* DATE			: 07-APR-2005
* NAME			: updateHypotheses
* DESCRIPTION	: This function tries to complete the
*               : existing hypothesis with the trace
* ARGUMENTS		: trace - new trace used for updation
* RETURNS		: 
* NOTES			:
* CHANGE HISTROY
* Author			Date				Description of 
*************************************************************************************/
int DPWordRecognizer::updateHypotheses(const LTKTrace& trace, LTKScreenContext screenContext, LTKCaptureDevice deviceContext)
{
	//<TODO> : Need to have relative position confidence measure

	LOG(LTKLogger::LTK_LOGLEVEL_DEBUG, "Entered DPWordRecognizer::updateHypotheses" );

	list<DPSymbolRecognitionResult>::iterator incompleteResultsIter;	//to iterate incomplete results

	list<DPSymbolRecognitionResult>::iterator tempIterator;	//temporary iterator to enable removing from middle

	int confidenceVecIndex=0;	//index variable to access confidence vector

	vector<int> shapeIDVec;	//vector holding next set of shape IDs

	vector<float> confidenceVec;	//confidence vector corresponding to shape ID vector

	//Iterating incomplete results list to find next set of shape IDs to be recognized
	for(incompleteResultsIter=m_incompleteResults.begin();incompleteResultsIter!=m_incompleteResults.end();++incompleteResultsIter)
	{

		shapeIDVec.push_back(m_styleModels[(*incompleteResultsIter).m_styleID][((*incompleteResultsIter).m_numTraces)-1]);

	}

	LOG(DEBUG)<<"Next set of shape IDs determined"<<endl;

	int x = this->m_numTracesProcessed;
	int y = shapeIDVec.size();

	//calling shape recognizer to recognize candidate shape IDs
	 if(m_shapeRecognizer->getShapeRecoConfidences(trace,screenContext,shapeIDVec,confidenceVec)==FAILURE)
	 {

		LOG(ERR)<<"Unable to get results from shape recognizer"<<endl;

		return FAILURE;
	 }


	 LOG(DEBUG)<<"Results of shape recognizer obtained. Updating hypotheses..."<<endl;

	//iterating incomplete results list to update the hypotheses
	for(incompleteResultsIter=m_incompleteResults.begin();incompleteResultsIter!=m_incompleteResults.end();)
	{

		tempIterator=incompleteResultsIter++;	//assigning to temporary iterator to enable deleting middle element from list

		++(*tempIterator).m_numTraces;	//updating number of traces processed in that style


		(*tempIterator).m_confidence+=confidenceVec[confidenceVecIndex];	//updating confidence of this style

		
		if((*tempIterator).isCompleted())	//if style is complete
		{


			bool alreadyAddedToLattice=false;
			for(vector<DPSymbolRecognitionResult>::iterator latticeColIterator=m_resultLattice[m_numTracesProcessed].begin();latticeColIterator!=m_resultLattice[m_numTracesProcessed].end();++latticeColIterator)
			{

				if(m_symbolIDs[(*latticeColIterator).m_styleID]==m_symbolIDs[(*tempIterator).m_styleID])
				{
						if((*latticeColIterator).m_totalNumTraces==(*tempIterator).m_totalNumTraces)
						{
							alreadyAddedToLattice=true;
							
							if((*latticeColIterator).m_confidence < (*tempIterator).m_confidence)
							{

								(*latticeColIterator).m_confidence=(*tempIterator).m_confidence;

							}
							
							break;

						}
				}
			}

			if(!alreadyAddedToLattice)
			{
				m_resultLattice[m_numTracesProcessed].push_back(*tempIterator);	//moving to result lattice

				//cout<<"Update complete symbol:"<<m_symbolIDs[(*tempIterator).m_styleID]<<endl;

				//cout<<"num oftrace processed:"<<m_numTracesProcessed<<"-"<<m_symbolIDs[(*tempIterator).m_styleID]<<endl;

				//getchar();

			}

			

			m_incompleteResults.erase(tempIterator);	//erasing from incomplete results list
		}

		++confidenceVecIndex;	//incrementing confidence vector index
	}
	 
	LOG(DEBUG)<<"Hypotheses updated!"<<endl;

	LOG(LTKLogger::LTK_LOGLEVEL_DEBUG, "Exiting DPWordRecognizer::updateHypotheses" );

	return SUCCESS;

}

/**********************************************************************************
* AUTHOR		: Deepu V.
* DATE			: 07-APR-2005
* NAME			: generateNewHypotheses
* DESCRIPTION	: This function tries to generate
*               : new  hypothesis starting with the trace
* ARGUMENTS		: trace - new trace used for generation
* RETURNS		: 
* NOTES			:
* CHANGE HISTROY
* Author			Date				Description of 
*************************************************************************************/

int DPWordRecognizer::generateNewHypotheses(const LTKTrace& trace, LTKScreenContext screenContext, 
											LTKCaptureDevice deviceContext)
{

	LOG(LTKLogger::LTK_LOGLEVEL_DEBUG, "Entered DPWordRecognizer::generateNewHypotheses" );

	vector<LTKShapeRecoResult> shapeRecoResults;	//stores the results of shape recognizer

	vector<LTKShapeRecoResult>::iterator resultIterator;	//iterator for shape recognition results

	int shapeID;	//stores shape ID

	float confidence;	//stores confidence associated with the shape

	multimap<int,int>::iterator iter;	//iterator for first shape ID to style map

	//<TODO> : m_styleModelStartSubset will be different if 
	//<TODO> : there is beginning of a character/word

	if(m_numTracesProcessed==0)	
	{
		if(m_dictionaryMode==DICT_DRIVEN)
		{
			vector<int> zeroLengthWord;
			vector<int> possibleSymbols;
			//m_grammar.isValid(zeroLengthWord,possibleSymbols);
			(m_currDictPointer->second).getNextPossibleSymbols(zeroLengthWord,possibleSymbols);
			m_candidateShapeIDs.assign(m_candidateShapeIDs.size(),false);
			for(vector<int>::iterator possibleSymbolsIter=possibleSymbols.begin();possibleSymbolsIter!=possibleSymbols.end();++possibleSymbolsIter)
			{
				//cout<<"candidate:"<<*possibleSymbolsIter<<endl;
				set<int>& eachSymbolStartShapes=m_symbolStartShapesMap[*possibleSymbolsIter];
				for(set<int>::iterator eachSymbolStartShapesIter=eachSymbolStartShapes.begin();eachSymbolStartShapesIter!=eachSymbolStartShapes.end();++eachSymbolStartShapesIter)
				{
					if(m_symbolIDs[(m_startShapeStyleMap.find(*eachSymbolStartShapesIter))->second]==*possibleSymbolsIter)
					{
						m_candidateShapeIDs[*eachSymbolStartShapesIter]=true;
					}
					//cout<<m_numTracesProcessed<<":candidate:"<<m_symbolIDs[(m_startShapeStyleMap.find(*eachSymbolStartShapesIter))->second]<<endl;
				}
				//getchar();
			}

			//getchar();
			
		}
		else
		{
			vector<int> zeroLengthWord;
			vector<int> possibleSymbols;
			m_grammar.isValid(zeroLengthWord,possibleSymbols);
			m_candidateShapeIDs.assign(m_candidateShapeIDs.size(),false);
			for(vector<int>::iterator possibleSymbolsIter=possibleSymbols.begin();possibleSymbolsIter!=possibleSymbols.end();++possibleSymbolsIter)
			{
				set<int>& eachSymbolStartShapes=m_symbolStartShapesMap[*possibleSymbolsIter];
				for(set<int>::iterator eachSymbolStartShapesIter=eachSymbolStartShapes.begin();eachSymbolStartShapesIter!=eachSymbolStartShapes.end();++eachSymbolStartShapesIter)
				{
					if(m_symbolIDs[(m_startShapeStyleMap.find(*eachSymbolStartShapesIter))->second]==*possibleSymbolsIter)
					{
						m_candidateShapeIDs[*eachSymbolStartShapesIter]=true;
					}
				}
			}
		}
	}

	m_shapeRecognizer->recognize(trace, screenContext, m_candidateShapeIDs,
		m_shapeRecoMinConfidence, m_numShapeRecoResults, shapeRecoResults);	//calling the shape recognizer for recognition

	//cout<<"number of shape reco results returned:"<<shapeRecoResults.size()<<","<<m_numShapeRecoResults<<","<<m_shapeRecoMinConfidence<<endl;

		
	LOG(LTKLogger::LTK_LOGLEVEL_DEBUG,"Got results from shape recognizer, generating new hypotheses"<<endl;


	//Iterating shape recognizer's results to generate new hypotheses
	for(resultIterator=shapeRecoResults.begin();resultIterator!=shapeRecoResults.end();++resultIterator)
	{
		shapeID=(*resultIterator).getShapeId();	//getting shape ID

		confidence=(*resultIterator).getConfidence();	//getting confidence associated with the shape

		//cout<<"Shape ID"<<shapeID<<"-Conf:"<<confidence<<endl;
		//getchar();

		
		for(iter=m_startShapeStyleMap.lower_bound(shapeID);iter!=m_startShapeStyleMap.upper_bound(shapeID);++iter)
		{
			//creating DPSymbolRecognitionResult from the result of shape recognizer
			DPSymbolRecognitionResult symbolRecognitionResult(iter->second,confidence,1,m_styleModels[iter->second].size()+1);

			if(m_styleModels[iter->second].size()==0)	//if style size is one (rest size zero), then it is complete
			{
				
				bool alreadyAddedToLattice=false;
				for(vector<DPSymbolRecognitionResult>::iterator latticeColIterator=m_resultLattice[m_numTracesProcessed].begin();latticeColIterator!=m_resultLattice[m_numTracesProcessed].end();++latticeColIterator)
				{
					if(m_symbolIDs[(*latticeColIterator).m_styleID]==m_symbolIDs[symbolRecognitionResult.m_styleID])
					{
							if((*latticeColIterator).m_totalNumTraces==symbolRecognitionResult.m_totalNumTraces)
							{
								alreadyAddedToLattice=true;
								
								if((*latticeColIterator).m_confidence < symbolRecognitionResult.m_confidence)
								{

									(*latticeColIterator).m_confidence=symbolRecognitionResult.m_confidence;

								}
								
								break;

							}
					}
				}

				if(!alreadyAddedToLattice)
				{
					m_resultLattice[m_numTracesProcessed].push_back(symbolRecognitionResult);	//moving to result lattice
					//cout<<"Gen complete symbol(shape):"<<m_symbolIDs[iter->second]<<"("<<shapeID<<")"<<endl;
				}

				
			}
			else	//style not complete,
			{
				//cout<<"Gen Incomplete symbol(shape):"<<m_symbolIDs[iter->second]<<"("<<shapeID<<")"<<endl;

				m_incompleteResults.push_back(symbolRecognitionResult);	//moving to the list which contains incomplete results
			}
		}

	}


	LOG(LTKLogger::LTK_LOGLEVEL_DEBUG, "Exiting DPWordRecognizer::generateNewHypotheses" );

	return SUCCESS;

}


/**********************************************************************************
* AUTHOR		: Bharath A.
* DATE			: 03-MAY-2005
* NAME			: decodeRecognitionPaths
* DESCRIPTION	: This function finds suitable paths after recognition of each trace
* ARGUMENTS		: recoContext - recognition context
* RETURNS		: 
* NOTES			:
* CHANGE HISTROY
* Author			Date				Description of 
*************************************************************************************/
int DPWordRecognizer::decodeRecognitionPaths(LTKRecognitionContext& recoContext)
	{
		LOG(LTKLogger::LTK_LOGLEVEL_DEBUG, "Entered DPWordRecognizer::decodeRecognitionPaths" <<endl;


		
		vector<DPDecodedRecognitionResult> decodedResultVec(recoContext.getNumResults()*2);	//holds top 2*N choices of paths to compensate for reduction after the last stroke using dictionary & grammar.

		LOG(LTKLogger::LTK_LOGLEVEL_DEBUG, "Iterating last column of the result lattice"<<endl;
		//Iterating through the last column of result lattice
		for(int symbolRecoResultIndex=0;
			symbolRecoResultIndex<m_resultLattice[m_numTracesProcessed].size();
			++symbolRecoResultIndex)
		{
			DPSymbolRecognitionResult& eachSymbolResult
				=m_resultLattice[m_numTracesProcessed][symbolRecoResultIndex];	//reference to symbol recognition result

			int symbolID=m_symbolIDs[eachSymbolResult.m_styleID];	//getting the symbol ID

			if(eachSymbolResult.m_totalNumTraces > m_decodedResults.size())	//If there is no predecessor symbols
			{
				//creating decoded recognition result with this symbol
				DPDecodedRecognitionResult decodedResult;	

				decodedResult.m_symbolIDVec.push_back(symbolID);

				decodedResult.m_confidence=eachSymbolResult.m_confidence;

				vector<int> nextPossibleSymbols;

				bool shouldAddToResults=true;

				for(vector<DPDecodedRecognitionResult>::iterator checkDecodedResultVecIter=decodedResultVec.begin();checkDecodedResultVecIter!=decodedResultVec.end();++checkDecodedResultVecIter)
				{
					if(decodedResult.m_symbolIDVec==(*checkDecodedResultVecIter).m_symbolIDVec && decodedResult.m_confidence <  (*checkDecodedResultVecIter).m_confidence)
					{
						shouldAddToResults=false;
						break;
					}
				}

				if(shouldAddToResults)
				{

					if(m_dictionaryMode==DICT_DRIVEN)
					{
						if((m_currDictPointer->second).getNextPossibleSymbols(decodedResult.m_symbolIDVec,nextPossibleSymbols))
						{
							
							constructTopNChoices(decodedResultVec,decodedResult);
						}
					}
					else
					{

						if(m_grammar.isValid(decodedResult.m_symbolIDVec,nextPossibleSymbols))
						{
							constructTopNChoices(decodedResultVec,decodedResult);	//to find best N paths
						}
					}
				}


				//	constructTopNChoices(decodedResultVec,decodedResult);	//to find best N paths
			}
			else
			{
				vector<DPDecodedRecognitionResult>& preceedingSymbolsVec 
					= m_decodedResults[eachSymbolResult.m_totalNumTraces-1];	//reference to vector containing the list of predecessor symbols

				vector<DPDecodedRecognitionResult>::iterator predecessorIter;	//iterator to iterate the vector

				for(predecessorIter=preceedingSymbolsVec.begin();predecessorIter!=preceedingSymbolsVec.end();++predecessorIter)
				{
					DPDecodedRecognitionResult decodedResult=(*predecessorIter);	//holds vector of predecessor symbols

					decodedResult.m_symbolIDVec.push_back(symbolID);	//concatenating the current symbol with its predecessors

					//This is the place where confidences are updated
					//<TODO>:May need to incorporate relative position info between symbols
					//in case we need to do something other than addition of confidences
				
					decodedResult.m_confidence+=eachSymbolResult.m_confidence;	//updating the confidence
					
					vector<int> nextPossibleSymbols;
					bool shouldAddToResults=true;


					for(vector<DPDecodedRecognitionResult>::iterator checkDecodedResultVecIter=decodedResultVec.begin();checkDecodedResultVecIter!=decodedResultVec.end();++checkDecodedResultVecIter)
					{
						if(decodedResult.m_symbolIDVec==(*checkDecodedResultVecIter).m_symbolIDVec && decodedResult.m_confidence <  (*checkDecodedResultVecIter).m_confidence)
						{
							shouldAddToResults=false;
							break;
						}
					}

					if(shouldAddToResults)
					{

						if(m_dictionaryMode==DICT_DRIVEN)
						{
							if((m_currDictPointer->second).getNextPossibleSymbols(decodedResult.m_symbolIDVec,nextPossibleSymbols))
							{
								constructTopNChoices(decodedResultVec,decodedResult);	//to find best N paths
							}
						}
						else
						{
							if(m_grammar.isValid(decodedResult.m_symbolIDVec,nextPossibleSymbols))
							{
								constructTopNChoices(decodedResultVec,decodedResult);	//to find best N paths
								
							}
						}
					}

				}
			}
		}


			m_candidateShapeIDs.assign(m_candidateShapeIDs.size(),false);

			for(vector<DPDecodedRecognitionResult>::iterator decodedResultVecIter=decodedResultVec.begin();decodedResultVecIter!=decodedResultVec.end();++decodedResultVecIter)
			{
				DPDecodedRecognitionResult& resultInTopN=*decodedResultVecIter;

				vector<int> possibleSymbols;

				if(m_dictionaryMode==DICT_DRIVEN)
				{
					(m_currDictPointer->second).getNextPossibleSymbols(resultInTopN.m_symbolIDVec,possibleSymbols);
				}
				else
				{
					m_grammar.isValid(resultInTopN.m_symbolIDVec,possibleSymbols);
				}

				for(vector<int>::iterator possibleSymbolsIter=possibleSymbols.begin();possibleSymbolsIter!=possibleSymbols.end();++possibleSymbolsIter)
				{
					set<int>& eachSymbolStartShapes=m_symbolStartShapesMap[*possibleSymbolsIter];
					//cout<<"Symbol:"<<*possibleSymbolsIter<<":";
					for(set<int>::iterator eachSymbolStartShapesIter=eachSymbolStartShapes.begin();eachSymbolStartShapesIter!=eachSymbolStartShapes.end();++eachSymbolStartShapesIter)
					{
						if(m_symbolIDs[(m_startShapeStyleMap.find(*eachSymbolStartShapesIter))->second]==*possibleSymbolsIter)
						{
							m_candidateShapeIDs[*eachSymbolStartShapesIter]=true;
						}
						//cout<<*eachSymbolStartShapesIter<<",";
						
					}	
					//cout<<endl;
					
				}
				//getchar();
			
			}

			

		LOG(LTKLogger::LTK_LOGLEVEL_DEBUG,"pushing top N choices to decoded results"<<endl;


		for(vector<DPDecodedRecognitionResult>::reverse_iterator finalDecodedResultVecIter=decodedResultVec.rbegin();finalDecodedResultVecIter!=decodedResultVec.rend();++finalDecodedResultVecIter)
		{
			if((*finalDecodedResultVecIter).m_confidence==0.0f && (*finalDecodedResultVecIter).m_symbolIDVec.size()==0)
			{
				decodedResultVec.pop_back();
			}
			else
			{
				break;
			}
		}


		//getchar();

		m_decodedResults.push_front(decodedResultVec);	//top N pushed to decoded results  deque

		if(m_decodedResults.size()>m_maxStyleLength)	//if the deque size greater than max style length
		{
			m_decodedResults.pop_back();	//removing the oldest vector
		}

		LOG(LTKLogger::LTK_LOGLEVEL_DEBUG, "Exiting DPWordRecognizer::decodeRecognitionPaths"<<endl;
		return SUCCESS;

	}



/**********************************************************************************
* AUTHOR		: Bharath A.
* DATE			: 03-MAY-2005
* NAME			: constructTopNChoices
* DESCRIPTION	: This function adds current decoded path to the vector and finds best N paths. Does insertion sort for newly added choice
* ARGUMENTS		: decodedResultVec - vector containing decoded results with confidences
*					decodedResult - decoded path with the confidence	
* RETURNS		: 
* NOTES			:
* CHANGE HISTROY
* Author			Date				Description of 
*************************************************************************************/

	int DPWordRecognizer::constructTopNChoices(
			vector<DPDecodedRecognitionResult>& decodedResultVec, 
			DPDecodedRecognitionResult& decodedResult)
	{
		LOG(LTKLogger::LTK_LOGLEVEL_DEBUG, "Entered DPWordRecognizer::constructTopNChoices"<<endl;
		//iterating through the decoded result vector	
		for(int vecIndex=decodedResultVec.size()-1;vecIndex>=0;--vecIndex)
		{
			
			//if confidence greater than the present element confidence
			if(decodedResult.m_confidence > decodedResultVec[vecIndex].m_confidence)
			{
				if(vecIndex+1<=decodedResultVec.size()-1)	//max bound checking
				{
					decodedResultVec[vecIndex+1]=decodedResultVec[vecIndex];	//moving lesser confidence result, down the vector
				}

				decodedResultVec[vecIndex]=decodedResult;	//replacing the current element with higher confidence decoded result
			}

			else	//if larger value is encountered
			{
				break;	//no further comparison required
			}

		}

		LOG(LTKLogger::LTK_LOGLEVEL_DEBUG, "Exiting DPWordRecognizer::constructTopNChoices"<<endl;

		return SUCCESS;

	}






/**********************************************************************************
* AUTHOR        : Thanigai 
* DATE          : 29-JUL-2005
* NAME          : createShapeRecognizer
* DESCRIPTION   : create an instance of shape recognizer object and call initialize
*				  function. Also loads the model data. 
* ARGUMENTS     : strProjectName - project name; strProfileName - profile name
* RETURNS       : handle to the recognizer on success & NULL on error
* NOTES         :
* CHANGE HISTROY
* Author            Date                Description of 
*************************************************************************************/
LTKShapeRecognizer* DPWordRecognizer::createShapeRecognizer(string strProjectName, string strProfileName)
{
    stringStringMap ProjectCfgConfigEntries;  //entries in the project config file
    stringStringMap ProfileCfgConfigEntries;  //entries in the profile config file
	string temp;   //temp strings
	string tempStr;
	int iResult = 0;
	LOG(LTKLogger::LTK_LOGLEVEL_DEBUG, "Entered DPWordRecognizer::createShapeRecognizer" );

	string strLocalProfileName(strProfileName);

	if(strProjectName == "")
	{
		errorCode = EINVALID_PROJECT_NAME;

		LOG(LTKLogger::LTK_LOGLEVEL_ERR, "Invalid or no entry for project name" );		

		return NULL; /* invalid or no entry for project name */
	}

	if(strProfileName == "")
	{
		strLocalProfileName = "default"; /* assume the "default" profile */
	}

	temp = m_lipiRoot + PROJECTS_PATH_STRING + strProjectName + PROFILE_PATH_STRING + PROJECT_CFG_STRING;
	try
	{
		ProjectCfgConfigEntries = LTKConfigFileReader::getMap(temp);
	}
	catch(LTKException e)
	{
		errorCode = EINVALID_PROJECT_NAME;

		LOG(LTKLogger::LTK_LOGLEVEL_ERR, e.getExceptionMessage() );

		return NULL; // Error exception thrown...
	}

	//	Read the project.cfg and ensure this is a shaperecognizer; i.e. ProjectType = SHAPEREC;
	if(ProjectCfgConfigEntries[PROJECT_TYPE_STRING] != "SHAPEREC")
	{
		errorCode = EINVALID_CONFIG_ENTRY;

		LOG(LTKLogger::LTK_LOGLEVEL_ERR, getError(errorCode) );

		return NULL; /* Invalid configuration entry for ProjectType */
	}

	//	Read the profile.cfg and find out the recognition module to load;
	temp = m_lipiRoot + PROJECTS_PATH_STRING + strProjectName + PROFILE_PATH_STRING + strLocalProfileName + SEPARATOR + PROFILE_CFG_STRING;
	try
	{
		ProfileCfgConfigEntries = LTKConfigFileReader::getMap(temp.c_str());
	}
	catch(LTKException e)
	{
		errorCode = EINVALID_PROJECT_NAME;

		LOG(LTKLogger::LTK_LOGLEVEL_ERR, e.getExceptionMessage() );

		return NULL; // Error exception thrown...
	}

	if(ProfileCfgConfigEntries[SHAPE_RECOGNIZER_STRING] == "")
	{
		errorCode = ENO_SHAPE_RECOGNIZER;

		LOG(LTKLogger::LTK_LOGLEVEL_ERR, getError(errorCode) );
	
		return NULL; /* No recognizer specified. */
	}

	//	Load the recognition module and Map the functions 
#ifdef _WIN32
	tempStr = m_lipiRoot + SEPARATOR + "lib" + SEPARATOR + ProfileCfgConfigEntries[SHAPE_RECOGNIZER_STRING] + DLL_EXT;
#else
	tempStr = m_lipiRoot + SEPARATOR + "lib" + SEPARATOR + "lib" + ProfileCfgConfigEntries[SHAPE_RECOGNIZER_STRING] + DLL_EXT;
#endif
	m_hAlgoDLLHandle = NULL;

	m_hAlgoDLLHandle = LTKLoadDLL(tempStr.c_str());
	if(m_hAlgoDLLHandle == NULL)
	{
		errorCode = ELOAD_SHAPEREC_DLL;

		LOG(LTKLogger::LTK_LOGLEVEL_ERR, getError(errorCode) << " : " << ProfileCfgConfigEntries[SHAPE_RECOGNIZER_STRING] <<endl;

		return NULL; // Unable to load dll
	}

	// Map Algo DLL functions...
	if((errorCode = mapShapeAlgoModuleFunctions()) != 0)
	{
		return NULL; // Unable to map the functions
	}

	LTKShapeRecognizer *pObj = NULL;

	// Call recognition module's createShapeRecognizer(); 
	pObj = module_createShapeRecognizer();
	if(pObj == NULL)
	{
		errorCode = ECREATE_SHAPEREC;

		LOG(LTKLogger::LTK_LOGLEVEL_ERR, getError(errorCode) << " : " << ProfileCfgConfigEntries[SHAPE_RECOGNIZER_STRING] <<endl;
 
		return NULL; /* Error, unable to create shape recognizer instance */
	}
		
	//Call obj->initialize() by passing the profilePath;
	iResult = pObj->initialize(strProjectName, strLocalProfileName);
	if(iResult != 0)
	{
		errorCode = iResult;
		LOG(LTKLogger::LTK_LOGLEVEL_ERR, getError(errorCode) );
		module_deleteShapeRecognizer(pObj);
		pObj = NULL;

		return NULL; /* Unable to initialize shape recognizer */
	}
	LOG(LTKLogger::LTK_LOGLEVEL_DEBUG, "Exiting DPWordRecognizer::createShapeRecognizer" );
	return pObj;
}




/**********************************************************************************
* AUTHOR        : Thanigai 
* DATE          : 29-JUL-2005
* NAME          : mapShapeAlgoModuleFunctions
* DESCRIPTION   : To map function addresses of the methods exposed by 
*				  the shape recognition modules
* ARGUMENTS     : None
* RETURNS       : 0 on success and other values on error 
* NOTES         :
* CHANGE HISTROY
* Author            Date                Description of 
*************************************************************************************/
int DPWordRecognizer::mapShapeAlgoModuleFunctions()
{
	LOG(LTKLogger::LTK_LOGLEVEL_DEBUG, "Entered BoxedFieldRecognizer::mapShapeAlgoModuleFunctions" );
	module_createShapeRecognizer = NULL;
	module_createShapeRecognizer = (FN_PTR_CREATESHAPERECOGNIZER) LTKGetDLLFunc(m_hAlgoDLLHandle, CREATESHAPERECOGNIZER_FUNC_NAME);
	if(module_createShapeRecognizer == NULL)
	{
		LOG(LTKLogger::LTK_LOGLEVEL_ERR, "Exported function not found in module" << " : createShapeRecognizer " );

		LTKReturnError(EDLL_FUNC_ADDRESS);
		// ERROR: Unable to link with createShapeRecognizer function in module */
	}

	module_deleteShapeRecognizer = (FN_PTR_DELETESHAPERECOGNIZER) LTKGetDLLFunc(m_hAlgoDLLHandle, DELETESHAPERECOGNIZER_FUNC_NAME);
	if(module_deleteShapeRecognizer == NULL)
	{
		LOG(LTKLogger::LTK_LOGLEVEL_ERR, "Exported function not found in module" << " : deleteShapeRecognizer " );

		LTKReturnError(EDLL_FUNC_ADDRESS);
	}

	LOG(LTKLogger::LTK_LOGLEVEL_DEBUG, "Exiting BoxedFieldRecognizer::mapShapeAlgoModuleFunctions" );
	return SUCCESS;
}



int DPWordRecognizer::unloadModelData()
{
	LOG(LTKLogger::LTK_LOGLEVEL_DEBUG, "Entered DPWordRecognizer::unloadModelData" );
	 //clear the recognition state
	clearRecognizerState();

	//unload the model data and 
	//delete the shape recognizer
	if( m_shapeRecognizer  && (module_deleteShapeRecognizer != NULL) )
	{
	   m_shapeRecognizer->unloadModelData();
	   module_deleteShapeRecognizer(m_shapeRecognizer);
	   m_shapeRecognizer = NULL;
	}

	//Freeing the shape recognition library
	if(m_hAlgoDLLHandle)
	{
		LTKUnloadDLL(m_hAlgoDLLHandle);
		m_hAlgoDLLHandle = NULL;
	}

	LOG(LTKLogger::LTK_LOGLEVEL_DEBUG, "Exiting DPWordRecognizer::unloadModelData" );

	return SUCCESS;

}



/**********************************************************************************
 * AUTHOR        : Vijayakumara M 
 * DATE          : 01-Aug-2005
 * NAME          : getLastError
 * DESCRIPTION   : returns last occured error description.
 * ARGUMENTS     : - NO -
 * RETURNS       : pointer to an error description 
 * NOTES         :
 * CHANGE HISTROY
 * Author            Date                Description of 
 *************************************************************************************/

int DPWordRecognizer::getLastError()
{

	return errorCode;
}







int DPWordRecognizer::loadModelData()
{

	LOG(LTKLogger::LTK_LOGLEVEL_DEBUG, "Entered DPWordRecognizer::loadModelData" );

	int totalStyles;	//stores the total number of styles
	int styleElement;	//stores element of each style
	int symbol;			//stores symbol ID
	int startShapeID;	//stores shape ID for each style

	m_maxStyleLength=0;

	int numOfShapes=-1;

	string algoVersion;					//Algorithm version.

	char versionInfo[VERSION_STR_LEN];
	string version;
	int iMajor, iMinor, iBugfix;

	stringStringMap headerSequence;

    if((errorCode = cheSumGen.readMDTHeader(m_styleModelFile,headerSequence)) != 0)
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
		LOG(LTKLogger::LTK_LOGLEVEL_ERR, "Incompatible algorithm version in style model file" << " : " << version.c_str() );

		LTKReturnError(EINCAMPATIBLE_VERSION);
	}


	ifstream input(m_styleModelFile.c_str(),ios::in | ios::binary);

    if(!input)
    {
		LOG(LTKLogger::LTK_LOGLEVEL_ERR, "Unable to open model data file" << " : " << m_styleModelFile.c_str() );

		LTKReturnError(EMODEL_DATA_FILE_OPEN);
	}

    LOG(LTKLogger::LTK_LOGLEVEL_DEBUG, "Reference style model file read" );

	input.seekg(atoi(headerSequence[HEADERLEN].c_str()),ios::beg);

	input.read((char*)&totalStyles,sizeof(int));			//reading total number of styles

	LOG(LTKLogger::LTK_LOGLEVEL_DEBUG, "Total number of styles read from style info:"<<totalStyles<<endl;

	cout<< "Total number of styles read from style info:"<<totalStyles<<" styles..."<<endl;

	//Reading each style
	for(int styleIndex=0;styleIndex<totalStyles;++styleIndex)
	{

		 int styleLength;	//stores the length of each style

		 
		 input.read((char*)&styleLength,sizeof(int));	//reading length of current style

		 if(styleLength>m_maxStyleLength)
		 {
			m_maxStyleLength=styleLength;
		 }

		 input.read((char*)&styleElement,sizeof(int));	//reading style element(cluster ID/shape ID)

		 cout<<styleElement<<"-";
		 
		 //m_styleModelStartIDs.push_back(styleElement);	//getting the first element of each style

		 startShapeID=styleElement;

		 if(styleElement>numOfShapes)
		 {
			numOfShapes=styleElement;
		 }

		 m_startShapeStyleMap.insert(make_pair(styleElement,styleIndex));	//holds mapping of starting element and the corresponding style ID

		 vector<int> eachStyleVec;	//vector which contains the elements of the current style except the first element

		 for(int eachStyleIndex=1;eachStyleIndex<styleLength;++eachStyleIndex)
		 {

			 input.read((char*)&styleElement,sizeof(int));	//reading style element
			 eachStyleVec.push_back(styleElement);			//each style is formed

			 if(styleElement>numOfShapes)
			 {
				numOfShapes=styleElement;
			 }

			 cout<<styleElement<<"-";

		 }

		 
		 m_styleModels.push_back(eachStyleVec);	//style model constructed with each style

		 
		 input.read((char*)&symbol,sizeof(int));//reading symbol ID

		 m_symbolIDs.push_back(symbol);//constructing vector of symbol IDs

		 cout<<"-"<<symbol<<endl;

		 
		 if(m_symbolStartShapesMap.find(symbol)==m_symbolStartShapesMap.end())
		 {
			set<int> startShapesSet;
			startShapesSet.insert(startShapeID);
			m_symbolStartShapesMap.insert(make_pair(symbol,startShapesSet));

		 }
		 else
		 {

			 set<int>& startShapesSet=m_symbolStartShapesMap[symbol];
			 startShapesSet.insert(startShapeID);
		 }


       
	}

	//getchar();


	++numOfShapes;	//since styleElement is zero-based, adding 1 gives the actual number of shapes

	LOG(LTKLogger::LTK_LOGLEVEL_DEBUG, "Total number of shapes:"<<numOfShapes<<endl;

	//m_styleModelStartSubset.resize(numOfShapes);	//creating m_styleModelStartSubset bool vector for total number of shapes

	m_candidateShapeIDs.resize(numOfShapes);	////creating m_candidateShapeIDs bool vector for total number of shapes


	//Indices of styleModelStartSubset corresponding to styleModelStartIDs are made true
	/*for(int startIDIndex=0;startIDIndex<m_styleModelStartIDs.size();++startIDIndex)
	{

		m_styleModelStartSubset[m_styleModelStartIDs[startIDIndex]]=true;	//start ID index made true in the bool vector

	}*/

	input.close(); //closing the input stream of dat file	
	
	input.open(m_positionModelFile.c_str(),ios::binary);	//opening input stream for relative position info file
	
	if(!input)
	{
		LOG(ERR)<<"Cannot open file "<<m_positionModelFile<<endl;
		return FAILURE;
	}

	input.read((char*)&totalStyles,sizeof(int));		//reading total number of styles

	
	LOG(LTKLogger::LTK_LOGLEVEL_DEBUG, "Total number of styles read from relative pos info:"<<totalStyles<<endl;

	int numOfFeatures;		//stores number of features to be read

	input.read((char*)&numOfFeatures,sizeof(int));	//reading number of features

	
	LOG(LTKLogger::LTK_LOGLEVEL_DEBUG, "Number of features in relative pos info:"<<numOfFeatures<<endl;
	
	//Reading relative position info for each style
	for(int posStyleIndex=0;posStyleIndex<totalStyles;posStyleIndex++)
	{
		int numOfRecords;	//stores number of records(mean plus variance vectors)

		input.read((char*)&numOfRecords,sizeof(int));	//reads number of records(mean plus variance vectors)

		float2DVector eachStylePosInfo;	//stores each style's position info

		//reading mean and variance vectors
		for(int i=0;i<numOfRecords;i++)
		{
			floatVector featureVector;	//stores the feature vector, could be mean or variance vector
		   
			//reading each feature
		   for(int ii=0;ii<numOfFeatures;ii++)
		   {
			 float value;	//stores feature value

			 input.read((char*)&value,sizeof(float));	//reading feature value

			 featureVector.push_back(value);	//constructing the feature vector        
		   }  

		   eachStylePosInfo.push_back(featureVector);	//constructing each style's position info vector
		}

		m_positionModels.push_back(eachStylePosInfo);	//constructing positional models for each style
		
		input.read((char*)&symbol,sizeof(int));		//reading symbol ID but it is not used
	   
	}


	input.close();	//closing the input stream of relative positional info file

	
	LOG(LTKLogger::LTK_LOGLEVEL_DEBUG, "Exiting DPWordRecognizer::loadTrainingData" );

	return SUCCESS;

	return SUCCESS;
}
