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
 * FILE DESCR: Definition of DPPreProcessor contains the specific pre-processing
 *			   techniques like 'shiro-rekha' detection
 *
 * CONTENTS:   
 *
 * AUTHOR:     Mudit Agrawal
 *
 * DATE:       Mar 4, 2005
 * CHANGE HISTORY:
 * Author		Date			Description of 
 * 
 ************************************************************************/

#ifndef __DPPREPROCESSOR_H
#define __DPPREPROCESSOR_H

#include "LTKInkUtils.h"

/**
* @class DPPreProcessor
* <p> This class contains the specific pre-processing techniques like 'shiro-rekha' detection </p>
*/

class DPPreProcessor  
{


public:
	/**
	 * @name Constructors and Destructor
	 */
	// @{

	/**
	 * Default Constructor
	 */

	DPPreProcessor();

	/** Destructor */
	
	~DPPreProcessor();
	// @}

	/**
	 * This function returns the stroke(s) which are shiro-rekha
	 * @param void
	 *
	 * @return stroke-id of shiro-rekha
	 */

	int detectShiroRekha(const LTKTraceGroup &traceGroup, vector<int> & shiroRekhaIndices);
    int detectShiroRekha(const LTKTraceVector &traceVector, vector<int> & shiroRekhaIndices);
};

#endif 
