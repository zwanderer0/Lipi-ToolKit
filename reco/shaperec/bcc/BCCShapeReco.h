/****************************************************************************
* Copyright (c) HP Labs India. All rights reserved.
* This source code is a property of HP Labs India. and is copyright protected.
* UNATHORISED ACCESS, COPYING, REPRODUCTION OR DISTRIBUTION of this program
* is strictly PROHIBITED.
****************************************************************************/

/************************************************************************
 * SVN MACROS
 *
 * $LastChangedDate: 2007-09-12 15:47:40 +0530 (Wed, 12 Sep 2007) $
 * $Revision: 188 $
 * $Author: sharmnid $
 *
 ************************************************************************/

/************************************************************************
 * FILE DESCR: Header file for sample shape recognition algorithm DLL module
 *
 * CONTENTS:	prototypes & include files list
 *  
 * CHANGE HISTORY:
 * Author       Date            Description
 *
 ************************************************************************/

#ifndef __BCC_H__
#define __BCC_H__

#ifdef _WIN32

#include <windows.h>

// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the BCC_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// BCC_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.
#ifdef BCC_EXPORTS
#define BCC_API __declspec(dllexport)
#else
#define BCC_API __declspec(dllimport)
#endif //#ifdef BCC_EXPORTS
#else
#define BCC_API
#endif	//#ifdef _WIN32

/**
 *  Crates instance of type PCAShaperecongnizer and retuns of type 
 *  LTKShpeRecongizer. (Acts as a Factory Method).
 *
 * @param  void - No argument
 *
 * @return  LTKShapeRecognizer - an instace of type LTKShapeRecoginzer.
 */

/**Creates Instance of the Derived Class (BCCnginzer) and returs of type Base class (LTKShapeRecognizer) **/
extern "C" BCC_API LTKShapeRecognizer* createShapeRecognizer();

/**
 * Destry the instance by taking the address as its argument.
 *
 * @param  obj - Address of LTKShapeRecognizer instnace.
 *
 * @return  0 on Success
 */

/**Destroys the instance by taking its addess as its argument. **/
extern "C" BCC_API int deleteShapeRecognizer(LTKShapeRecognizer *obj);

#endif //#ifndef __BCC_H__
