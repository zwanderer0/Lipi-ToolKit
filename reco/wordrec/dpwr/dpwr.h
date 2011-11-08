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
 * FILE DESCR: Definitions for DPWR dll exporting functions.
 *
 * CONTENTS:   
 *
 * AUTHOR:     Bharath A.
 *
 * DATE:       Aug 23, 2005
 * CHANGE HISTORY:
 * Author		Date			Description of 
 ************************************************************************/
#ifndef __DPWR_H__
#define __DPWR_H__

#ifdef _WIN32
#include <windows.h>

#ifdef DPWR_EXPORTS
#define DPWR_API __declspec(dllexport)
#else
#define DPWR_API __declspec(dllimport)
#endif
#else
#define DPWR_API
#endif	//#ifdef _WIN32


#include "LTKWordRecognizer.h"
#include "DPWordRecognizer.h"

void *m_hAlgoDLLHandle;

/**
 *  Creates an instance of DPWordRecognizer. Acts as Factory Method
 *
 * @param  void - No argument
 *
 * @return  LTKWordRecognizer - an instace of type LTKWordRecognizer.
 */

//Creates Instance of the Derived Class (DPWordRecognizer) and returs of type Base class (LTKWordRecognizer)
extern "C" DPWR_API LTKWordRecognizer* createWordRecognizer();

/**
 * Destroy the instance by taking the address as its argument.
 *
 * @param  obj - Address of LTKWordRecognizer instnace.
 *
 * @return  0 on Success
 */

//Destroys the instance by taking its addess as its argument.
extern "C" DPWR_API int deleteWordRecognizer(LTKWordRecognizer *obj);

#endif //#ifndef __DPWR_H__
