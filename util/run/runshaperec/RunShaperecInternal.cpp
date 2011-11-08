/*****************************************************************************************
* Copyright (c) 2006 Hewlett-Packard Development Company, L.P.
* Permission is hereby granted, free of charge, to any person obtaining a copy of 
* this software and associated documentation files (the "Software"), to deal in 
* the Software without restriction, including without limitation the rights to use, 
* copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the 
* Software, and to permit persons to whom the Software is furnished to do so, 
* subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all copies or 
* substantial portions of the Software.
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
* $LastChangedDate: 
* $Revision: 
* $Author: Anish Kumar
*
************************************************************************/
#include "RunShaperec.h"
#include "LTKShapeRecoUtil.h"
#include "LTKInkFileReader.h"
#include "LTKTrace.h"
#include "LTKLipiEngineInterface.h"
#include "LTKErrors.h"
#include "LTKStringUtil.h"
#include "LTKException.h"
#include "LTKErrorsList.h"
#include "LTKConfigFileReader.h"

extern char strLipiRootPath[MAX_PATH];
extern char strOutputFileName[MAX_PATH]; 
extern float confThreshold;
extern int numChoices;
extern char strAdaptListFile[MAX_PATH];
extern char strProjectName[MAX_PATH];
extern char strProfileName[MAX_PATH];
#define ADAPTCHAR "#"
#define NEW_SHAPEID -2

