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
 * FILE DESCR: Definitions for DP word recognizer 
 *
 * CONTENTS:   
 *
 * AUTHOR:     Deepu V.
 *
 * DATE:       March 23, 2005
 * CHANGE HISTORY:
 * Author		Date			Description of 
 ************************************************************************/

#ifndef __DPWORDRECOGNIZER_H
#define __DPWORDRECOGNIZER_H


#ifdef _WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif



#include <deque>

#include "LTKInc.h"
#include "LTKTypes.h"
#include "LTKWordRecoConfig.h"
#include "LTKWordRecognizer.h"
#include "LTKShapeRecoEngine.h"
#include "LTKRecognitionContext.h"
#include "LTKWordRecoResult.h"
#include "LTKWordPreprocessor.h"
#include "LTKGrammar.h"
#include "LTKDictionary.h"
#include "DPPreProcessor.h"
#include "LTKMacros.h"
#include "LTKCheckSumGenerate.h"

class LTKWordRecognizer;
class LTKRecognitionContext;


typedef LTKShapeRecognizer* (*FN_PTR_CREATESHAPERECOGNIZER)(void);
typedef int (*FN_PTR_DELETESHAPERECOGNIZER)(LTKShapeRecognizer*);


/**
* @class symbolRecognitionResult
* <p> this class holds the partial result shape recognition 
* and updates the result as more strokes are considered</p>
*/
struct DPSymbolRecognitionResult
{
	int m_styleID;
	int m_numTraces;
	int m_totalNumTraces;
	float m_confidence; 
	DPSymbolRecognitionResult(int styleID, float confidence, int numTraces, int totalNumTraces):
	m_styleID(styleID),m_confidence(confidence), m_numTraces(numTraces), m_totalNumTraces (totalNumTraces){}
	bool isCompleted() {return (m_numTraces == m_totalNumTraces);}
	~DPSymbolRecognitionResult(){}
	
};

/**
* @struct DPDecodedRecognitionResult
* <p> this structure holds the concatenated recognized symbols and the total confidence</p>
*/
struct DPDecodedRecognitionResult
{
	vector<int> m_symbolIDVec;	//holds concatenated symbol IDs

	float m_confidence;	//holds total confidence

	DPDecodedRecognitionResult()
	{
		m_confidence=0.0f;
	}
	
};

/**
* @class DPWordRecognizer
* <p> This class contains the implementation of the Dynamic Programming 
* based word recognizer</p>
*/

class DPWordRecognizer : public LTKWordRecognizer  
{
private:
	/**
	 * @name training and config files
	 */
	// @{
	string m_styleModelFile;		//	training filename 
	string m_positionModelFile;		//	positional training filename
	string m_dpConfigFile;			//	DP recognizer config file name
	string m_dictionaryConfigFile;	//  Dictionary config file name
	string m_grammarConfigFile;		//	Grammar config file name
	// @}

	LTKCheckSumGenerate cheSumGen;				// Holds the checksum info

	/**
	 * @name training data
	 */
	// @{
	int2DVector m_styleModels;	//holds different styles except the starting element of each style
	float3DVector m_positionModels;	//holds relative position of symbols and between traces of a symbol
	//intVector m_styleModelStartIDs;	//holds first shape ids of each style
	//vector<bool> m_styleModelStartSubset;	//bool vector for shape recognizer
	vector<bool> m_candidateShapeIDs;	//possible shape IDs determined from possible symbols from the grammar
	map<int,set<int> > m_symbolStartShapesMap;	//map that holds possible start shape IDs corresponding to each symbol
	intVector m_symbolIDs;	//vector for storing symbol IDs corresponding to each style
	multimap<int,int> m_startShapeStyleMap;	//map to hold starting shape ID and style index
	// @}

	string m_lipiRoot;				//  lipi root path
	string m_script;				//  script for recognition
	int m_numSymbols;				//	number of symbols
	//int m_totalNumTracesToRecognize;	//	total number of traces to be recognized in the current recognition unit

	/**
	 * @name shape recognizer related
	 */
	// @{
	LTKShapeRecognizer *m_shapeRecognizer;	//	shape recognizer
	string m_shapeRecoProject;
	string m_shapeRecoProfile;
	int m_numShapeRecoResults;              // num of results from shape recognizer
	float m_shapeRecoMinConfidence;         // the confidence parameter input to shape recognizer
	// @}

