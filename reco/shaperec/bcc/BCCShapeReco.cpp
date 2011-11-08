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
 * FILE DESCR: Defines the entry point for the sample algorithm
 *			   DLL application(Windows)
 *
 * CONTENTS:
 *  DllMain
 *  createShapeRecognizer
 *  deleteShapeRecognizer
 *
 * CHANGE HISTORY:
 * Author       Date            Description of 
 *
 ************************************************************************/

#include "BCCShapeReco.h"
#include "LTKShapeRecognizer.h"
#include "BCCShapeRecognizer.h"
#include "LTKLoggerUtil.h"

#ifdef _WIN32

/**********************************************************************************
* NAME          : DllMain
* DESCRIPTION   : This function is applicable only in Win32 Platform. an entry point
*				  for the DLL module. 
* ARGUMENTS     : Windows standard args
* RETURNS       : TRUE
* NOTES         :
* CHANGE HISTROY
* Author            Date                Description of 
* 
*************************************************************************************/
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
* NAME          : createShapeRecognizer
* DESCRIPTION   : Creates an instance of AbcShapeRecognizer class and returns the 
*				  object 
* ARGUMENTS     : 
* RETURNS       : Handle to the newly created AbcShapeRecognizer object.
* NOTES         :
* CHANGE HISTROY
* Author            Date                Description of 
* 
*************************************************************************************/
LTKShapeRecognizer* createShapeRecognizer()
{
	return new BCCShapeRecognizer();
}

/**********************************************************************************
* NAME          : deleteShapeRecognizer
* DESCRIPTION   : Delete the LTKShapeRecognizer object
* ARGUMENTS     : LTKShapeRecognizer *obj - handle to the object to delete
* RETURNS       : 0 - Success
* NOTES         :
* CHANGE HISTROY
* Author            Date                Description of 
* 
*************************************************************************************/
int deleteShapeRecognizer(LTKShapeRecognizer *obj)
{
	if(obj != NULL)
		delete obj;

	return 0;
}

