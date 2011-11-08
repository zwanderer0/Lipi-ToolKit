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
 * FILE DESCR: Definitions of EigDef feature extraction module
 *
 * CONTENTS:   
 *
 * AUTHOR:     Balaji R.
 *
 * DATE:       December 23, 2004
 * CHANGE HISTORY:
 * Author		Date			Description
 ************************************************************************/

#ifndef __EigDefFEATUREEXTRACTOR_H
#define __EigDefFEATUREEXTRACTOR_H

#include "LTKTraceGroup.h"

/**
 * @class EigDefFeatureExtractor
 * <p> This class extracts the features of the preprocessed shape.<p> 
 */

class EigDefFeatureExtractor
{
public:

	/** @name Constructors and Destructor */
	//@{

	/**
	 * Default Constructor.
	 */

	EigDefFeatureExtractor();

	/**
	* Destructor
	*/

	~EigDefFeatureExtractor();

	// @}


	/**
	 * @name Methods
	 */

	// @{

	/**
	 * This is a static method which does the feature extraction from the preprocessed
	 * shape traceGroupObj
	 * @param traceGroupObj The input shape
	 * @param featureVector The feature vector extracted
	 */

	static int extractFeatures(LTKTraceGroup traceGroupObj, floatVector& featureVector); 

	// @}

};

#endif

//#ifndef __EigDefFEATUREEXTRACTOR_H
//#define __EigDefFEATUREEXTRACTOR_H
