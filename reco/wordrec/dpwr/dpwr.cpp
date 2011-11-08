/************************************************************************
 * SVN MACROS
 *
 * $LastChangedDate: 2008-07-18 15:00:39 +0530 (Fri, 18 Jul 2008) $
 * $Revision: 561 $
 * $Author: sharmnid $
 *
 ************************************************************************/
#include "dpwr.h"


#ifdef _WIN32

//DLL MAIN for Windows
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
* AUTHOR		: Deepu V
* DATE			: 23-Aug-2005
* NAME			: createWordRecognizer
* DESCRIPTION	: Crates instance of type DP Word Recongnizer and retuns of type 
				  LTKWordRecongizer. (Acts as a Factory Method).
* ARGUMENTS		: 
* RETURNS		: returns an instace of type LTKWordRecoginzer.
* NOTES			: 
* CHANGE HISTORY
* Author			Date				Description of 
*************************************************************************************/
LTKWordRecognizer* createWordRecognizer()
{
	return new DPWordRecognizer();
}

/**********************************************************************************
* AUTHOR		: Deepu V
* DATE			: 23-Aug-2005
* NAME			: deleteWordRecognizer
* DESCRIPTION	: Destroys the instance by taking its addess as its argument.
* ARGUMENTS		: 
* RETURNS		: 
* NOTES			: 
* CHANGE HISTORY
* Author			Date				Description of 
*************************************************************************************/
int deleteWordRecognizer(LTKWordRecognizer *obj)
{
	delete obj;
	return 0;
}
