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
 * $LastChangedDate: 2008-07-18 15:00:39 +0530 (Fri, 18 Jul 2008) $
 * $Revision: 561 $
 * $Author: sharmnid $
 *
 ************************************************************************/

/************************************************************************
 * FILE DESCR: Definitions for PCA dll exporting functions.
 *
 * CONTENTS:   
 *
 * AUTHOR:     Vijayakumara M.
 *
 * DATE:       Aug 08, 2005
 * CHANGE HISTORY:
 * Author		Date			Description of change
 ************************************************************************/
#ifndef __PCA_H__
#define __PCA_H__

#ifdef _WIN32
// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the PCA_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// PCA_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.
#ifdef PCA_EXPORTS
#define PCA_API __declspec(dllexport)
#else
#define PCA_API __declspec(dllimport)
#endif
#else
#define PCA_API
#endif	//#ifdef _WIN32


#include "LTKShapeRecognizer.h"
#include "PCAShapeRecognizer.h"
#include "LTKLoggerUtil.h"

void *m_libHandler;
void *m_libHandlerFE;

/** @defgroup PCAShapeRecognizer PCAShapeRecognizer
*@brief The PCAShapeRecognizer
*/

/**
* @ingroup PCAShapeRecognizer
* @file PCA.h
*/

/**
 *  Crates instance of type PCAShaperecongnizer and retuns of type 
 *  LTKShpeRecongizer. (Acts as a Factory Method).
 *
 * @param  void - No argument
 *
 * @return  LTKShapeRecognizer - an instace of type LTKShapeRecoginzer.
 */

//Creates Instance of the Derived Class (PCAShapeReconginzer) and returs of type Base class (LTKShapeRecognizer)
extern "C" PCA_API LTKShapeRecognizer* createShapeRecognizer(LTKControlInfo controlInfo);

/**
 * Destry the instance by taking the address as its argument.
 *
 * @param  obj - Address of LTKShapeRecognizer instnace.
 *
 * @return  0 on Success
 */

//Destroys the instance by taking its addess as its argument.
extern "C" PCA_API int deleteShapeRecognizer(LTKShapeRecognizer *obj);

void unloadDLLs();

#endif //#ifndef __PCA_H__
