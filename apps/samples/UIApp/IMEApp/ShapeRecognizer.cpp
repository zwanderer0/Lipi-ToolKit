/*****************************************************************************************
* Copyright (c) 2006 Hewlett-Packard Development Company, L.P.
* Permission is hereby granted, free of charge, to any person obtaining a copy of 
* this software and associated documentation files (the "Software"), to deal in 
* the Software without restriction, including without limitation the rights to use, 
* copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the 
* Software, and to permit persons to whom the Software is furnished to do so, 
* subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
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
 * $LastChangedDate:  $
 * $Revision:  $
 * $Author:  $
 *
 ************************************************************************/
/************************************************************************
 * FILE DESCR	: 
 * 
 * AUTHOR		:  Balaji MNA
 * Version		:  1.0
 * Date			:  10-Dec-2008
 * CHANGE HISTORY:
 * Author       Date            Description of change
 ************************************************************************/

#include "StdAfx.h"
#include "LTKChannel.h"
#include "LTKTraceFormat.h"
#include "ShapeRecognizer.h"
#include "LTKOSUtilFactory.h"
#include "LTKMacros.h"
#include "LTKErrorsList.h"
#include "LTKErrors.h"
#include "LTKLogger.h"

CShapeRecognizer::CShapeRecognizer() :
	m_OSUtilPtr(LTKOSUtilFactory::getInstance()),
	m_ptrLipiEngineHandle(NULL)
{
	m_LipiEngine = NULL;
	m_ShapeRec =  NULL;

	//Get the LIPI_ROOT environment variable 
	_tcscpy(envstring,_T(""));
	Common::GetAppPath(NULL,envstring);

}

CShapeRecognizer::~CShapeRecognizer()
{
	unloadModelData();
	if(NULL != fptrDeleteLTKLipiEngine)
	{
		fptrDeleteLTKLipiEngine() ;
	}

	unloadLipiInterface();

	delete m_OSUtilPtr;

}

/**********************************************************************************
* AUTHOR		: Balaji MNA
* DATE			: 29-Nov-2008
* NAME			: recognize
* DESCRIPTION	: sends strokes for recognition
* ARGUMENTS		: vector<CPoint> - stroke points
*                 int - no of choices
*                 LTKScreenContext - screen coordinates
*                 vector<LTKShapeRecoResult> - output recognized result set
* RETURNS		: return 0 - successfully recognize
* NOTES			: 
* CHANGE HISTROY
* ************************************************************************************/

int CShapeRecognizer::recognize(vector<vector<CPoint>>& strokePoints, int numChoice, 
								vector<LTKShapeRecoResult>& resultSet)
{
	if(!m_ShapeRec && (strokePoints.size() != 0))
		return FAILURE;

	vector<int> oSubSetOfClasses;
	LTKTraceGroup traceGroup;
	LTKScreenContext hscreenContext;

	// add the points to TraceGroup
	convertPointsToTraceGroup(strokePoints,traceGroup);		
	resultSet.reserve(numChoice);
	
	float confThreshold = 0.00 ;

	//Set DPI value to shape recognizer
	LTKCaptureDevice oDevInfo;
	oDevInfo.setXDPI(96);
	oDevInfo.setYDPI(96);
	m_ShapeRec->setDeviceContext(oDevInfo);

	
	int iResult = m_ShapeRec->recognize(traceGroup, hscreenContext, 
		oSubSetOfClasses, confThreshold, numChoice, resultSet);	

	if ((resultSet.size() == 0) || (iResult != SUCCESS))
		return iResult;

	return SUCCESS;
}
/**********************************************************************************
* AUTHOR		: Balaji MNA
* DATE			: 29-Nov-2008
* NAME			: loadModelData
* DESCRIPTION	: loads model data for project and for profile
* ARGUMENTS		: string - project
*                 string - profile
* RETURNS		: return 0 - successfully loads model data
* NOTES			: 
* CHANGE HISTROY
* ************************************************************************************/

int CShapeRecognizer::loadModelData(string strProfectName,string strProfileName)
{
	if(m_LipiEngine->createShapeRecognizer(strProfectName, 
					strProfileName, 
					&m_ShapeRec) != 0)
		return -1;



	//Load the model data into memory before starting the recognition...
	if(m_ShapeRec->loadModelData() != 0)
	{	
		m_LipiEngine->deleteShapeRecognizer(&m_ShapeRec);
		return FAILURE;
	}

	return SUCCESS;
}

/**********************************************************************************
* AUTHOR		: Balaji MNA
* DATE			: 29-Nov-2008
* NAME			: unloadModelData
* DESCRIPTION	: unloads model data
* ARGUMENTS		: none
* RETURNS		: none
* NOTES			: 
* CHANGE HISTROY
* ************************************************************************************/

void CShapeRecognizer::unloadModelData()
{
	if(m_ShapeRec != NULL)
	{	
		m_ShapeRec->unloadModelData();
		m_LipiEngine->deleteShapeRecognizer(&m_ShapeRec);
		m_ShapeRec = NULL;
	}
}