/**********************************************************************************
* AUTHOR		: Anish Kumar
* DATE			: 30-AUG-2005
* NAME			: evaluateAdapt
* DESCRIPTION	: 
* ARGUMENTS		: 
* RETURNS		: -1 on error 0 on success
* NOTES			:
* CHANGE HISTROY
* Author			Date				
* 					
*************************************************************************************/
int evaluateAdapt(LTKShapeRecognizer **addReco, string projectName)
{
        int iErrorCode;
	string recognizerName;

	errorCode = getRecognizerName(strProjectName,strProfileName,recognizerName);
	if(errorCode != SUCCESS)
	{
		LOG(LTKLogger::LTK_LOGLEVEL_ERR) << "Error: " <<errorCode <<
			" " << getErrorMessage(errorCode) << "RunShaperec::evaluateAdapt()" 
			<<endl;
		LTKReturnError(errorCode);
	}

	//if recognizer name is ACTIVEDTW call evaluateAdaptActiveDTW 
	if(recognizerName == ACTIVEDTW)
	{
		errorCode = evaluateAdaptActiveDTW(addReco,projectName);
		LOG(LTKLogger::LTK_LOGLEVEL_ERR) << "Error: " <<errorCode <<
			" " << getErrorMessage(errorCode) << "RunShaperec::evaluateAdapt()" 
			<<endl;
		LTKReturnError(errorCode);
	}
	else
	{

		LTKCaptureDevice captureDevice;
		LTKScreenContext screenContext;

		vector<int> shapeSubSet; //subset of classes to be compared for recognition. If empty, all classes are compared
		vector<LTKShapeRecoResult> results;
		string path;
		int trueShapeId;

		string adaptCharCheck;
		string lineBuffer;
		ifstream adaptFileHandle;
		string line = "";
		//Flag is set when EOF is reached
		bool eofFlag = false;
		stringVector tokens;
		//ID for each shapes
		int shapeId = -1;

		LTKTraceGroup traceGroup;
		LTKShapeRecoUtil shapeRecoUtil;
		bool projectTypeDynamic = false;
		unsigned short numShapes = 0;
		string strNumShapes = "";
		string strProfileDirectory;
		string tempLipiRootPath = strLipiRootPath;
		strProfileDirectory = tempLipiRootPath + PROJECTS_PATH_STRING +projectName + PROFILE_PATH_STRING;

		string projectCFGPath;
		projectCFGPath = strProfileDirectory + PROJECT_CFG_STRING;

		
		int errorCode = shapeRecoUtil.isProjectDynamic(projectCFGPath,
			numShapes, strNumShapes, projectTypeDynamic);

		if( errorCode != SUCCESS)
		{
			LOG(LTKLogger::LTK_LOGLEVEL_ERR) << "Error: " <<errorCode <<
			" " << getErrorMessage(errorCode) << "RunShaperec::evaluateAdapt()" 
			<<endl;
			LTKReturnError(errorCode);
		}
		

		if(!projectTypeDynamic)
		{	 
			LOG(LTKLogger::LTK_LOGLEVEL_ERR) << "Error: " << errorCode << " " <<
			"RunShaperec::evaluateAdapt()" <<endl;
			LTKReturnError(EPROJ_NOT_DYNAMIC);
		}

		
	
		adaptFileHandle.open(strAdaptListFile, ios::in);

		ofstream resultfile(strOutputFileName);
	
		(*addReco)->setDeviceContext(captureDevice);

		
		int sampleCount = 1;
	
		while(!adaptFileHandle.eof())
		{		
			getline(adaptFileHandle, line, NEW_LINE_DELIMITER);
			path  = "";
			if( adaptFileHandle.eof() )
			{
				eofFlag = true;
			}
			if ( line[0] == COMMENTCHAR )
			{
				continue;
			}

			if (eofFlag == false)
			{
				/* Print the line to be parsed	*/
				cout << line <<" "<<endl;	
				sampleCount++;

				//Tokenize the string
				errorCode = LTKStringUtil::tokenizeString(line,  LIST_FILE_DELIMITER,  tokens);

				if( errorCode != SUCCESS )
				{
					LOG(LTKLogger::LTK_LOGLEVEL_ERR)<<"Error: "<< errorCode << " " <<
					"RunShaperec::evaluateAdapt()" << endl;
					LTKReturnError(errorCode);
				}

				//Tokens must be of size 2, one is pathname and other is shapeId
				//If the end of file not reached then continue the looping
				if( tokens.size() > 3 )
				{
					LOG(LTKLogger::LTK_LOGLEVEL_ERR)<<"Error: "<<
					"Number of tokens identified in the line is > 3 " <<
					line << endl;
					cout<< "Error: Number of tokens identified in the line is > 3 " <<
					line << endl;
				
					continue;
				}
				//Tokens must be of size 3, one is pathname ,second is shapeId and last is Adapt Character
				if( tokens.size() == 3)
				{
					
					if(tokens[2] != ADAPTCHAR)
					{
						LOG(LTKLogger::LTK_LOGLEVEL_ERR)<<"Error: "<<
						"Invalid token " <<
						tokens[2] << endl;
						cout<< "Error: Invalid token " << tokens[2] << endl;
					
						continue;
					}

					path = tokens[0];//Tokens[0] indicates the path name

					/* To get the Absolute Path from the ink file	*/					
					int error = getAbsolutePath(path, strLipiRootPath);

					iErrorCode = LTKInkFileReader::readUnipenInkFile(path, traceGroup, captureDevice, screenContext);
					if(iErrorCode != SUCCESS)
					{
						LOG(LTKLogger::LTK_LOGLEVEL_ERR) << getErrorMessage(iErrorCode) << endl;
						cout << "Error while reading the input file " << endl;
						cout << getErrorMessage(iErrorCode) << endl;

						LTKReturnError(iErrorCode);
					}
					//Tokens[1] indicates the shapeId
					shapeId = atoi( tokens[1].c_str() );
					if(shapeId < 0)
					{
						LOG(LTKLogger::LTK_LOGLEVEL_ERR) << "Error: "<<
						"NNShapeRecognizer requires training file class Ids to be positive integers "<<
						"RunShaperec::evaluateAdapt()" << endl;
						LTKReturnError(iErrorCode);
					}
					(*addReco)->addSample(traceGroup, shapeId);		
				}
				if( tokens.size() == 2)
				{				
					//cout<<"inside adapt character"<<endl;
					trueShapeId = atoi( tokens[1].c_str() );

					LTKTraceGroup traceGroup;
					path = tokens[0];
					/* To get the Absolute Path from the ink file	*/
					int error = getAbsolutePath(path, strLipiRootPath);

					iErrorCode = LTKInkFileReader::readUnipenInkFile(path, traceGroup, captureDevice, screenContext);
					if(iErrorCode != SUCCESS)
					{
						cout << "Error while reading the input file " << endl;
						cout << getErrorMessage(iErrorCode) << endl;
						LOG(LTKLogger::LTK_LOGLEVEL_ERR) << getErrorMessage(iErrorCode) << endl;
						LTKReturnError(iErrorCode);
					}
				
					
					iErrorCode = (*addReco)->recognize(traceGroup, screenContext, shapeSubSet, confThreshold, numChoices, results);
					if(iErrorCode != SUCCESS)
					{
						cout << "Error : " <<  getErrorMessage(iErrorCode) << endl;
						LOG(LTKLogger::LTK_LOGLEVEL_ERR) << "Error: " <<iErrorCode <<getErrorMessage(iErrorCode)
						<<" Recognize()"<<endl;
						LTKReturnError(iErrorCode);
					}

					resultfile << path << '\t' << trueShapeId << ' ';

					if (results.size() == 0)
					{
						resultfile << -1 << ' ' << -1 << ' ';
					}

					for(int index =0; index < results.size(); ++index)
					{
						resultfile << results[index].getShapeId() << ' ' << results[index].getConfidence() << ' ';
					}
					resultfile << endl;

					

					//Call Adapt after recognize
					(*addReco)->adapt(trueShapeId);
				
					

					results.clear();
				}
			}
		}
		printf("\n");
        }
	return SUCCESS;
}

