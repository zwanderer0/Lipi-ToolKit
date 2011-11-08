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
 * $LastChangedDate: 2007-10-01 14:32:49 +0530 (Mon, 01 Oct 2007) $
 * $Revision: 224 $
 * $Author: dineshm $
 *
 ************************************************************************/

/************************************************************************
 * FILE DESCR: Defines the entry point for the DLL application.
 *
 * CONTENTS:
 *	DllMain
 *	createShapeRecognizer
 *	deleteShapeRecognizer
 *
 * AUTHOR:  VijayaKumara M. 
 *
 * DATE:       
 * CHANGE HISTORY:
 * Author		Date			Description of change
 ************************************************************************/
#include "PCA.h"

#ifdef _WIN32
#include <windows.h>
BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			break;
    }
    return TRUE;
}
#endif

/**********************************************************************************
* AUTHOR		: Vijayakumara M
* DATE			: 28-July-2005
* NAME			: createShapeRecoginizer
* DESCRIPTION	: Crates instance of type PCAShaperecongnizer and retuns of type 
				  LTKShpeRecongizer. (Acts as a Factory Method).
* ARGUMENTS		: string strlLogicalProjectName.
* RETURNS		: returns an instace of type LTKShapeRecoginzer.
* NOTES			: 
* CHANGE HISTORY
* Author			Date				Description of change
*************************************************************************************/
LTKShapeRecognizer* createShapeRecognizer(LTKControlInfo controlInfo)
{
	try
	{
		return new PCAShapeRecognizer(controlInfo);
	}
	catch(LTKException e)
	{
		return NULL;
	}
}

/**********************************************************************************
* AUTHOR		: Vijayakumara M 
* DATE			: 28-July-2005
* NAME			: deleteShapeRecoginzer
* DESCRIPTION	: Destry the instance by taking the address as its argument.
* ARGUMENTS		: Address of LTKShapeRecognizer instnace.
* RETURNS		: Returns 0 on Success
* NOTES			: 
* CHANGE HISTORY
* Author			Date				Description of change
*************************************************************************************/
int deleteShapeRecognizer(LTKShapeRecognizer *obj)
{
	if (obj != NULL )
	{
		delete obj;
		obj = NULL;

		unloadDLLs();
	}
	return 0;
}


/**********************************************************************************
* AUTHOR        : Nidhi Sharma 
* DATE          : 
* NAME          : 
* DESCRIPTION   : 
* ARGUMENTS     : 
* RETURNS       : 
* NOTES         :
* CHANGE HISTROY
* Author            Date                Description of change
*************************************************************************************/
void unloadDLLs()
{
	// Unload feature extractor DLL
	if(m_libHandlerFE != NULL)
	{
		//Unload the DLL
		LTKUnloadDLL(m_libHandlerFE);
		m_libHandlerFE = NULL;
	}
}