/**********************************************************************************
* AUTHOR		: Balaji MNA
* DATE			: 29-Nov-2008
* NAME			: loadLipiInterface
* DESCRIPTION	: loads lipi engine modules
* ARGUMENTS		: none
* RETURNS		: return 0 - successfully load lipi engine
* NOTES			: 
* CHANGE HISTROY
* ************************************************************************************/

int CShapeRecognizer::loadLipiInterface()
{
#ifdef _WIN32_WCE
	char environmentVal[MAX_PATH]={0};
	wcstombs(environmentVal,envstring,MAX_PATH);
#else
	char *environmentVal = getenv("LIPI_ROOT");
#endif

	int returnVal = m_OSUtilPtr->loadSharedLib(environmentVal, 
			LIPIENGINE_MODULE_STR, &m_ptrLipiEngineHandle);

	if(returnVal != SUCCESS)
		return -1;

    // Map createLTKLipiEngine and deleteLTKLipiEngine functions
	void* functionHandle = NULL;
	returnVal = m_OSUtilPtr->getFunctionAddress(m_ptrLipiEngineHandle, 
											"createLTKLipiEngine", 
											&functionHandle);
	// Could not map the createLTKLipiEngine function from the DLL
	if(returnVal != SUCCESS)
	{
		//Unload the dll
		unloadLipiInterface();
		return -1;
	}

	fptrCreateLTKLipiEngine = (FN_PTR_CREATELTKLIPIENGINE) functionHandle;

	functionHandle = NULL;
	returnVal = m_OSUtilPtr->getFunctionAddress(m_ptrLipiEngineHandle, 
											"deleteLTKLipiEngine", 
											&functionHandle);
	// Could not map the deleteLTKLipiEngine function from the DLL
	if(returnVal != SUCCESS)
	{
		//Unload the dll
		unloadLipiInterface();
		return -1;
	}
	fptrDeleteLTKLipiEngine = (FN_PTR_DELETELTKLIPIENGINE)functionHandle;

	//create an instance of LipiEngine Module		
	if(m_LipiEngine == NULL)
	{	
		m_LipiEngine = fptrCreateLTKLipiEngine();
		m_LipiEngine->setLipiRootPath(environmentVal);
		string logfile(environmentVal);
		logfile += "\\lipi.log";
		m_LipiEngine->setLipiLogFileName(logfile);
		m_LipiEngine->setLipiLogLevel("Error");
		if(m_LipiEngine->initializeLipiEngine() == -1)
			return FAILURE;
	}

	return SUCCESS;
}
/**********************************************************************************
* AUTHOR		: Balaji MNA
* DATE			: 29-Nov-2008
* NAME			: unloadLipiInterface
* DESCRIPTION	: unloads lipi engine modules
* ARGUMENTS		: none
* RETURNS		: return 0 - successfully load lipi engine
* NOTES			: 
* CHANGE HISTROY
* ************************************************************************************/

int CShapeRecognizer::unloadLipiInterface()
{
    //Check the lipiengine DLL was loaded already
    if(m_ptrLipiEngineHandle != NULL)
    {
        //Unload the DLL
		m_OSUtilPtr->unloadSharedLib(m_ptrLipiEngineHandle);
        m_ptrLipiEngineHandle = NULL;
    }
	return SUCCESS;
}
/**********************************************************************************
* AUTHOR		: Anish Kumar
* DATE			: 10-Dec-2008
* NAME			: convertPointsToTraceGroup
* DESCRIPTION	: Function to copy the ink vector in LTKTraceGroup vector
* ARGUMENTS		: oTraceGroup : Vector which will store the ink data
*				  xypts		  : Vector which has the ink data 
* RETURNS		: returns 0 on success and any other value on error. 
* NOTES			: 
* CHANGE HISTROY
* ************************************************************************************/
int CShapeRecognizer::convertPointsToTraceGroup(vector<vector<CPoint>>& strokePoints,
												LTKTraceGroup &oTraceGroup)
{	

	int countpoints = 0;
	for (vector<vector<CPoint>>::iterator strokes = strokePoints.begin();
		strokes<strokePoints.end();strokes++)
	{
		vector<LTKChannel> channels;			//	channels of a trace 
		LTKChannel xChannel("X", DT_INT, true);	//	x-coordinate channel of the trace 
		LTKChannel yChannel("Y", DT_INT, true);	//	y-coordinate channel of the trace		

		//	initializing the channels of the trace
		channels.push_back(xChannel);	
		channels.push_back(yChannel);

		//	composing the trace format object
		LTKTraceFormat traceFormat(channels);
		LTKTrace trace(traceFormat);

		for (vector<CPoint>::iterator it = (*strokes).begin();
			it<(*strokes).end();it++)
		{
			countpoints++;
			
			if(2 == countpoints ||
				3 == countpoints ||
				4 == countpoints)
				continue;

			vector<float> point;
			point.push_back((float)(*it).x);
			point.push_back((float)(*it).y);				
			trace.addPoint(point);
		}
		oTraceGroup.addTrace(trace);
	}

	return 0;
}


