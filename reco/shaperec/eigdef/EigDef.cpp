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
 * Author		Date			Description
 ************************************************************************/
#include "EigDef.h"
#include "LTKLoggerUtil.h"

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
* DESCRIPTION	: Crates instance of type EigDefShaperecongnizer and retuns of type 
				  LTKShpeRecongizer. (Acts as a Factory Method).
* ARGUMENTS		: string strlLogicalProjectName.
* RETURNS		: returns an instace of type LTKShapeRecoginzer.
* NOTES			: 
* CHANGE HISTORY
* Author			Date				Description
*************************************************************************************/
LTKShapeRecognizer* createShapeRecognizer()
{
	return new EigDefShapeRecognizer();
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
* Author			Date				Description
*************************************************************************************/
int deleteShapeRecognizer(LTKShapeRecognizer *obj)
{
	return 0;
}
