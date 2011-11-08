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
#pragma once

#include "LTKLipiEngineInterface.h"
#include "LTKMacros.h"
#include "LTKInc.h"
#include "LTKTypes.h"
#include "LTKTrace.h"
#include "LTKOSUtil.h"

class CShapeRecognizer
{
public:
	CShapeRecognizer();
	~CShapeRecognizer();
	
	int recognize(vector<vector<CPoint>>& strokePoints, int numChoice, 
				  vector<LTKShapeRecoResult>& resultSet);
	int loadModelData(string strProfectName,string strProfileName="default");
	int loadLipiInterface();
	void unloadModelData();

private:
	int convertPointsToTraceGroup(vector<vector<CPoint>>& strokePoints,LTKTraceGroup &oTraceGroup);
	int unloadLipiInterface();

	// Function pointer declarations for exported functions from algorithm DLL...
	typedef LTKLipiEngineInterface* (*FN_PTR_CREATELTKLIPIENGINE) (void);
	FN_PTR_CREATELTKLIPIENGINE fptrCreateLTKLipiEngine;

	typedef void (*FN_PTR_DELETELTKLIPIENGINE) (void);
	FN_PTR_DELETELTKLIPIENGINE fptrDeleteLTKLipiEngine;

	void* m_ptrLipiEngineHandle;

	LTKLipiEngineInterface *m_LipiEngine;
	LTKShapeRecognizer *m_ShapeRec;
	
	LTKOSUtil*  m_OSUtilPtr;

	TCHAR envstring[MAX_PATH];
};

