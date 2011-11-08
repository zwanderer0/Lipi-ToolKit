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
 * FILE DESCR: Definitions for EigDef dll exporting functions.
 *
 * CONTENTS:   
 *
 * AUTHOR:     Vijayakumara M.
 *
 * DATE:       Aug 08, 2005
 * CHANGE HISTORY:
 * Author		Date			Description
 ************************************************************************/
#ifndef __EigDef_H__
#define __EigDef_H__

#ifdef _WIN32
// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the EigDef_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// EigDef_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.
#ifdef EigDef_EXPORTS
#define Eigdef_API __declspec(dllexport)
#else
#define EigDef_API __declspec(dllimport)
#endif
#else
#define EigDef_API
#endif	//#ifdef _WIN32

#include "LTKShapeRecognizer.h"
#include "EigDefShapeRecognizer.h"
#include "LTKLoggerUtil.h"
void *m_libHandler;

/**
 *  Crates instance of type EigDefShaperecongnizer and retuns of type 
 *  LTKShpeRecongizer. (Acts as a Factory Method).
 *
 * @param  void - No argument
 *
 * @return  LTKShapeRecognizer - an instace of type LTKShapeRecoginzer.
 */

//Creates Instance of the Derived Class (EigDefShapeReconginzer) and returs of type Base class (LTKShapeRecognizer)
extern "C" EigDef_API LTKShapeRecognizer* createShapeRecognizer();

/**
 * Destry the instance by taking the address as its argument.
 *
 * @param  obj - Address of LTKShapeRecognizer instnace.
 *
 * @return  0 on Success
 */

//Destroys the instance by taking its addess as its argument.
extern "C" EigDef_API int deleteShapeRecognizer(LTKShapeRecognizer *obj);
#endif //#ifndef __EigDef_H__