	/**
	 * @name state of recognizer
	 */
	// @{
	int m_numTracesProcessed;	                             //number of traces processed so far 
	vector<vector<DPSymbolRecognitionResult> > m_resultLattice; //maintains the alternate hypotheses 
	list<DPSymbolRecognitionResult> m_incompleteResults;          //maintains the "incomplete" shape recognizer outputs
	deque<vector<DPDecodedRecognitionResult> > m_decodedResults;	//maintains decoded recognition results after each trace, upto max style length
	int m_maxStyleLength;	//maximum style length
	
	// @}

	/**
	 * @name language models
	 */
	// @{
	//<TODO>: implement the grammar and dictionary members
	//LTKDictionary m_dictionary;
		LTKGrammar m_grammar;
		map<string,LTKDictionary> m_dictNameContentsMap;
		map<string,LTKDictionary>::iterator m_currDictPointer;
		int m_dictionaryMode;
	// @}


	/**
	 * @name Shape recognizer loading
	 */
	FN_PTR_CREATESHAPERECOGNIZER module_createShapeRecognizer;   //Factory method to create the shape recognizer
	FN_PTR_DELETESHAPERECOGNIZER module_deleteShapeRecognizer;   //Factory method to delete the shape recognizer


public:
	/**
	 * @name Constructors and Destructor
	 */
	// @{

	/**
	 * Default Constructor
	 */

	DPWordRecognizer();
    
	/**
	 * Initialization Constructor
	 */

	 DPWordRecognizer(const LTKWordRecoConfig& config);
	/**
	 *  Destructor 
	 */

   ~DPWordRecognizer();
	 // @}

   /**
	* This method loads the training Data of the derived class.
	*/

   	/**
	 * @name Word Recognition Interface
	 */
	// @{

     int initialize(string& strProjectName,string& strProfileName) ;
  
   /**
	* This method is called from recognition context whenever new traces
	* are added to it.  The Recognizer need to process the new traces 
	* in this methods and updates the internal state.
	* @param rc The recognition context for the current recognition
	*/
     int processInk (LTKRecognitionContext& rc) ;

   /**
	* This function notifies the recognizer that end of current ink is 
	* the end of a logic segment.  This information could be used in 
	* constraining the recognizer choices
	*/
     int endRecoUnit () ;
     
   /**
	* This is the recognize call. In case of trace by trace recognition
	* The results of the recognition is set on the Recognition context 
	* object.  
	* @param rc The recognition context for the current recognition
	*/
     int recognize (LTKRecognitionContext& rc) ;
 
   /**
	* This method reset the recognizer.  
	* @param resetParam This parameter could specify what to reset
	*/
     int reset (int resetParam = 0) ;
	
	// @}
private:
   /**
	* This method loads the training data  
	* @param void
	*/
	 int loadTrainingData();

	 /**
	 * This method performs the recognition task
	 * @param ink to be recognized 
	 */
	 int recognizeTraces(const vector<LTKTrace>& traces, LTKRecognitionContext& rc);

	 /**
	 * Clears the current state of the recognizer
	 * @param void
	 */
	 int clearRecognizerState();

	 /**
	 * This function tries to complete the
	 * existing hypothesis with the trace
	 * @param trace the new trace used for updation
	 */
	 int updateHypotheses(const LTKTrace& trace, LTKScreenContext screenContext, LTKCaptureDevice deviceContext);

	 /**
	 * This function tries to generate
	 * new  hypothesis starting with the trace
	 * @param trace the new trace used for generation
	 */
	 int generateNewHypotheses(const LTKTrace& trace, LTKScreenContext screenContext, LTKCaptureDevice deviceContext);


	 /**
	 * This function finds suitable paths after recognition of each trace
	 * @param recoContext recognition context
	 */
	 int decodeRecognitionPaths(LTKRecognitionContext& recoContext);

	 /**
	 * This function adds current decoded path to the vector and finds best N paths. Does insertion sort for newly added choice
	 * @param decodedResultVec vector containing decoded results with confidences
	 * @param decodedResult decoded path with the confidence
	 */
	 int constructTopNChoices(vector<DPDecodedRecognitionResult>& decodedResultVec, DPDecodedRecognitionResult& decodedResult);


	 LTKShapeRecognizer* createShapeRecognizer(string strProjectName, string strProfileName);

	 int mapShapeAlgoModuleFunctions();

	 int loadModelData();

	 int unloadModelData();

	 int getLastError();

};
#endif //#ifndef __DPWORDRECOGNIZER_H
