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

#include <tchar.h>
using namespace std;

class Common
{
public:

	static bool PathRemoveFileSpec(LPTSTR pszPath)
	{
		basic_string<TCHAR>  szPathName(pszPath);

		int index = szPathName.find_last_of ( _T("\\") );
		if( -1 == index )
		{
			return false;
		}

		_tcscpy(pszPath, szPathName.erase( index + 1 , 
						 szPathName.size() - (index + 1) ).c_str());
		return true;
	}
	static bool GetAppPath(LPTSTR pszAppName, LPTSTR pszPathName)
	{
		::GetModuleFileName(::GetModuleHandle(pszAppName),pszPathName,MAX_PATH);
		Common::PathRemoveFileSpec(pszPathName);
		return false;
	}
};