int getRecognizerName(const string strProjectName,const string strProfileName, string& outRecognizerString)
{
 
    int errorCode;
    string tempLipiRootPath = strLipiRootPath;
    string profileCfgPath = tempLipiRootPath + PROJECTS_PATH_STRING + strProjectName + PROFILE_PATH_STRING + strProfileName + SEPARATOR + PROFILE_CFG_STRING;
 
    LTKConfigFileReader* profileConfigReader = NULL;
 
    try
    {
        profileConfigReader = new LTKConfigFileReader(profileCfgPath);
	
    }
    catch(LTKException e)
    {
	
        LOG( LTKLogger::LTK_LOGLEVEL_ERR) <<
            "Error: "<< getErrorMessage(e.getErrorCode()) << profileCfgPath <<
            " LTKLipiEngineModule::validateProfile()"<<endl;
 
        LTKReturnError(e.getErrorCode());
    }
 
    errorCode = profileConfigReader->getConfigValue(SHAPE_RECOGNIZER_STRING,
                                                    outRecognizerString);
   
 
    if(errorCode != SUCCESS)
    {
        
        errorCode = ENO_SHAPE_RECOGNIZER;
        
        
        LOG( LTKLogger::LTK_LOGLEVEL_ERR) <<
            "Error: "<< getErrorMessage(errorCode) << profileCfgPath <<
            " LTKLipiEngineModule::validateProfile()"<<endl;
 
        delete profileConfigReader;
 
        LTKReturnError(errorCode);
    }
 
    delete profileConfigReader;

    return SUCCESS;
 
}

