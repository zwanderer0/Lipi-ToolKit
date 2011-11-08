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
 * FILE DESCR: Definitions for the Word Preprocessing Module
 *
 * CONTENTS:   
 *
 * AUTHOR:     Deepu V.
 *
 * DATE:       February 21, 2005
 * CHANGE HISTORY:
 * Author		Date			Description of change
 ************************************************************************/

#ifndef __LTKWORDPREPROCESSOR_H 
#define __LTKWORDPREPROCESSOR_H 

#include "LTKInc.h"

#include "LTKTypes.h"

#include "LTKTraceGroup.h"

#include "LTKTrace.h"

#include "LTKRecognitionContext.h"


typedef vector<LTKTrace> LTKTraceVector;
typedef vector<LTKTraceGroup> LTKTraceGroupVector;

/** 
 * @class LTKWordPreprocessor This class perform word level size normalization
 * 
 */
class LTKWordPreprocessor  
{
public:
	/**
	* @name Constructors and Destructor
	*/

	// @{

	/**
	 * Default Constructor
	 */
	LTKWordPreprocessor();
   
	/**
	* Destructor
	*/
	
	~LTKWordPreprocessor();
	// @}

	/**
	 * @name Size normalization methods
	 */

	// @{

	/**
	 * This is a static method that normalizes size of a word 
	 * reference lines are estimated and the word is normalized 
	 * so that the new reference lines are y = 0 and y = 1
	 * @param inTraces           traces to be preprocessed
	 * @param preProcessedTraces preprocessed traces
	 * @return SUCCESS/FAILURE
	 */

	static int normalizeWordSize(const vector<LTKTrace>& inTraces, vector<LTKTrace>& preProcessedTraces);

	/**
	 * This is a static method that normalizes size of a word 
	 * reference lines are estimated and the word is normalized 
	 * so that the new reference lines are y = 0 and y = 1
	 * @param inInk  traces to be preprocessed
	 * @param preProcessedInk preprocessed traces
	 * @return SUCCESS/FAILURE
	 */
	static int normalizeWordSize(const LTKTraceGroup& inInk, LTKTraceGroup& preProcessedInk);

	/**
	 * This is a static method that normalizes size of a word. The 
	 * reference lines are given through the screen context obj 
	 * as sc.getHlines().at(0) and sc.getHlines().at(1) 
	 * @param inTraces           traces to be preprocessed
	 * @param sc                 screen context that contains the reference lines
	 * @param preProcessedTraces preprocessed traces
	 * @return SUCCESS/FAILURE
	 */
	static int normalizeWordSize(const vector<LTKTrace>& inTraces, const LTKScreenContext& sc ,vector<LTKTrace>& preProcessedTraces, int startTraceIndex =0, int endTraceIndex = -1);

	/**
	 * This is a static method that normalizes size of a word. The 
	 * reference lines are given through the screen context obj 
	 * as sc.getHlines().at(0) and sc.getHlines().at(1) 
	 * @param inink              traces to be preprocessed
	 * @param sc                 screen context that contains the reference lines
	 * @param preProcessedInk    preprocessed traces
	 * @return SUCCESS/FAILURE
	 */
	static int normalizeWordSize(const LTKTraceGroup& inInk, const LTKScreenContext& sc, LTKTraceGroup& preProcessedInk);

	//@}


};

#endif //#ifndef __LTKWORDPREPROCESSOR_H 