int evaluateAdaptActiveDTW(LTKShapeRecognizer **addReco, const string strProjectName)
{
	int errorCode;	
	LTKCaptureDevice captureDevice;
	LTKScreenContext screenContext;

	vector<int> shapeSubSet; //subset of classes to be compared for recognition. If empty, all classes are compared
	vector<LTKShapeRecoResult> results;
	string path;

	//incase the shapeid of the sample is not specified
        //we assign it a new shape id based on the exiting shape ids

	int trueShapeId = NEW_SHAPEID;

	string adaptCharCheck;
	string lineBuffer;
	ifstream adaptFileHandle;
	string line = "";
	//Flag is set when EOF is reached
	bool eofFlag = false;

	//a flag used to detect if a new shape has been added
	bool addFlag = false;
	stringVector tokens;
	//ID for each shapes
	int shapeId = -1;

	LTKTraceGroup traceGroup;
	LTKShapeRecoUtil shapeRecoUtil;
	bool projectTypeDynamic = false;
	unsigned short numShapes = 0;
	string strNumShapes = "";
	string strProfileDirectory;
	string tempLipiRootPath = strLipiRootPath;
	strProfileDirectory = tempLipiRootPath + PROJECTS_PATH_STRING + strProjectName + PROFILE_PATH_STRING;

	string projectCFGPath;
	projectCFGPath = strProfileDirectory + PROJECT_CFG_STRING;

	errorCode = shapeRecoUtil.isProjectDynamic(projectCFGPath,
			numShapes, strNumShapes, projectTypeDynamic);

	if( errorCode != SUCCESS)
	{
		LOG(LTKLogger::LTK_LOGLEVEL_ERR) << "Error: " <<errorCode <<
		" " << getErrorMessage(errorCode) << "RunShaperec::evaluateAdaptActiveDTW()" 
		<<endl;
		LTKReturnError(errorCode);
	}
		

	if(!projectTypeDynamic)
	{	 
		LOG(LTKLogger::LTK_LOGLEVEL_ERR) << "Error: " << errorCode << " " <<
		"RunShaperec::evaluateAdaptActiveDTW()" <<endl;
		LTKReturnError(EPROJ_NOT_DYNAMIC);
	}

	adaptFileHandle.open(strAdaptListFile, ios::in);
	ofstream resultfile(strOutputFileName);
	(*addReco)->setDeviceContext(captureDevice);

	//loading model data
	errorCode = (*addReco)->loadModelData();
	

	if(errorCode != SUCCESS)
	{
		if(errorCode == EMODEL_DATA_FILE_OPEN)
		{
			LOG(LTKLogger::LTK_LOGLEVEL_DEBUG) << "Model Data File Does Not Exist" << endl;
		
			addFlag = true;
		}
		else
		{
			LOG( LTKLogger::LTK_LOGLEVEL_ERR) << "Error : " <<
			errorCode << getErrorMessage(errorCode) << "Error in evaluateAdaptActiveDTW()"
			<<endl;
			LTKReturnError(errorCode);
		}
	}

	int sampleCount = 1;
	while(!adaptFileHandle.eof())
	{
		getline(adaptFileHandle, line, NEW_LINE_DELIMITER);
		path  = "";
		
		if( adaptFileHandle.eof() )
		{
			eofFlag = true;
		}
		if ( line[0] == COMMENTCHAR )
		{
			continue;
		}

		if (eofFlag == false)
		{

			/* Print the line to be parsed	*/
			cout << line <<" "<<sampleCount<< endl;	
			sampleCount++;

			//Tokenize the string
			errorCode = LTKStringUtil::tokenizeString(line,  LIST_FILE_DELIMITER,  tokens);

			if( errorCode != SUCCESS )
			{
				LOG(LTKLogger::LTK_LOGLEVEL_ERR)<<"Error: "<< errorCode << " " <<
				"RunShaperec::evaluateAdapt()" << endl;
				LTKReturnError(errorCode);
			}

			//Tokens must be of size 2, one is pathname and other is shapeId
				//If the end of file not reached then continue the looping
			//if( tokens.size() > 2 )
			if( tokens.size() > 3 )
			{
				LOG(LTKLogger::LTK_LOGLEVEL_ERR)<<"Error: "<<
				"Number of tokens identified in the line is > 2 " <<
				line << endl;
				cout<< "Error: Number of tokens identified in the line is > 2 " <<
				line << endl;
				
				continue;
			}
			if( tokens.size() == 3)
			{

				if(tokens[2] != ADAPTCHAR)
				{
					LOG(LTKLogger::LTK_LOGLEVEL_ERR)<<"Error: "<<
						"Invalid token " <<
						tokens[2] << endl;
					cout<< "Error: Invalid token " << tokens[2] << endl;

					continue;
				}
			}

			trueShapeId = atoi( tokens[1].c_str() );
			
			LTKTraceGroup traceGroup;
			path = tokens[0];
			/* To get the Absolute Path from the ink file	*/
			int error = getAbsolutePath(path, strLipiRootPath);

			errorCode = LTKInkFileReader::readUnipenInkFile(path, traceGroup, captureDevice, screenContext);
			if(errorCode != SUCCESS)
			{
				cout << "Error while reading the input file " << endl;
				cout << getErrorMessage(errorCode) << endl;
				LOG(LTKLogger::LTK_LOGLEVEL_ERR) << getErrorMessage(errorCode) << endl;
				LTKReturnError(errorCode);
			}

			//if addFlag set to true add class 
			if(addFlag)
			{
				errorCode = (*addReco)->addClass(traceGroup,trueShapeId);
				if(errorCode != SUCCESS)
				{
					LOG(LTKLogger::LTK_LOGLEVEL_ERR) << "Error: " <<errorCode <<getErrorMessage(errorCode)
					<<"RunshaperecInternal:addClass()"<<endl;
					LTKReturnError(errorCode);				
				}
			}
			else
			{
				//call recognize
				errorCode = (*addReco)->recognize(traceGroup, screenContext, shapeSubSet, confThreshold, numChoices, results);
				if(errorCode != SUCCESS)
				{
					LOG(LTKLogger::LTK_LOGLEVEL_ERR) << "Error: " <<errorCode <<getErrorMessage(errorCode)
					<<"RunshaperecInternal:recognize()"<<endl;
					LTKReturnError(errorCode);				
				}

				resultfile << path << '\t' << trueShapeId << ' ';

				if (results.size() == 0)
				{
					resultfile << -1 << ' ' << -1 << ' ';
				}

				for(int index =0; index < results.size(); ++index)
				{
					resultfile << results[index].getShapeId() << ' ' << results[index].getConfidence() << ' ';
				}
				resultfile << endl;

				errorCode = (*addReco)->adapt(trueShapeId);

				if(errorCode != SUCCESS)
				{
					if(errorCode == EINVALID_SHAPEID)
					{
						errorCode = (*addReco)->addClass(traceGroup,trueShapeId);

						if(errorCode != SUCCESS)
						{
							LOG(LTKLogger::LTK_LOGLEVEL_ERR) << "Error: " <<errorCode <<getErrorMessage(errorCode)
							<<" addClass()"<<endl;
							LTKReturnError(errorCode);
						}
					}
					else
					{
						LOG(LTKLogger::LTK_LOGLEVEL_ERR) << "Error: " <<errorCode <<getErrorMessage(errorCode)
						<<" adapt()"<<endl;
						LTKReturnError(errorCode);
					}
				}
			}
			
			results.clear();
			
		}
		//resetting adaptFlag
		addFlag = false;
	}

	return SUCCESS;
	
}
